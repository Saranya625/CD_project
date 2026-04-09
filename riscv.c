/*
 * riscv.c  —  RISC-V (RV32IM) code generation backend
 * =====================================================
 *
 * INPUT  : output.ir   (textual TAC from ir.c / generate_ir)
 * OUTPUT : output.s    (RV32IM assembly, bare-metal / Spike / QEMU)
 *
 * PIPELINE
 * --------
 *  Pass 0  IR reader     – read every line from output.ir
 *  Pass 1  Symbol scan   – collect decl/decl_array/decl_matrix
 *  Pass 2  IR optimiser  – copy-prop, const-fold, dead-temp elim,
 *                          strength reduction, alias collapsing
 *  Pass 3  Code emission – IR → RV32IM with register-reuse cache
 *
 * MEMORY MODEL (LRM §7)
 * ----------------------
 *  Static allocation only.  All variables live in .data.
 *  Arrays  : base + index * 4
 *  Matrices: base + (row*cols + col) * 4   (row-major, §7.1)
 *  The IR already linearises 2-D indices, so from the backend's
 *  point of view every element access is 1-D.
 *
 * REGISTER STRATEGY
 * -----------------
 *  A 6-slot LRU register cache maps temporaries to scratch regs
 *  t0..t5.  Within a basic block, values already in a register are
 *  reused without reloading from .data.  The cache is flushed at
 *  every label / branch.  Register t6 is reserved as an address
 *  scratch for la+sw sequences.
 *
 * ECALL I/O  (bare-metal / Spike)
 * --------------------------------
 *   a7=1   print int     a0=value
 *   a7=4   print string  a0=address
 *   a7=5   read  int     result→a0
 *   a7=11  print char    a0=char code
 *   a7=10  exit
 */

#include "riscv.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================== */
/*  Compile-time limits                                                 */
/* ================================================================== */

#define MAX_LINES   8192
#define MAX_SYMS     512
#define MAX_TEMPS   4096
#define MAX_STRINGS  256
#define MAX_LINE     512
#define REG_POOL       6    /* t0..t5 */

/* ================================================================== */
/*  IR storage                                                          */
/* ================================================================== */

static char *g_lines[MAX_LINES];
static int   g_nlines = 0;

/* ================================================================== */
/*  Symbol table                                                        */
/* ================================================================== */

typedef enum { SYM_SCALAR, SYM_ARRAY, SYM_MATRIX } SymKind;

typedef struct {
    char    name[128];
    SymKind kind;
    int     elems;   /* 1 for scalar; rows*cols for matrix */
    int     cols;    /* matrix cols (0 for scalar/array)   */
} Symbol;

static Symbol g_syms[MAX_SYMS];
static int    g_nsyms = 0;

/* ================================================================== */
/*  String literal table                                                */
/* ================================================================== */

static struct {
    char label[32];
    char text[MAX_LINE];
} g_strtab[MAX_STRINGS];
static int g_nstrs = 0;

/* ================================================================== */
/*  Per-temp optimisation metadata                                      */
/* ================================================================== */

typedef struct {
    int       has_const;   /* is a known integer constant              */
    long long cval;        /* constant value                           */
    int       alias_of;   /* >0: copy of that temp (after resolve)     */
} TempMeta;

static TempMeta g_tm[MAX_TEMPS + 1];
static int      g_max_temp = 0;

/* ================================================================== */
/*  Register cache                                                      */
/* ================================================================== */

static int g_rcache[MAX_TEMPS + 1]; /* temp → slot index or -1        */
static int g_rowner[REG_POOL];      /* slot → temp number or 0        */
static int g_rlru  [REG_POOL];      /* LRU counter per slot           */
static int g_lru_clk = 0;

static const char *g_rnames[REG_POOL] = {
    "t0","t1","t2","t3","t4","t5"
};

/* ================================================================== */
/*  Generic helpers                                                     */
/* ================================================================== */

static const char *skip_ws(const char *s)
{
    while (s && isspace((unsigned char)*s)) s++;
    return s;
}

static void rtrim(char *s)
{
    int n = (int)strlen(s);
    while (n > 0 && isspace((unsigned char)s[n-1])) s[--n] = '\0';
}

static int is_imm(const char *s)
{
    if (!s || !*s) return 0;
    const char *p = s;
    if (*p == '-') p++;
    if (!isdigit((unsigned char)*p)) return 0;
    while (*p) { if (!isdigit((unsigned char)*p)) return 0; p++; }
    return 1;
}

static int parse_tmp(const char *s, int *out)
{
    if (!s || s[0] != 't' || !isdigit((unsigned char)s[1])) return 0;
    int n = atoi(s + 1);
    if (n <= 0 || n > MAX_TEMPS) return 0;
    *out = n;
    if (n > g_max_temp) g_max_temp = n;
    return 1;
}

static void note_temp_in_line(const char *ln)
{
    while (*ln) {
        if (*ln == 't' && isdigit((unsigned char)ln[1])) {
            int n = atoi(ln + 1);
            if (n > 0 && n <= MAX_TEMPS && n > g_max_temp)
                g_max_temp = n;
        }
        ln++;
    }
}

static void replace_line(int i, const char *fmt, ...)
{
    char buf[MAX_LINE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    free(g_lines[i]);
    g_lines[i] = strdup(buf);
}

/* ================================================================== */
/*  Pass 0 — Read IR                                                    */
/* ================================================================== */

static int read_ir(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return 0; }
    char buf[MAX_LINE];
    g_nlines = 0;
    while (fgets(buf, sizeof(buf), f) && g_nlines < MAX_LINES) {
        int n = (int)strlen(buf);
        while (n > 0 && (buf[n-1]=='\n'||buf[n-1]=='\r')) buf[--n]='\0';
        rtrim(buf);
        const char *p = skip_ws(buf);
        if (p && *p) g_lines[g_nlines++] = strdup(p);
    }
    fclose(f);
    return 1;
}

static void free_ir(void)
{
    for (int i = 0; i < g_nlines; i++) { free(g_lines[i]); g_lines[i]=NULL; }
    g_nlines = 0;
}

/* ================================================================== */
/*  String registration                                                 */
/* ================================================================== */

static void extract_str(const char *src, char *dst, int dstsz)
{
    const char *p = strchr(src, '"');
    if (!p) { *dst='\0'; return; }
    p++;
    int i = 0;
    while (*p && *p != '"' && i < dstsz-1) dst[i++] = *p++;
    dst[i] = '\0';
}

static const char *reg_str(const char *text)
{
    for (int i = 0; i < g_nstrs; i++)
        if (strcmp(g_strtab[i].text, text)==0) return g_strtab[i].label;
    if (g_nstrs >= MAX_STRINGS) return ".Ls_err";
    snprintf(g_strtab[g_nstrs].label, 32, ".Ls%d", g_nstrs);
    strncpy(g_strtab[g_nstrs].text, text, MAX_LINE-1);
    return g_strtab[g_nstrs++].label;
}

/* ================================================================== */
/*  Pass 1 — Symbol collection                                          */
/* ================================================================== */

static Symbol *sym_find(const char *n)
{
    for (int i=0;i<g_nsyms;i++)
        if (strcmp(g_syms[i].name,n)==0) return &g_syms[i];
    return NULL;
}
static void sym_add(const char *n, SymKind k, int elems, int cols)
{
    if (sym_find(n)||g_nsyms>=MAX_SYMS) return;
    Symbol *s = &g_syms[g_nsyms++];
    strncpy(s->name,n,127); s->kind=k; s->elems=elems; s->cols=cols;
}

static void pass1_collect(void)
{
    char name[128]; int rows,cols,sz;
    g_nsyms=g_nstrs=g_max_temp=0;
    memset(g_tm,0,sizeof(g_tm));

    for (int i=0;i<g_nlines;i++) {
        const char *ln=g_lines[i];
        if (sscanf(ln,"decl %*s %127s",name)==1)
            { sym_add(name,SYM_SCALAR,1,0); continue; }
        if (sscanf(ln,"decl_array %127[^,], %d",name,&sz)==2)
            { sym_add(name,SYM_ARRAY,sz,0); continue; }
        if (sscanf(ln,"decl_matrix %127[^,], %d, %d",name,&rows,&cols)==3)
            { sym_add(name,SYM_MATRIX,rows*cols,cols); continue; }
        if (strncmp(ln,"print_str ",10)==0) {
            char t[MAX_LINE]; extract_str(ln+10,t,sizeof(t)); reg_str(t); continue;
        }
        note_temp_in_line(ln);
    }
}

/* ================================================================== */
/*  Pass 2 — IR optimiser                                               */
/* ================================================================== */

/* Resolve alias chain */
static int resolve(int t)
{
    int g=0;
    while (t>0&&t<=g_max_temp&&g_tm[t].alias_of>0&&g<64)
        { t=g_tm[t].alias_of; g++; }
    return t;
}

static long long fold2(const char *op, long long a, long long b, int *ok)
{
    *ok=1;
    if (!strcmp(op,"+"))   return a+b;
    if (!strcmp(op,"-"))   return a-b;
    if (!strcmp(op,"*"))   return a*b;
    if (!strcmp(op,"/"))   { if(!b){*ok=0;return 0;} return a/b; }
    if (!strcmp(op,"%"))   { if(!b){*ok=0;return 0;} return a%b; }
    if (!strcmp(op,"eq"))  return a==b;
    if (!strcmp(op,"ne"))  return a!=b;
    if (!strcmp(op,"lt"))  return a<b;
    if (!strcmp(op,"gt"))  return a>b;
    if (!strcmp(op,"le"))  return a<=b;
    if (!strcmp(op,"ge"))  return a>=b;
    if (!strcmp(op,"and")) return (a!=0)&&(b!=0);
    if (!strcmp(op,"or"))  return (a!=0)||(b!=0);
    *ok=0; return 0;
}

static void flush_tm(void) { memset(g_tm,0,sizeof(g_tm)); }

/* Name→constant tracking (scalar variables) */
typedef struct { char n[128]; long long v; int ok; } NC;
static NC g_nc[MAX_SYMS];
static int g_nnc=0;

static void nc_set(const char *n, long long v) {
    for (int i=0;i<g_nnc;i++) if (!strcmp(g_nc[i].n,n)){g_nc[i].v=v;g_nc[i].ok=1;return;}
    if (g_nnc<MAX_SYMS){strncpy(g_nc[g_nnc].n,n,127);g_nc[g_nnc].v=v;g_nc[g_nnc++].ok=1;}
}
static int nc_get(const char *n, long long *v) {
    for (int i=0;i<g_nnc;i++) if(g_nc[i].ok&&!strcmp(g_nc[i].n,n)){*v=g_nc[i].v;return 1;}
    return 0;
}
static void nc_clr(const char *n) {
    for (int i=0;i<g_nnc;i++) if(!strcmp(g_nc[i].n,n)){g_nc[i].ok=0;return;}
}

static void pass2_opt(void)
{
    flush_tm(); g_nnc=0;

    int def,src,lhs,rhs,addr,val,label;
    char kind[32],sval[128],op[32],name[128];

    /* ── forward pass: const-fold + copy-prop + strength-reduction ── */
    for (int i=0;i<g_nlines;i++) {
        const char *ln=g_lines[i];

        /* label: flush all temp knowledge */
        if (sscanf(ln,"label L%d",&def)==1) { flush_tm(); g_nnc=0; continue; }

        /* tN = const K */
        if (sscanf(ln,"t%d = const %31s %127s",&def,kind,sval)==3) {
            if (!strcmp(kind,"int")&&is_imm(sval)) {
                g_tm[def].has_const=1; g_tm[def].cval=atoll(sval);
                g_tm[def].alias_of=0;
            }
            continue;
        }

        /* tN = addr name */
        if (sscanf(ln,"t%d = addr %127s",&def,name)==2) {
            g_tm[def].has_const=0; g_tm[def].alias_of=0; continue;
        }

        /* tN = load name */
        if (sscanf(ln,"t%d = load %127s",&def,name)==2 && name[0]!='[') {
            long long cv;
            if (nc_get(name,&cv)) {
                replace_line(i,"t%d = const int %lld",def,cv);
                g_tm[def].has_const=1; g_tm[def].cval=cv; g_tm[def].alias_of=0;
            } else {
                g_tm[def].has_const=0; g_tm[def].alias_of=0;
            }
            continue;
        }

        /* tN = load [tM] */
        if (sscanf(ln,"t%d = load [t%d]",&def,&src)==2) {
            g_tm[def].has_const=0; g_tm[def].alias_of=0; continue;
        }

        /* tN = mov tM */
        if (sscanf(ln,"t%d = mov t%d",&def,&src)==2) {
            int rs=resolve(src);
            if (rs>0&&rs<=g_max_temp&&g_tm[rs].has_const) {
                replace_line(i,"t%d = const int %lld",def,g_tm[rs].cval);
                g_tm[def].has_const=1; g_tm[def].cval=g_tm[rs].cval;
                g_tm[def].alias_of=0;
            } else {
                if (rs!=src) replace_line(i,"t%d = mov t%d",def,rs);
                g_tm[def].alias_of=rs; g_tm[def].has_const=0;
            }
            continue;
        }

        /* store [tA], tB  → update name const map */
        if (sscanf(ln,"store [t%d], t%d",&addr,&val)==2) {
            int ar=resolve(addr), vr=resolve(val);
            if (ar!=addr||vr!=val) replace_line(i,"store [t%d], t%d",ar,vr);
            /* track scalar name if addr came from an "addr name" temp */
            /* (simple heuristic: the immediately preceding addr line) */
            if (i>0) {
                char aname[128]; int at;
                if (sscanf(g_lines[i-1],"t%d = addr %127s",&at,aname)==2
                    && at==ar) {
                    if (vr>0&&vr<=g_max_temp&&g_tm[vr].has_const)
                        nc_set(aname, g_tm[vr].cval);
                    else
                        nc_clr(aname);
                }
            }
            continue;
        }

        /* tN = op tA, tB */
        if (sscanf(ln,"t%d = %31s t%d, t%d",&def,op,&lhs,&rhs)==4) {
            int lr=resolve(lhs), rr=resolve(rhs);
            if (lr!=lhs||rr!=rhs) replace_line(i,"t%d = %s t%d, t%d",def,op,lr,rr);

            /* try full constant fold */
            if (g_tm[lr].has_const&&g_tm[rr].has_const) {
                int ok; long long res=fold2(op,g_tm[lr].cval,g_tm[rr].cval,&ok);
                if (ok) {
                    replace_line(i,"t%d = const int %lld",def,res);
                    g_tm[def].has_const=1; g_tm[def].cval=res; g_tm[def].alias_of=0;
                    continue;
                }
            }
            /* strength reduction */
            long long rv=g_tm[rr].has_const?g_tm[rr].cval:0;
            long long lv=g_tm[lr].has_const?g_tm[lr].cval:0;
            int rconst=g_tm[rr].has_const, lconst=g_tm[lr].has_const;
            if (!strcmp(op,"+")) {
                if (rconst&&rv==0){replace_line(i,"t%d = mov t%d",def,lr);g_tm[def].alias_of=lr;g_tm[def].has_const=0;continue;}
                if (lconst&&lv==0){replace_line(i,"t%d = mov t%d",def,rr);g_tm[def].alias_of=rr;g_tm[def].has_const=0;continue;}
            }
            if (!strcmp(op,"-")) {
                if (rconst&&rv==0){replace_line(i,"t%d = mov t%d",def,lr);g_tm[def].alias_of=lr;g_tm[def].has_const=0;continue;}
            }
            if (!strcmp(op,"*")) {
                if (rconst&&rv==1){replace_line(i,"t%d = mov t%d",def,lr);g_tm[def].alias_of=lr;g_tm[def].has_const=0;continue;}
                if (lconst&&lv==1){replace_line(i,"t%d = mov t%d",def,rr);g_tm[def].alias_of=rr;g_tm[def].has_const=0;continue;}
                if ((rconst&&rv==0)||(lconst&&lv==0)){replace_line(i,"t%d = const int 0",def);g_tm[def].has_const=1;g_tm[def].cval=0;g_tm[def].alias_of=0;continue;}
            }
            if (!strcmp(op,"/")&&rconst&&rv==1){replace_line(i,"t%d = mov t%d",def,lr);g_tm[def].alias_of=lr;g_tm[def].has_const=0;continue;}
            g_tm[def].has_const=0; g_tm[def].alias_of=0;
            continue;
        }

        /* tN = op tA  (unary) */
        if (sscanf(ln,"t%d = %31s t%d",&def,op,&src)==3) {
            int sr=resolve(src);
            if (sr!=src) replace_line(i,"t%d = %s t%d",def,op,sr);
            if (!strcmp(op,"uminus")&&g_tm[sr].has_const) {
                replace_line(i,"t%d = const int %lld",def,-g_tm[sr].cval);
                g_tm[def].has_const=1; g_tm[def].cval=-g_tm[sr].cval;
            } else if (!strcmp(op,"not")&&g_tm[sr].has_const) {
                long long r=g_tm[sr].cval?0:1;
                replace_line(i,"t%d = const int %lld",def,r);
                g_tm[def].has_const=1; g_tm[def].cval=r;
            } else {
                g_tm[def].has_const=0; g_tm[def].alias_of=0;
            }
            continue;
        }

        /* ifz: resolve + constant condition */
        if (sscanf(ln,"ifz t%d goto L%d",&src,&def)==2) {
            int sr=resolve(src);
            if (g_tm[sr].has_const) {
                if (g_tm[sr].cval==0) replace_line(i,"goto L%d",def);
                else                   replace_line(i,"# [const ifz removed]");
            } else if (sr!=src) {
                replace_line(i,"ifz t%d goto L%d",sr,def);
            }
            flush_tm(); g_nnc=0;
            continue;
        }

        /* print / ret / scan: resolve operands */
        if (sscanf(ln,"print t%d",&src)==1) {
            int sr=resolve(src);
            if (sr!=src) replace_line(i,"print t%d",sr);
            continue;
        }
        if (sscanf(ln,"ret t%d",&src)==1) {
            int sr=resolve(src);
            if (sr!=src) replace_line(i,"ret t%d",sr);
            continue;
        }
        if (sscanf(ln,"scan [t%d]",&addr)==1) {
            int ar=resolve(addr);
            if (ar!=addr) replace_line(i,"scan [t%d]",ar);
            continue;
        }
    }

    /* ── dead-temp elimination ─────────────────────────────────── */
    static int use_cnt[MAX_TEMPS+1];
    static int def_ln [MAX_TEMPS+1];
    memset(use_cnt,0,sizeof(int)*(g_max_temp+1));
    for (int j=0;j<=g_max_temp;j++) def_ln[j]=-1;

    for (int i=0;i<g_nlines;i++) {
        const char *ln=g_lines[i]; if(!ln||!*ln) continue;
        int d=0;
        if (sscanf(ln,"t%d =",&d)==1) def_ln[d]=i;
        /* count uses of every tN (skip the lhs def position) */
        const char *p=ln;
        int past_eq=0;
        while (*p) {
            if (*p=='='&&!past_eq) { past_eq=1; }
            if (past_eq && *p=='t' && isdigit((unsigned char)p[1])) {
                int n=atoi(p+1);
                if (n>0&&n<=g_max_temp) use_cnt[n]++;
                while (isdigit((unsigned char)p[1])) p++;
            }
            p++;
        }
    }

    for (int t=1;t<=g_max_temp;t++) {
        if (use_cnt[t]==0 && def_ln[t]>=0) {
            const char *ln=g_lines[def_ln[t]];
            /* only remove pure defs (no side effects) */
            int pure=0;
            { int d,s1,s2; char k[32],v[128],o[32],n[128];
              if (sscanf(ln,"t%d = const %s %s",&d,k,v)==3)       pure=1;
              if (sscanf(ln,"t%d = mov t%d",&d,&s1)==2)            pure=1;
              if (sscanf(ln,"t%d = %31s t%d, t%d",&d,o,&s1,&s2)==4) pure=1;
              if (sscanf(ln,"t%d = load %127s",&d,n)==2&&n[0]!='[') pure=1;
              if (sscanf(ln,"t%d = %31s t%d",&d,o,&s1)==3
                  &&strcmp(o,"addr")&&strcmp(o,"load"))              pure=1;
            }
            if (pure) { free(g_lines[def_ln[t]]); g_lines[def_ln[t]]=strdup("# [dead]"); }
        }
    }
}

/* ================================================================== */
/*  Pass 3 — Code emission                                              */
/* ================================================================== */

/* ── Register cache ops ────────────────────────────────────────── */

static void rc_flush(void)
{
    memset(g_rcache,-1,sizeof(g_rcache));
    memset(g_rowner,0, sizeof(g_rowner));
    memset(g_rlru,  0, sizeof(g_rlru));
    g_lru_clk=0;
}

static const char *rc_of(int t)
{
    if (t<=0||t>MAX_TEMPS) return NULL;
    int s=g_rcache[t]; if(s<0) return NULL;
    return g_rnames[s];
}

static int rc_evict(void)
{
    int oldest=0;
    for (int r=1;r<REG_POOL;r++) if(g_rlru[r]<g_rlru[oldest]) oldest=r;
    if (g_rowner[oldest]>0) g_rcache[g_rowner[oldest]]=-1;
    g_rowner[oldest]=0;
    return oldest;
}

static int rc_alloc(int t)
{
    if (g_rcache[t]>=0) { g_rlru[g_rcache[t]]=++g_lru_clk; return g_rcache[t]; }
    int slot=-1;
    for (int r=0;r<REG_POOL;r++) if(!g_rowner[r]){slot=r;break;}
    if (slot<0) slot=rc_evict();
    if (g_rowner[slot]) g_rcache[g_rowner[slot]]=-1;
    g_rowner[slot]=t; g_rcache[t]=slot; g_rlru[slot]=++g_lru_clk;
    return slot;
}

/* ── Emission helpers ──────────────────────────────────────────── */

/*
 * Load temp t into a scratch register.
 * Returns the register name.
 * Emits "li reg, K"  for known constants.
 * Emits "lw reg, t_N" for memory-resident temps.
 * Reuses cached register if already loaded.
 */
static const char *ld_tmp(int t, FILE *out)
{
    const char *c = rc_of(t);
    if (c) return c;
    int slot = rc_alloc(t);
    const char *r = g_rnames[slot];
    if (g_tm[t].has_const)
        fprintf(out,"\tli\t%s, %lld\n", r, g_tm[t].cval);
    else
        fprintf(out,"\tlw\t%s, t_%d\n", r, t);
    return r;
}

/*
 * Store register reg into temp t's .data slot.
 * Uses:  la t6, t_N  /  sw reg, 0(t6)
 * This is the correct, portable sequence (no non-standard pseudo).
 */
static void st_tmp(int t, const char *reg, FILE *out)
{
    fprintf(out,"\tla\tt6, t_%d\n", t);
    fprintf(out,"\tsw\t%s, 0(t6)\n", reg);
    /* pin the result in the cache */
    for (int r=0;r<REG_POOL;r++) {
        if (g_rnames[r]==reg) {
            if (g_rowner[r]&&g_rowner[r]!=t) g_rcache[g_rowner[r]]=-1;
            g_rowner[r]=t; g_rcache[t]=r; g_rlru[r]=++g_lru_clk;
            break;
        }
    }
}

static void ld_name(const char *name, const char *reg, FILE *out)
{
    fprintf(out,"\tla\tt6, %s\n", name);
    fprintf(out,"\tlw\t%s, 0(t6)\n", reg);
}

static void st_name(const char *name, const char *reg, FILE *out)
{
    fprintf(out,"\tla\tt6, %s\n", name);
    fprintf(out,"\tsw\t%s, 0(t6)\n", reg);
}

/* ── Single IR line → assembly ─────────────────────────────────── */

static void emit_one(const char *ln, FILE *out)
{
    int def,src,lhs,rhs,addr,val,label;
    char kind[32],sval[128],op[32],name[128];

    if (!ln || !*ln || ln[0]=='#') return;

    if (!strcmp(ln,"IR_BEGIN")||!strcmp(ln,"IR_END"))
        { fprintf(out,"\t# %s\n",ln); return; }

    /* ── label ─────────────────────────────────────────────────── */
    if (sscanf(ln,"label L%d",&label)==1) {
        rc_flush();
        fprintf(out,"L%d:\n",label); return;
    }

    /* ── goto ──────────────────────────────────────────────────── */
    if (sscanf(ln,"goto L%d",&label)==1) {
        rc_flush();
        fprintf(out,"\tj\tL%d\n",label); return;
    }

    /* ── ifz tA goto LN ─────────────────────────────────────────── */
    if (sscanf(ln,"ifz t%d goto L%d",&src,&label)==2) {
        const char *r=ld_tmp(src,out);
        fprintf(out,"\tbeqz\t%s, L%d\n",r,label);
        rc_flush(); return;
    }

    /* ── ret ────────────────────────────────────────────────────── */
    if (!strcmp(ln,"ret")) {
        fprintf(out,"\tli\ta0, 0\n\tj\t.Lexit\n");
        rc_flush(); return;
    }
    if (sscanf(ln,"ret t%d",&src)==1) {
        const char *r=ld_tmp(src,out);
        if (strcmp(r,"a0")) fprintf(out,"\tmv\ta0, %s\n",r);
        fprintf(out,"\tj\t.Lexit\n");
        rc_flush(); return;
    }

    /* ── print_str ──────────────────────────────────────────────── */
    if (!strncmp(ln,"print_str ",10)) {
        char txt[MAX_LINE]; extract_str(ln+10,txt,sizeof(txt));
        const char *lbl=reg_str(txt);
        fprintf(out,"\tla\ta0, %s\n\tli\ta7, 4\n\tecall\n",lbl);
        return;
    }

    /* ── print tN ───────────────────────────────────────────────── */
    if (sscanf(ln,"print t%d",&src)==1) {
        const char *r=ld_tmp(src,out);
        fprintf(out,"\tmv\ta0, %s\n\tli\ta7, 1\n\tecall\n",r);
        fprintf(out,"\tli\ta0, 10\n\tli\ta7, 11\n\tecall\n");
        return;
    }

    /* ── scan [tN] ──────────────────────────────────────────────── */
    if (sscanf(ln,"scan [t%d]",&addr)==1) {
        const char *ra=ld_tmp(addr,out);
        fprintf(out,"\tli\ta7, 5\n\tecall\n\tsw\ta0, 0(%s)\n",ra);
        return;
    }

    /* ── decl* ──────────────────────────────────────────────────── */
    if (!strncmp(ln,"decl",4)) { fprintf(out,"\t# %s\n",ln); return; }

    /* ── store [tA], tB ─────────────────────────────────────────── */
    if (sscanf(ln,"store [t%d], t%d",&addr,&val)==2) {
        const char *ra=ld_tmp(addr,out);
        const char *rv=ld_tmp(val, out);
        /* if eviction aliased them, reload addr into t6 */
        if (!strcmp(ra,rv)) {
            fprintf(out,"\tmv\tt6, %s\n",ra); ra="t6";
        }
        fprintf(out,"\tsw\t%s, 0(%s)\n",rv,ra);
        return;
    }

    /* ── tN = const K ────────────────────────────────────────────── */
    if (sscanf(ln,"t%d = const %31s %127s",&def,kind,sval)==3) {
        int slot=rc_alloc(def);
        const char *r=g_rnames[slot];
        if (!strcmp(kind,"int")&&is_imm(sval)) {
            long long v=atoll(sval);
            fprintf(out,"\tli\t%s, %lld\n",r,v);
            g_tm[def].has_const=1; g_tm[def].cval=v;
        } else {
            fprintf(out,"\tli\t%s, %ld\t# %s %s\n",r,(long)atof(sval),kind,sval);
        }
        return;
    }

    /* ── tN = addr <name> ─────────────────────────────────────── */
    if (sscanf(ln,"t%d = addr %127s",&def,name)==2) {
        int slot=rc_alloc(def);
        fprintf(out,"\tla\t%s, %s\n",g_rnames[slot],name);
        return;
    }

    /* ── tN = load <name>  (scalar) ─────────────────────────── */
    if (sscanf(ln,"t%d = load %127s",&def,name)==2 && name[0]!='[') {
        int slot=rc_alloc(def);
        const char *r=g_rnames[slot];
        ld_name(name,r,out);
        return;
    }

    /* ── tN = load [tM] ─────────────────────────────────────────── */
    if (sscanf(ln,"t%d = load [t%d]",&def,&addr)==2) {
        const char *ra=ld_tmp(addr,out);
        int slot=rc_alloc(def);
        const char *rd=g_rnames[slot];
        if (!strcmp(ra,rd)) {
            fprintf(out,"\tmv\tt6, %s\n",ra);
            fprintf(out,"\tlw\t%s, 0(t6)\n",rd);
        } else {
            fprintf(out,"\tlw\t%s, 0(%s)\n",rd,ra);
        }
        g_tm[def].has_const=0;
        return;
    }

    /* ── tN = mov tM ─────────────────────────────────────────────── */
    if (sscanf(ln,"t%d = mov t%d",&def,&src)==2) {
        const char *rs=ld_tmp(src,out);
        int slot=rc_alloc(def);
        const char *rd=g_rnames[slot];
        if (strcmp(rs,rd)) fprintf(out,"\tmv\t%s, %s\n",rd,rs);
        if (g_tm[src].has_const) { g_tm[def].has_const=1; g_tm[def].cval=g_tm[src].cval; }
        return;
    }

    /* ── tN = <op> tA, tB ────────────────────────────────────────── */
    if (sscanf(ln,"t%d = %31s t%d, t%d",&def,op,&lhs,&rhs)==4) {
        const char *r1=ld_tmp(lhs,out);
        const char *r2=ld_tmp(rhs,out);

        /* handle same-register operands */
        char r2b[8]; strncpy(r2b,r2,7);
        if (!strcmp(r1,r2)) { fprintf(out,"\tmv\tt6, %s\n",r2); strncpy(r2b,"t6",7); }

        int slot=rc_alloc(def);
        const char *rd=g_rnames[slot];
        /* re-check r1 after alloc may have evicted */
        r1=ld_tmp(lhs,out);
        if (!strcmp(r1,r2b)) { fprintf(out,"\tmv\tt6, %s\n",r1); r1="t6"; }

        if      (!strcmp(op,"+")||!strcmp(op,"add"))
            fprintf(out,"\tadd\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"-")||!strcmp(op,"sub"))
            fprintf(out,"\tsub\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"*")||!strcmp(op,"mul"))
            fprintf(out,"\tmul\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"/"))
            fprintf(out,"\tdiv\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"%"))
            fprintf(out,"\trem\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"eq")) {
            fprintf(out,"\tsub\t%s, %s, %s\n",rd,r1,r2b);
            fprintf(out,"\tseqz\t%s, %s\n",rd,rd); }
        else if (!strcmp(op,"ne")) {
            fprintf(out,"\tsub\t%s, %s, %s\n",rd,r1,r2b);
            fprintf(out,"\tsnez\t%s, %s\n",rd,rd); }
        else if (!strcmp(op,"lt"))
            fprintf(out,"\tslt\t%s, %s, %s\n",rd,r1,r2b);
        else if (!strcmp(op,"gt"))
            fprintf(out,"\tslt\t%s, %s, %s\n",rd,r2b,r1);
        else if (!strcmp(op,"le")) {
            fprintf(out,"\tslt\t%s, %s, %s\n",rd,r2b,r1);
            fprintf(out,"\txori\t%s, %s, 1\n",rd,rd); }
        else if (!strcmp(op,"ge")) {
            fprintf(out,"\tslt\t%s, %s, %s\n",rd,r1,r2b);
            fprintf(out,"\txori\t%s, %s, 1\n",rd,rd); }
        else if (!strcmp(op,"and")) {
            fprintf(out,"\tsnez\t%s, %s\n",rd,r1);
            fprintf(out,"\tsnez\tt6, %s\n",r2b);
            fprintf(out,"\tand\t%s, %s, t6\n",rd,rd); }
        else if (!strcmp(op,"or")) {
            fprintf(out,"\tor\t%s, %s, %s\n",rd,r1,r2b);
            fprintf(out,"\tsnez\t%s, %s\n",rd,rd); }
        else fprintf(out,"\t# [?binop] %s\n",ln);

        g_tm[def].has_const=0;
        return;
    }

    /* ── tN = <op> tA  (unary) ──────────────────────────────────── */
    if (sscanf(ln,"t%d = %31s t%d",&def,op,&src)==3) {
        const char *rs=ld_tmp(src,out);
        int slot=rc_alloc(def);
        const char *rd=g_rnames[slot];
        if (!strcmp(op,"uminus"))     fprintf(out,"\tneg\t%s, %s\n",rd,rs);
        else if (!strcmp(op,"not"))   fprintf(out,"\tseqz\t%s, %s\n",rd,rs);
        else fprintf(out,"\t# [?unary] %s\n",ln);
        g_tm[def].has_const=0;
        return;
    }

    fprintf(out,"\t# [unhandled] %s\n",ln);
}

/* ================================================================== */
/*  Public API implementations                                          */
/* ================================================================== */

void generate_data_section(FILE *out)
{
    fprintf(out,"\t.data\n\t.align\t2\n\n");
    for (int i=0;i<g_nsyms;i++) {
        Symbol *s=&g_syms[i];
        if (s->kind==SYM_SCALAR)
            fprintf(out,"%s:\t.word\t0\n",s->name);
        else
            fprintf(out,"%s:\t.space\t%d\t# %d words\n",
                    s->name,s->elems*4,s->elems);
    }
    if (g_max_temp>0) {
        fprintf(out,"\n\t# temporaries\n");
        for (int t=1;t<=g_max_temp;t++)
            fprintf(out,"t_%d:\t.word\t0\n",t);
    }
    if (g_nstrs>0) {
        fprintf(out,"\n\t# strings\n");
        for (int i=0;i<g_nstrs;i++)
            fprintf(out,"%s:\t.asciz\t\"%s\"\n",
                    g_strtab[i].label,g_strtab[i].text);
    }
    fprintf(out,"\n");
}

void generate_text_header(FILE *out)
{
    fprintf(out,"\t.text\n\t.globl\tmain\n\t.type\tmain, @function\nmain:\n");
}

void generate_exit(FILE *out)
{
    fprintf(out,"\n.Lexit:\n\tli\ta7, 10\n\tecall\n\t.size\tmain, .-main\n");
}

/* Stubs for per-instruction public functions (not used by main flow) */
void generate_instruction(const char *l,FILE *o){emit_one(l,o);}
void generate_assignment(const char*a,const char*b,FILE*o){(void)a;(void)b;(void)o;}
void generate_arithmetic(const char*a,const char*b,const char*c,const char*d,FILE*o)
    {(void)a;(void)b;(void)c;(void)d;(void)o;}
void generate_compare(const char*a,const char*b,const char*c,const char*d,FILE*o)
    {(void)a;(void)b;(void)c;(void)d;(void)o;}
void generate_if(const char*a,const char*b,FILE*o){(void)a;(void)b;(void)o;}
void generate_goto(const char*a,FILE*o){(void)a;(void)o;}
void generate_label(const char*a,FILE*o){(void)a;(void)o;}
void generate_array_load(const char*a,const char*b,FILE*o){(void)a;(void)b;(void)o;}
void generate_array_store(const char*a,const char*b,FILE*o){(void)a;(void)b;(void)o;}
void generate_print(const char*a,FILE*o){(void)a;(void)o;}
void generate_scan(const char*a,FILE*o){(void)a;(void)o;}
void generate_return(const char*a,FILE*o){(void)a;(void)o;}

/* ================================================================== */
/*  Top-level entry                                                     */
/* ================================================================== */

int generate_riscv(const char *ir_file, const char *asm_file)
{
    if (!read_ir(ir_file)) return -1;
    pass1_collect();
    pass2_opt();

    FILE *out=fopen(asm_file,"w");
    if (!out) { perror(asm_file); free_ir(); return -1; }

    fprintf(out,"# RISC-V (RV32IM) — compiler project backend\n");
    fprintf(out,"# Source IR : %s\n\n",ir_file);

    generate_data_section(out);
    generate_text_header(out);

    rc_flush();
    for (int i=0;i<g_nlines;i++) {
        const char *ln=g_lines[i];
        if (!ln||!*ln) continue;
        /* print IR as inline comment for readability */
        if (ln[0]!='#') fprintf(out,"\t\t\t# %s\n",ln);
        emit_one(ln,out);
    }

    generate_exit(out);
    fclose(out);
    free_ir();
    printf("RISC-V assembly written to '%s'\n",asm_file);
    return 0;
}