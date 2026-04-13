#include "ir.h"
#include "ir_internal.h"
#include "cd_util.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

struct IRContext {
    int temp_id;
    int label_id;
    int indent;
    struct MatrixInfo *matrices;
    struct ArrayInfo *arrays;
    struct ConstBinding *consts;
    struct ExprCache *cse_cache;
    struct MatExprCache *mat_cse_cache;
    int capture;
    char **lines;
    int line_count;
    int line_cap;
    IROptLevel opt_level;
};

typedef struct MatrixInfo {
    char *name;
    int rows;
    int cols;
    int props_known;
    int is_zero;
    int is_identity;
    int is_diagonal;
    int is_symmetric;
    int is_upper_triangular;
    int is_lower_triangular;
    struct MatrixInfo *next;
} MatrixInfo;

typedef struct ArrayInfo {
    char *name;
    int size;
    struct ArrayInfo *next;
} ArrayInfo;

typedef enum {
    CONST_NONE = 0,
    CONST_INT,
    CONST_DECIMAL,
    CONST_CHAR,
    CONST_STRING
} ConstKind;

typedef struct ConstVal {
    ConstKind kind;
    long long i;
    double d;
    const char *s;
} ConstVal;

typedef struct ConstBinding {
    char *name;
    ConstVal val;
    struct ConstBinding *next;
} ConstBinding;

typedef struct ExprCache {
    char *key;
    int temp;
    struct ExprCache *next;
} ExprCache;

typedef struct MatExprCache {
    char *key;
    char *name;
    struct MatExprCache *next;
} MatExprCache;

typedef struct {
    ConstKind kind;
    long long i;
    double d;
    char *s;
} IRConst;

typedef struct NameInfo {
    char *name;
    int has_const;
    IRConst val;
    int has_copy;
    char *copy_name;
    int has_last_store;
    int last_store_temp;
    struct NameInfo *next;
} NameInfo;

typedef struct {
    int has_const;
    IRConst val;
    int has_copy;
    int copy_temp;
    char *name_src;
} TempInfo;

static void ir_lines_push(IRContext *ctx, char *line)
{
    if (!ctx) {
        free(line);
        return;
    }
    if (ctx->line_count == ctx->line_cap) {
        int new_cap = ctx->line_cap == 0 ? 128 : ctx->line_cap * 2;
        char **next = realloc(ctx->lines, (size_t) new_cap * sizeof(char *));
        if (!next) {
            free(line);
            return;
        }
        ctx->lines = next;
        ctx->line_cap = new_cap;
    }
    ctx->lines[ctx->line_count++] = line;
}

static void ir_lines_free(IRContext *ctx)
{
    if (!ctx) {
        return;
    }
    for (int i = 0; i < ctx->line_count; i++) {
        free(ctx->lines[i]);
    }
    free(ctx->lines);
    ctx->lines = NULL;
    ctx->line_count = 0;
    ctx->line_cap = 0;
}

void ir_printf(IRContext *ctx, FILE *out, const char *fmt, ...)
{
    va_list args;
    va_list copy;
    int len = 0;
    int prefix = 0;
    char *tmp = NULL;
    char *buf = NULL;

    if (!ctx) {
        return;
    }
    prefix = ctx->indent * 2;
    va_start(args, fmt);
    va_copy(copy, args);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (len < 0) {
        va_end(copy);
        return;
    }
    tmp = malloc((size_t) len + 1);
    if (!tmp) {
        va_end(copy);
        return;
    }
    vsnprintf(tmp, (size_t) len + 1, fmt, copy);
    va_end(copy);

    if (ctx->capture) {
        if (prefix > 0) {
            buf = malloc((size_t) prefix + (size_t) len + 1);
            if (!buf) {
                free(tmp);
                return;
            }
            memset(buf, ' ', (size_t) prefix);
            memcpy(buf + prefix, tmp, (size_t) len + 1);
            free(tmp);
        } else {
            buf = tmp;
        }
        ir_lines_push(ctx, buf);
        return;
    }
    for (int i = 0; i < ctx->indent; i++) {
        fputs("  ", out);
    }
    fputs(tmp, out);
    fputc('\n', out);
    free(tmp);
}

int new_temp(IRContext *ctx)
{
    ctx->temp_id++;
    return ctx->temp_id;
}

int new_label(IRContext *ctx)
{
    ctx->label_id++;
    return ctx->label_id;
}

const char *safe_str(const char *s)
{
    return s ? s : "";
}

static int emit_expr(IRContext *ctx, ASTNode *node, FILE *out);
static int emit_const(IRContext *ctx, const char *kind, const char *value, FILE *out);
static void emit_stmt(IRContext *ctx, ASTNode *node, FILE *out, int break_label, int continue_label);
static void emit_matrix_init_list(IRContext *ctx, const char *name, ASTNode *node, int row, int *col, FILE *out);
static void emit_matrix_init_rows(IRContext *ctx, const char *name, ASTNode *node, int *row, FILE *out);
static void emit_matrix_init(IRContext *ctx, const char *name, ASTNode *node, FILE *out);
static int emit_matrix_add3(IRContext *ctx, const char *dst, const char *a, const char *b, const char *c,
                            int rows, int cols, FILE *out);
static int emit_matrix_zero_fill(IRContext *ctx, const char *dst, int rows, int cols, FILE *out);
static int emit_matrix_identity_fill(IRContext *ctx, const char *dst, int n, FILE *out);
static int emit_matrix_scalar_copy(IRContext *ctx, const char *dst, const char *src, FILE *out);
static int emit_matrix_scalar_binop(IRContext *ctx, const char *dst, const char *a, const char *b,
                                    const char *op, FILE *out);
static int emit_matrix_mul_inner1(IRContext *ctx, const char *dst, const char *a, const char *b,
                                  int rows, int cols, FILE *out);
static int emit_matrix_mul_diag_left(IRContext *ctx, const char *dst, const char *diag, const char *a,
                                     int rows, int cols, FILE *out);
static int emit_matrix_mul_diag_right(IRContext *ctx, const char *dst, const char *a, const char *diag,
                                      int rows, int cols, FILE *out);
static int emit_matrix_mul_sym_self(IRContext *ctx, const char *dst, const char *a,
                                    int n, FILE *out);
static int emit_matrix_transpose_inplace(IRContext *ctx, const char *name, int n, FILE *out);
static int emit_matrix_transpose_via_temp(IRContext *ctx, const char *dst, const char *src,
                                          int src_rows, int src_cols, int dst_rows, int dst_cols, FILE *out);
static int emit_matrix_mul_unrolled(IRContext *ctx, const char *dst, const char *a, const char *b,
                                    int n, FILE *out);
static int emit_matrix_det_diag(IRContext *ctx, const char *src, int n, FILE *out);

static int ir_opt_enabled(const IRContext *ctx)
{
    return ctx && ctx->opt_level >= IR_OPT_O1;
}

static int ir_o2_enabled(const IRContext *ctx)
{
    return ctx && ctx->opt_level >= IR_OPT_O2;
}

static int parse_int_value(const char *text, int *out)
{
    char *end = NULL;
    long value;

    if (!text || !out) {
        return 0;
    }

    value = strtol(text, &end, 10);
    if (end == text || *end != '\0') {
        return 0;
    }

    *out = (int) value;
    return 1;
}

static int parse_double_value(const char *text, double *out)
{
    char *end = NULL;
    double value;

    if (!text || !out) {
        return 0;
    }

    value = strtod(text, &end);
    if (end == text || *end != '\0') {
        return 0;
    }

    *out = value;
    return 1;
}

static int parse_char_value(const char *text, int *out)
{
    size_t len;

    if (!text || !out) {
        return 0;
    }

    len = strlen(text);
    if (len < 3 || text[0] != '\'' || text[len - 1] != '\'') {
        return 0;
    }

    if (text[1] != '\\') {
        *out = (unsigned char) text[1];
        return 1;
    }

    if (len < 4) {
        return 0;
    }

    switch (text[2]) {
        case 'n':
            *out = '\n';
            return 1;
        case 't':
            *out = '\t';
            return 1;
        case 'r':
            *out = '\r';
            return 1;
        case '0':
            *out = '\0';
            return 1;
        case '\\':
            *out = '\\';
            return 1;
        case '\'':
            *out = '\'';
            return 1;
        case 'b':
            *out = '\b';
            return 1;
        case 'f':
            *out = '\f';
            return 1;
        case 'v':
            *out = '\v';
            return 1;
        default:
            return 0;
    }
}

static void const_free_list(ConstBinding *head)
{
    while (head) {
        ConstBinding *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

static void const_clear(IRContext *ctx)
{
    if (!ctx) {
        return;
    }
    const_free_list(ctx->consts);
    ctx->consts = NULL;
}

static ConstBinding *const_find_binding(IRContext *ctx, const char *name)
{
    ConstBinding *cur;

    if (!ctx || !name) {
        return NULL;
    }

    cur = ctx->consts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

static void const_unbind(IRContext *ctx, const char *name)
{
    ConstBinding *cur;
    ConstBinding *prev;

    if (!ctx || !name) {
        return;
    }

    prev = NULL;
    cur = ctx->consts;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) {
                prev->next = cur->next;
            } else {
                ctx->consts = cur->next;
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void const_bind(IRContext *ctx, const char *name, const ConstVal *val)
{
    ConstBinding *cur;

    if (!ctx || !name || !val) {
        return;
    }

    cur = const_find_binding(ctx, name);
    if (!cur) {
        cur = malloc(sizeof(ConstBinding));
        cur->name = strdup(name);
        cur->next = ctx->consts;
        ctx->consts = cur;
    }
    cur->val = *val;
}

static void expr_cache_free(ExprCache *head)
{
    while (head) {
        ExprCache *next = head->next;
        free(head->key);
        free(head);
        head = next;
    }
}

static void mat_cache_free(MatExprCache *head)
{
    while (head) {
        MatExprCache *next = head->next;
        free(head->key);
        free(head->name);
        free(head);
        head = next;
    }
}

static MatExprCache *mat_cache_find(MatExprCache *head, const char *key)
{
    while (head) {
        if (strcmp(head->key, key) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

static void mat_cache_add(MatExprCache **head, char *key, const char *name)
{
    MatExprCache *node;

    if (!head || !key || !name) {
        free(key);
        return;
    }
    node = malloc(sizeof(MatExprCache));
    node->key = key;
    node->name = strdup(name);
    node->next = *head;
    *head = node;
}

static ExprCache *expr_cache_find(ExprCache *head, const char *key)
{
    while (head) {
        if (strcmp(head->key, key) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

static void expr_cache_add(ExprCache **head, char *key, int temp)
{
    ExprCache *node;

    if (!head || !key) {
        return;
    }
    node = malloc(sizeof(ExprCache));
    node->key = key;
    node->temp = temp;
    node->next = *head;
    *head = node;
}

static void cse_clear(IRContext *ctx)
{
    if (!ctx) {
        return;
    }
    expr_cache_free(ctx->cse_cache);
    ctx->cse_cache = NULL;
    mat_cache_free(ctx->mat_cse_cache);
    ctx->mat_cse_cache = NULL;
}

static int cse_key_mentions(const char *key, const char *name)
{
    char buf[128];

    if (!key || !name || name[0] == '\0') {
        return 0;
    }
    snprintf(buf, sizeof(buf), "id:%s", name);
    if (strstr(key, buf)) {
        return 1;
    }
    snprintf(buf, sizeof(buf), "arr:%s[", name);
    if (strstr(key, buf)) {
        return 1;
    }
    snprintf(buf, sizeof(buf), "mat:%s[", name);
    if (strstr(key, buf)) {
        return 1;
    }
    return 0;
}

static void cse_invalidate_name(IRContext *ctx, const char *name)
{
    ExprCache *cur;
    ExprCache *prev;

    if (!ctx || !name) {
        return;
    }
    prev = NULL;
    cur = ctx->cse_cache;
    while (cur) {
        ExprCache *next = cur->next;
        if (cse_key_mentions(cur->key, name)) {
            if (prev) {
                prev->next = next;
            } else {
                ctx->cse_cache = next;
            }
            free(cur->key);
            free(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
}

static ConstBinding *const_clone_list(const ConstBinding *head)
{
    ConstBinding *result = NULL;
    ConstBinding *tail = NULL;

    while (head) {
        ConstBinding *node = malloc(sizeof(ConstBinding));
        node->name = strdup(head->name);
        node->val = head->val;
        node->next = NULL;
        if (!result) {
            result = node;
        } else {
            tail->next = node;
        }
        tail = node;
        head = head->next;
    }
    return result;
}

static int const_val_equal(const ConstVal *a, const ConstVal *b)
{
    if (!a || !b) {
        return 0;
    }
    if (a->kind != b->kind) {
        return 0;
    }
    switch (a->kind) {
        case CONST_INT:
            return a->i == b->i;
        case CONST_DECIMAL:
            return a->d == b->d;
        case CONST_CHAR:
        case CONST_STRING:
            return a->s && b->s && strcmp(a->s, b->s) == 0;
        default:
            return 0;
    }
}

static int expr_is_pure(ASTNode *node)
{
    if (!node || !node->type) {
        return 0;
    }
    if (strcmp(node->type, "int") == 0 ||
        strcmp(node->type, "decimal") == 0 ||
        strcmp(node->type, "char") == 0 ||
        strcmp(node->type, "string") == 0 ||
        strcmp(node->type, "id") == 0) {
        return 1;
    }
    if (strcmp(node->type, "array_access") == 0) {
        return expr_is_pure(node->left);
    }
    if (strcmp(node->type, "matrix_access") == 0) {
        return expr_is_pure(node->left) && expr_is_pure(node->right);
    }
    if (strcmp(node->type, "not") == 0 || strcmp(node->type, "uminus") == 0) {
        return expr_is_pure(node->left);
    }
    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 ||
        strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0 ||
        strcmp(node->type, "%") == 0 ||
        strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0 ||
        strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0 ||
        strcmp(node->type, "matadd") == 0 || strcmp(node->type, "matsub") == 0 ||
        strcmp(node->type, "matmul") == 0) {
        return expr_is_pure(node->left) && expr_is_pure(node->right);
    }
    if (strcmp(node->type, "size") == 0 ||
        strcmp(node->type, "sort") == 0 ||
        strcmp(node->type, "transpose") == 0 ||
        strcmp(node->type, "det") == 0 ||
        strcmp(node->type, "inv") == 0 ||
        strcmp(node->type, "shape") == 0) {
        return expr_is_pure(node->left);
    }
    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        return 0;
    }
    if (strcmp(node->type, "assign") == 0) {
        return 0;
    }
    return 0;
}

static char *str_printf(const char *fmt, ...)
{
    va_list args;
    va_list copy;
    int len;
    char *buf;

    va_start(args, fmt);
    va_copy(copy, args);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (len < 0) {
        va_end(copy);
        return NULL;
    }
    buf = malloc((size_t) len + 1);
    if (!buf) {
        va_end(copy);
        return NULL;
    }
    vsnprintf(buf, (size_t) len + 1, fmt, copy);
    va_end(copy);
    return buf;
}

static char *expr_key(ASTNode *node)
{
    char *left = NULL;
    char *right = NULL;
    char *key = NULL;

    if (!expr_is_pure(node)) {
        return NULL;
    }
    if (strcmp(node->type, "int") == 0 ||
        strcmp(node->type, "decimal") == 0 ||
        strcmp(node->type, "char") == 0 ||
        strcmp(node->type, "string") == 0 ||
        strcmp(node->type, "id") == 0) {
        return str_printf("%s:%s", node->type, safe_str(node->value));
    }
    if (strcmp(node->type, "array_access") == 0) {
        left = expr_key(node->left);
        if (!left) {
            return NULL;
        }
        key = str_printf("arr:%s[%s]", safe_str(node->value), left);
        free(left);
        return key;
    }
    if (strcmp(node->type, "matrix_access") == 0) {
        left = expr_key(node->left);
        right = expr_key(node->right);
        if (!left || !right) {
            free(left);
            free(right);
            return NULL;
        }
        key = str_printf("mat:%s[%s,%s]", safe_str(node->value), left, right);
        free(left);
        free(right);
        return key;
    }
    if (strcmp(node->type, "not") == 0 || strcmp(node->type, "uminus") == 0) {
        left = expr_key(node->left);
        if (!left) {
            return NULL;
        }
        key = str_printf("%s(%s)", node->type, left);
        free(left);
        return key;
    }
    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 ||
        strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0 ||
        strcmp(node->type, "%") == 0 ||
        strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0 ||
        strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0 ||
        strcmp(node->type, "matadd") == 0 || strcmp(node->type, "matsub") == 0 ||
        strcmp(node->type, "matmul") == 0) {
        left = expr_key(node->left);
        right = expr_key(node->right);
        if (!left || !right) {
            free(left);
            free(right);
            return NULL;
        }
        key = str_printf("%s(%s,%s)", node->type, left, right);
        free(left);
        free(right);
        return key;
    }
    if (strcmp(node->type, "size") == 0 ||
        strcmp(node->type, "sort") == 0 ||
        strcmp(node->type, "transpose") == 0 ||
        strcmp(node->type, "det") == 0 ||
        strcmp(node->type, "inv") == 0 ||
        strcmp(node->type, "shape") == 0) {
        left = expr_key(node->left);
        if (!left) {
            return NULL;
        }
        key = str_printf("%s(%s)", node->type, left);
        free(left);
        return key;
    }
    return NULL;
}

static ConstBinding *const_intersect(ConstBinding *a, ConstBinding *b)
{
    ConstBinding *result = NULL;
    ConstBinding *tail = NULL;

    while (a) {
        ConstBinding *cur = b;
        while (cur) {
            if (strcmp(a->name, cur->name) == 0 && const_val_equal(&a->val, &cur->val)) {
                ConstBinding *node = malloc(sizeof(ConstBinding));
                node->name = strdup(a->name);
                node->val = a->val;
                node->next = NULL;
                if (!result) {
                    result = node;
                } else {
                    tail->next = node;
                }
                tail = node;
                break;
            }
            cur = cur->next;
        }
        a = a->next;
    }
    return result;
}

static void const_set(IRContext *ctx, ConstBinding *list)
{
    if (!ctx) {
        return;
    }
    const_free_list(ctx->consts);
    ctx->consts = list;
}

static void add_matrix_info(IRContext *ctx, const char *name, int rows, int cols)
{
    MatrixInfo *info;

    if (!ctx || !name) {
        return;
    }

    info = malloc(sizeof(MatrixInfo));
    info->name = strdup(name);
    info->rows = rows;
    info->cols = cols;
    info->props_known = 0;
    info->is_zero = 0;
    info->is_identity = 0;
    info->is_diagonal = 0;
    info->is_symmetric = 0;
    info->is_upper_triangular = 0;
    info->is_lower_triangular = 0;
    info->next = ctx->matrices;
    ctx->matrices = info;
}

static char *mat_expr_key(const char *op, const char *a, const char *b)
{
    if (!op || !a || !b) {
        return NULL;
    }
    return str_printf("mop:%s(id:%s,id:%s)", op, a, b);
}

static char *mat_unary_key(const char *op, const char *a)
{
    if (!op || !a) {
        return NULL;
    }
    return str_printf("mop:%s(id:%s)", op, a);
}

static void mat_cache_invalidate_name(IRContext *ctx, const char *name)
{
    MatExprCache *cur;
    MatExprCache *prev;

    if (!ctx || !name) {
        return;
    }
    prev = NULL;
    cur = ctx->mat_cse_cache;
    while (cur) {
        MatExprCache *next = cur->next;
        if (cse_key_mentions(cur->key, name)) {
            if (prev) {
                prev->next = next;
            } else {
                ctx->mat_cse_cache = next;
            }
            free(cur->key);
            free(cur->name);
            free(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
}

static void matrix_invalidate_name(IRContext *ctx, const char *name)
{
    mat_cache_invalidate_name(ctx, name);
    cse_invalidate_name(ctx, name);
}

static void add_array_info(IRContext *ctx, const char *name, int size)
{
    ArrayInfo *info;

    if (!ctx || !name) {
        return;
    }

    info = malloc(sizeof(ArrayInfo));
    info->name = strdup(name);
    info->size = size;
    info->next = ctx->arrays;
    ctx->arrays = info;
}

static MatrixInfo *find_matrix_info(IRContext *ctx, const char *name)
{
    MatrixInfo *info;

    if (!ctx || !name) {
        return NULL;
    }

    info = ctx->matrices;
    while (info) {
        if (strcmp(info->name, name) == 0) {
            return info;
        }
        info = info->next;
    }
    return NULL;
}

static void matrix_props_set(MatrixInfo *info, int known, int is_zero, int is_identity, int is_diagonal,
                             int is_symmetric, int is_upper_triangular, int is_lower_triangular)
{
    if (!info) {
        return;
    }
    info->props_known = known;
    info->is_zero = is_zero;
    info->is_identity = is_identity;
    info->is_diagonal = is_diagonal;
    info->is_symmetric = is_symmetric;
    info->is_upper_triangular = is_upper_triangular;
    info->is_lower_triangular = is_lower_triangular;
}

static void matrix_props_unknown(MatrixInfo *info)
{
    matrix_props_set(info, 0, 0, 0, 0, 0, 0, 0);
}

static void matrix_props_copy(MatrixInfo *dst, const MatrixInfo *src)
{
    if (!dst) {
        return;
    }
    if (src && src->props_known) {
        matrix_props_set(dst, 1, src->is_zero, src->is_identity, src->is_diagonal,
                         src->is_symmetric, src->is_upper_triangular, src->is_lower_triangular);
        return;
    }
    matrix_props_unknown(dst);
}

static int matrix_is_zero(const MatrixInfo *info)
{
    return info && info->props_known && info->is_zero;
}

static int matrix_is_identity(const MatrixInfo *info)
{
    return info && info->props_known && info->is_identity;
}

static int matrix_is_diagonal(const MatrixInfo *info)
{
    return info && info->props_known && info->is_diagonal;
}

static int matrix_is_symmetric(const MatrixInfo *info)
{
    return info && info->props_known && info->is_symmetric;
}

static int matrix_is_upper_triangular(const MatrixInfo *info)
{
    return info && info->props_known && info->is_upper_triangular;
}

static int matrix_is_lower_triangular(const MatrixInfo *info)
{
    return info && info->props_known && info->is_lower_triangular;
}

static ArrayInfo *find_array_info(IRContext *ctx, const char *name)
{
    ArrayInfo *info;

    if (!ctx || !name) {
        return NULL;
    }

    info = ctx->arrays;
    while (info) {
        if (strcmp(info->name, name) == 0) {
            return info;
        }
        info = info->next;
    }
    return NULL;
}

static int const_is_numeric(const ConstVal *val)
{
    if (!val) {
        return 0;
    }
    return val->kind == CONST_INT || val->kind == CONST_DECIMAL || val->kind == CONST_CHAR;
}

static int const_to_int(const ConstVal *val, long long *out)
{
    int ch = 0;

    if (!val || !out) {
        return 0;
    }
    if (val->kind == CONST_INT) {
        *out = val->i;
        return 1;
    }
    if (val->kind == CONST_CHAR) {
        if (!parse_char_value(val->s, &ch)) {
            return 0;
        }
        *out = ch;
        return 1;
    }
    return 0;
}

static int const_to_double(const ConstVal *val, double *out)
{
    int ch = 0;

    if (!val || !out) {
        return 0;
    }
    if (val->kind == CONST_DECIMAL) {
        *out = val->d;
        return 1;
    }
    if (val->kind == CONST_INT) {
        *out = (double) val->i;
        return 1;
    }
    if (val->kind == CONST_CHAR) {
        if (!parse_char_value(val->s, &ch)) {
            return 0;
        }
        *out = (double) ch;
        return 1;
    }
    return 0;
}

static int const_to_bool(const ConstVal *val, int *out)
{
    long long i = 0;
    double d = 0.0;

    if (!val || !out) {
        return 0;
    }
    if (!const_is_numeric(val)) {
        return 0;
    }
    if (val->kind == CONST_DECIMAL) {
        if (!const_to_double(val, &d)) {
            return 0;
        }
        *out = (d != 0.0);
        return 1;
    }
    if (!const_to_int(val, &i)) {
        return 0;
    }
    *out = (i != 0);
    return 1;
}

static int const_is_zero(const ConstVal *val)
{
    long long i = 0;
    double d = 0.0;

    if (!val) {
        return 0;
    }
    if (val->kind == CONST_DECIMAL) {
        return const_to_double(val, &d) && d == 0.0;
    }
    return const_to_int(val, &i) && i == 0;
}

static int const_is_one(const ConstVal *val)
{
    long long i = 0;
    double d = 0.0;

    if (!val) {
        return 0;
    }
    if (val->kind == CONST_DECIMAL) {
        return const_to_double(val, &d) && d == 1.0;
    }
    return const_to_int(val, &i) && i == 1;
}

static int const_is_neg_one(const ConstVal *val)
{
    long long i = 0;
    double d = 0.0;

    if (!val) {
        return 0;
    }
    if (val->kind == CONST_DECIMAL) {
        return const_to_double(val, &d) && d == -1.0;
    }
    return const_to_int(val, &i) && i == -1;
}

static int const_compare_equal(const ConstVal *a, const ConstVal *b)
{
    long long ai = 0;
    long long bi = 0;
    double ad = 0.0;
    double bd = 0.0;

    if (!a || !b) {
        return 0;
    }
    if (!const_is_numeric(a) || !const_is_numeric(b)) {
        return 0;
    }
    if (a->kind == CONST_DECIMAL || b->kind == CONST_DECIMAL) {
        if (!const_to_double(a, &ad) || !const_to_double(b, &bd)) {
            return 0;
        }
        return ad == bd;
    }
    if (!const_to_int(a, &ai) || !const_to_int(b, &bi)) {
        return 0;
    }
    return ai == bi;
}

static int emit_const_value(IRContext *ctx, const ConstVal *val, FILE *out)
{
    char buf[64];

    if (!ctx || !val) {
        return -1;
    }

    switch (val->kind) {
        case CONST_INT:
            snprintf(buf, sizeof(buf), "%lld", val->i);
            return emit_const(ctx, "int", buf, out);
        case CONST_DECIMAL:
            snprintf(buf, sizeof(buf), "%.15g", val->d);
            return emit_const(ctx, "decimal", buf, out);
        case CONST_CHAR:
            return emit_const(ctx, "char", val->s, out);
        case CONST_STRING:
            return emit_const(ctx, "string", val->s, out);
        default:
            return -1;
    }
}

static int eval_const_expr(IRContext *ctx, ASTNode *node, ConstVal *out)
{
    ConstBinding *binding;
    ConstVal lhs;
    ConstVal rhs;
    long long li = 0;
    long long ri = 0;
    double ld = 0.0;
    double rd = 0.0;
    int tmp = 0;

    if (!node || !out || !node->type) {
        return 0;
    }

    if (strcmp(node->type, "int") == 0) {
        if (!parse_int_value(node->value, &tmp)) {
            return 0;
        }
        out->kind = CONST_INT;
        out->i = tmp;
        out->d = 0.0;
        out->s = NULL;
        return 1;
    }
    if (strcmp(node->type, "decimal") == 0) {
        if (!parse_double_value(node->value, &ld)) {
            return 0;
        }
        out->kind = CONST_DECIMAL;
        out->d = ld;
        out->i = 0;
        out->s = NULL;
        return 1;
    }
    if (strcmp(node->type, "char") == 0) {
        if (!parse_char_value(node->value, &tmp)) {
            return 0;
        }
        out->kind = CONST_CHAR;
        out->i = tmp;
        out->d = 0.0;
        out->s = node->value;
        return 1;
    }
    if (strcmp(node->type, "string") == 0) {
        out->kind = CONST_STRING;
        out->s = node->value;
        out->i = 0;
        out->d = 0.0;
        return 1;
    }
    if (strcmp(node->type, "id") == 0) {
        binding = const_find_binding(ctx, node->value);
        if (!binding) {
            return 0;
        }
        *out = binding->val;
        return 1;
    }
    if (strcmp(node->type, "size") == 0 &&
        node->left && strcmp(node->left->type, "id") == 0) {
        ArrayInfo *info = find_array_info(ctx, node->left->value);
        if (info && info->size > 0) {
            out->kind = CONST_INT;
            out->i = info->size;
            out->d = 0.0;
            out->s = NULL;
            return 1;
        }
    }

    if (strcmp(node->type, "not") == 0 || strcmp(node->type, "uminus") == 0) {
        if (!eval_const_expr(ctx, node->left, &lhs)) {
            return 0;
        }
        if (!const_is_numeric(&lhs)) {
            return 0;
        }
        if (strcmp(node->type, "not") == 0) {
            int b = 0;
            if (!const_to_bool(&lhs, &b)) {
                return 0;
            }
            out->kind = CONST_INT;
            out->i = b ? 0 : 1;
            out->d = 0.0;
            out->s = NULL;
            return 1;
        }
        if (lhs.kind == CONST_DECIMAL) {
            if (!const_to_double(&lhs, &ld)) {
                return 0;
            }
            out->kind = CONST_DECIMAL;
            out->d = -ld;
            out->i = 0;
            out->s = NULL;
            return 1;
        }
        if (!const_to_int(&lhs, &li)) {
            return 0;
        }
        out->kind = CONST_INT;
        out->i = -li;
        out->d = 0.0;
        out->s = NULL;
        return 1;
    }

    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 ||
        strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0 ||
        strcmp(node->type, "%") == 0 ||
        strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0 ||
        strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0) {
        if (!eval_const_expr(ctx, node->left, &lhs) ||
            !eval_const_expr(ctx, node->right, &rhs)) {
            return 0;
        }
        if (!const_is_numeric(&lhs) || !const_is_numeric(&rhs)) {
            return 0;
        }

        if (strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0) {
            int lb = 0;
            int rb = 0;
            if (!const_to_bool(&lhs, &lb) || !const_to_bool(&rhs, &rb)) {
                return 0;
            }
            out->kind = CONST_INT;
            out->i = (strcmp(node->type, "and") == 0) ? (lb && rb) : (lb || rb);
            out->d = 0.0;
            out->s = NULL;
            return 1;
        }

        if (strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
            strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
            strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0) {
            int result = 0;
            if (lhs.kind == CONST_DECIMAL || rhs.kind == CONST_DECIMAL) {
                if (!const_to_double(&lhs, &ld) || !const_to_double(&rhs, &rd)) {
                    return 0;
                }
                if (strcmp(node->type, "eq") == 0) result = (ld == rd);
                else if (strcmp(node->type, "ne") == 0) result = (ld != rd);
                else if (strcmp(node->type, "lt") == 0) result = (ld < rd);
                else if (strcmp(node->type, "gt") == 0) result = (ld > rd);
                else if (strcmp(node->type, "le") == 0) result = (ld <= rd);
                else if (strcmp(node->type, "ge") == 0) result = (ld >= rd);
            } else {
                if (!const_to_int(&lhs, &li) || !const_to_int(&rhs, &ri)) {
                    return 0;
                }
                if (strcmp(node->type, "eq") == 0) result = (li == ri);
                else if (strcmp(node->type, "ne") == 0) result = (li != ri);
                else if (strcmp(node->type, "lt") == 0) result = (li < ri);
                else if (strcmp(node->type, "gt") == 0) result = (li > ri);
                else if (strcmp(node->type, "le") == 0) result = (li <= ri);
                else if (strcmp(node->type, "ge") == 0) result = (li >= ri);
            }
            out->kind = CONST_INT;
            out->i = result;
            out->d = 0.0;
            out->s = NULL;
            return 1;
        }

        if (strcmp(node->type, "%") == 0) {
            if (!const_to_int(&lhs, &li) || !const_to_int(&rhs, &ri)) {
                return 0;
            }
            if (ri == 0) {
                return 0;
            }
            out->kind = CONST_INT;
            out->i = li % ri;
            out->d = 0.0;
            out->s = NULL;
            return 1;
        }

        if (lhs.kind == CONST_DECIMAL || rhs.kind == CONST_DECIMAL) {
            if (!const_to_double(&lhs, &ld) || !const_to_double(&rhs, &rd)) {
                return 0;
            }
            if (strcmp(node->type, "/") == 0 && rd == 0.0) {
                return 0;
            }
            out->kind = CONST_DECIMAL;
            if (strcmp(node->type, "+") == 0) out->d = ld + rd;
            else if (strcmp(node->type, "-") == 0) out->d = ld - rd;
            else if (strcmp(node->type, "*") == 0) out->d = ld * rd;
            else if (strcmp(node->type, "/") == 0) out->d = ld / rd;
            else return 0;
            out->i = 0;
            out->s = NULL;
            return 1;
        }

        if (!const_to_int(&lhs, &li) || !const_to_int(&rhs, &ri)) {
            return 0;
        }
        if (strcmp(node->type, "/") == 0 && ri == 0) {
            return 0;
        }
        out->kind = CONST_INT;
        if (strcmp(node->type, "+") == 0) out->i = li + ri;
        else if (strcmp(node->type, "-") == 0) out->i = li - ri;
        else if (strcmp(node->type, "*") == 0) out->i = li * ri;
        else if (strcmp(node->type, "/") == 0) out->i = li / ri;
        else return 0;
        out->d = 0.0;
        out->s = NULL;
        return 1;
    }

    return 0;
}

static void collect_matrix_decls(IRContext *ctx, ASTNode *node)
{
    int rows = -1;
    int cols = -1;

    if (!ctx || !node || !node->type) {
        return;
    }

    if (strcmp(node->type, "matrix_decl") == 0) {
        if (!parse_int_value(node->left ? node->left->value : NULL, &rows)) {
            rows = -1;
        }
        if (!parse_int_value(node->right ? node->right->value : NULL, &cols)) {
            cols = -1;
        }
        if (node->value) {
            add_matrix_info(ctx, node->value, rows, cols);
        }
    }

    if (strcmp(node->type, "array_decl") == 0) {
        if (!parse_int_value(node->left ? node->left->value : NULL, &rows)) {
            rows = -1;
        }
        if (node->value) {
            add_array_info(ctx, node->value, rows);
        }
    }

    collect_matrix_decls(ctx, node->left);
    collect_matrix_decls(ctx, node->right);
    collect_matrix_decls(ctx, node->third);
}

static void free_matrix_info(IRContext *ctx)
{
    MatrixInfo *cur;
    MatrixInfo *next;

    if (!ctx) {
        return;
    }

    cur = ctx->matrices;
    while (cur) {
        next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
    ctx->matrices = NULL;
}

static void free_array_info(IRContext *ctx)
{
    ArrayInfo *cur;
    ArrayInfo *next;

    if (!ctx) {
        return;
    }

    cur = ctx->arrays;
    while (cur) {
        next = cur->next;
        free(cur->name);
        free(cur);
        cur = next;
    }
    ctx->arrays = NULL;
}

static int emit_matrix_elem_addr(IRContext *ctx, const char *name, int row_temp, int col_temp, FILE *out)
{
    int base = new_temp(ctx);
    int row_off = new_temp(ctx);
    int col_off = new_temp(ctx);
    int off = new_temp(ctx);
    int addr = new_temp(ctx);

    ir_printf(ctx, out, "t%d = addr %s", base, safe_str(name));
    ir_printf(ctx, out, "t%d = mul t%d, cols(%s)", row_off, row_temp, safe_str(name));
    ir_printf(ctx, out, "t%d = add t%d, t%d", col_off, row_off, col_temp);
    ir_printf(ctx, out, "t%d = mul t%d, 4", off, col_off);
    ir_printf(ctx, out, "t%d = add t%d, t%d", addr, base, off);
    return addr;
}

static int emit_array_elem_addr(IRContext *ctx, const char *name, int idx_temp, FILE *out)
{
    int base = new_temp(ctx);
    int off = new_temp(ctx);
    int addr = new_temp(ctx);

    ir_printf(ctx, out, "t%d = addr %s", base, safe_str(name));
    ir_printf(ctx, out, "t%d = mul t%d, 4", off, idx_temp);
    ir_printf(ctx, out, "t%d = add t%d, t%d", addr, base, off);
    return addr;
}

static int emit_matrix_add_sub(IRContext *ctx, const char *dst, const char *a, const char *b,
                               int rows, int cols, const char *op, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int base_a;
    int base_b;
    int base_dst;
    int cols_bytes;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !a || !b || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_a = new_temp(ctx);
    base_b = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_b, safe_str(b));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int row_base_b = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_val = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_a;
        int addr_b;
        int addr_dst;
        int val_a;
        int val_b;
        int val_out;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_b, base_b, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_a = new_temp(ctx);
        addr_b = new_temp(ctx);
        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_b, row_base_b, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);

        val_a = new_temp(ctx);
        val_b = new_temp(ctx);
        val_out = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", val_out, op, val_a, val_b);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_out);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_copy(IRContext *ctx, const char *dst, const char *src, int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int base_src;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !src || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_src = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_src, safe_str(src));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_src = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_val = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_src;
        int addr_dst;
        int val_src;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_src, base_src, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_src = new_temp(ctx);
        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_src, row_base_src, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);

        val_src = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_src, addr_src);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_src);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_mul(IRContext *ctx, const char *dst, const char *a, const char *b,
                           int rows, int inner, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    int k_var_id;
    int acc_var_id;
    char row_var[32];
    char col_var[32];
    char k_var[32];
    char acc_var[32];
    int row_addr;
    int col_addr;
    int k_addr;
    int acc_addr;
    int zero;
    int one;
    int rows_const;
    int inner_const;
    int cols_const;
    int four;
    int inner_bytes;
    int cols_bytes;
    int base_a;
    int base_b;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;
    int k_label;
    int k_end_label;

    if (!dst || !a || !b || rows <= 0 || inner <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    k_var_id = new_temp(ctx);
    acc_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);
    snprintf(k_var, sizeof(k_var), "mat_k%d", k_var_id);
    snprintf(acc_var, sizeof(acc_var), "mat_acc%d", acc_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    ir_printf(ctx, out, "decl int %s", k_var);
    ir_printf(ctx, out, "decl int %s", acc_var);

    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    k_addr = new_temp(ctx);
    acc_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);
    ir_printf(ctx, out, "t%d = addr %s", k_addr, k_var);
    ir_printf(ctx, out, "t%d = addr %s", acc_addr, acc_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    inner_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", inner_const, inner);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    inner_bytes = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", inner_bytes, inner_const, four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_a = new_temp(ctx);
    base_b = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_b, safe_str(b));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", acc_addr, zero);
    ir_printf(ctx, out, "store [t%d], t%d", k_addr, zero);
    k_label = new_label(ctx);
    k_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", k_label);
    {
        int k_val = new_temp(ctx);
        int k_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", k_cmp, k_val, inner_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", k_cmp, k_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int k_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int col_off = new_temp(ctx);
        int k_off;
        int addr_a;
        int addr_b;
        int b_row_off;
        int b_off;
        int val_a;
        int val_b;
        int mul_val;
        int acc_val;
        int acc_next;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, inner_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);

        k_off = new_temp(ctx);
        addr_a = new_temp(ctx);
        b_row_off = new_temp(ctx);
        b_off = new_temp(ctx);
        addr_b = new_temp(ctx);

        ir_printf(ctx, out, "t%d = * t%d, t%d", k_off, k_val, four);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, k_off);

        ir_printf(ctx, out, "t%d = * t%d, t%d", b_row_off, k_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", b_off, b_row_off, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_b, base_b, b_off);

        val_a = new_temp(ctx);
        val_b = new_temp(ctx);
        mul_val = new_temp(ctx);
        acc_val = new_temp(ctx);
        acc_next = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
        ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_a, val_b);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", acc_next, acc_val, mul_val);
        ir_printf(ctx, out, "store [t%d], t%d", acc_addr, acc_next);
    }

    {
        int k_val = new_temp(ctx);
        int k_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", k_next, k_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_next);
    }
    ir_printf(ctx, out, "goto L%d", k_label);
    ir_printf(ctx, out, "label L%d", k_end_label);

    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int acc_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_dst;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);

        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, acc_val);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_transpose(IRContext *ctx, const char *dst, const char *src, int dst_rows, int dst_cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int src_cols_bytes;
    int dst_cols_bytes;
    int base_src;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !src || dst_rows <= 0 || dst_cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, dst_rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, dst_cols);
    four = new_temp(ctx);
    src_cols_bytes = new_temp(ctx);
    dst_cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    /* src cols == dst_rows, dst cols == dst_cols */
    {
        int src_cols_const = new_temp(ctx);
        ir_printf(ctx, out, "t%d = const int %d", src_cols_const, dst_rows);
        ir_printf(ctx, out, "t%d = * t%d, t%d", src_cols_bytes, src_cols_const, four);
    }
    ir_printf(ctx, out, "t%d = * t%d, t%d", dst_cols_bytes, cols_const, four);

    base_src = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_src, safe_str(src));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int row_off_dst = new_temp(ctx);
        int col_off_src = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_base_src = new_temp(ctx);
        int addr_src;
        int addr_dst;
        int val_src;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_dst, row_val, dst_cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off_dst);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off_src, col_val, src_cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_base_src, base_src, col_off_src);

        addr_src = new_temp(ctx);
        addr_dst = new_temp(ctx);
        {
            int row_off_src = new_temp(ctx);
            int col_off_dst = new_temp(ctx);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_src, row_val, four);
            ir_printf(ctx, out, "t%d = * t%d, t%d", col_off_dst, col_val, four);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_src, col_base_src, row_off_src);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off_dst);
        }

        val_src = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_src, addr_src);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_src);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_transpose_via_temp(IRContext *ctx, const char *dst, const char *src,
                                          int src_rows, int src_cols, int dst_rows, int dst_cols, FILE *out)
{
    int tmp_id;
    char tmp_name[32];

    if (!ctx || !dst || !src || src_rows <= 0 || src_cols <= 0 || dst_rows <= 0 || dst_cols <= 0) {
        return 0;
    }

    tmp_id = new_temp(ctx);
    snprintf(tmp_name, sizeof(tmp_name), "mat_tr_tmp%d", tmp_id);
    ir_printf(ctx, out, "decl_matrix %s, %d, %d", tmp_name, src_rows, src_cols);
    add_matrix_info(ctx, tmp_name, src_rows, src_cols);

    if (!emit_matrix_copy(ctx, tmp_name, src, src_rows, src_cols, out)) {
        return 0;
    }
    return emit_matrix_transpose(ctx, dst, tmp_name, dst_rows, dst_cols, out);
}

static int emit_matrix_det_2x2(IRContext *ctx, const char *src, FILE *out)
{
    int addr_a;
    int addr_b;
    int addr_c;
    int addr_d;
    int val_a;
    int val_b;
    int val_c;
    int val_d;
    int mul_ad;
    int mul_bc;
    int det;
    int row0;
    int row1;
    int col0;
    int col1;

    if (!src) {
        return -1;
    }

    row0 = emit_const(ctx, "int", "0", out);
    row1 = emit_const(ctx, "int", "1", out);
    col0 = emit_const(ctx, "int", "0", out);
    col1 = emit_const(ctx, "int", "1", out);

    addr_a = emit_matrix_elem_addr(ctx, src, row0, col0, out);
    addr_b = emit_matrix_elem_addr(ctx, src, row0, col1, out);
    addr_c = emit_matrix_elem_addr(ctx, src, row1, col0, out);
    addr_d = emit_matrix_elem_addr(ctx, src, row1, col1, out);

    val_a = new_temp(ctx);
    val_b = new_temp(ctx);
    val_c = new_temp(ctx);
    val_d = new_temp(ctx);
    ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
    ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
    ir_printf(ctx, out, "t%d = load [t%d]", val_c, addr_c);
    ir_printf(ctx, out, "t%d = load [t%d]", val_d, addr_d);

    mul_ad = new_temp(ctx);
    mul_bc = new_temp(ctx);
    det = new_temp(ctx);
    ir_printf(ctx, out, "t%d = * t%d, t%d", mul_ad, val_a, val_d);
    ir_printf(ctx, out, "t%d = * t%d, t%d", mul_bc, val_b, val_c);
    ir_printf(ctx, out, "t%d = - t%d, t%d", det, mul_ad, mul_bc);

    return det;
}

static int emit_matrix_det_diag(IRContext *ctx, const char *src, int n, FILE *out)
{
    int acc;

    if (!src || n <= 0) {
        return -1;
    }
    acc = emit_const(ctx, "int", "1", out);
    for (int i = 0; i < n; i++) {
        char ibuf[16];
        int it;
        int addr;
        int val;
        int next;
        snprintf(ibuf, sizeof(ibuf), "%d", i);
        it = emit_const(ctx, "int", ibuf, out);
        addr = emit_matrix_elem_addr(ctx, src, it, it, out);
        val = new_temp(ctx);
        next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val, addr);
        ir_printf(ctx, out, "t%d = * t%d, t%d", next, acc, val);
        acc = next;
    }
    return acc;
}

static int emit_matrix_det(IRContext *ctx, const char *src, int n, FILE *out)
{
    int col_var_id;
    char col_var[32];
    int col_addr;
    int det_var_id;
    char det_var[32];
    int det_addr;
    int zero;
    int one;
    int two;
    int n_const;
    int col_label;
    int col_end_label;

    if (!src || n <= 0) {
        return -1;
    }
    if (n == 1) {
        int row0 = emit_const(ctx, "int", "0", out);
        int col0 = emit_const(ctx, "int", "0", out);
        int addr = emit_matrix_elem_addr(ctx, src, row0, col0, out);
        int val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val, addr);
        return val;
    }
    if (n == 2) {
        return emit_matrix_det_2x2(ctx, src, out);
    }

    col_var_id = new_temp(ctx);
    det_var_id = new_temp(ctx);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);
    snprintf(det_var, sizeof(det_var), "det_acc%d", det_var_id);

    ir_printf(ctx, out, "decl int %s", col_var);
    ir_printf(ctx, out, "decl decimal %s", det_var);
    col_addr = new_temp(ctx);
    det_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);
    ir_printf(ctx, out, "t%d = addr %s", det_addr, det_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    two = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int 2", two);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    ir_printf(ctx, out, "store [t%d], t%d", det_addr, zero);

    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_var_id = new_temp(ctx);
        int src_col_var_id = new_temp(ctx);
        int minor_col_var_id = new_temp(ctx);
        char row_var[32];
        char src_col_var[32];
        char minor_col_var[32];
        int row_addr;
        int src_col_addr;
        int minor_col_addr;
        int row_label;
        int row_end_label;
        int src_col_label;
        int src_col_end_label;
        char minor_name[32];
        int col_val;

        snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
        snprintf(src_col_var, sizeof(src_col_var), "mat_sc%d", src_col_var_id);
        snprintf(minor_col_var, sizeof(minor_col_var), "mat_mc%d", minor_col_var_id);

        ir_printf(ctx, out, "decl int %s", row_var);
        ir_printf(ctx, out, "decl int %s", src_col_var);
        ir_printf(ctx, out, "decl int %s", minor_col_var);
        row_addr = new_temp(ctx);
        src_col_addr = new_temp(ctx);
        minor_col_addr = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
        ir_printf(ctx, out, "t%d = addr %s", src_col_addr, src_col_var);
        ir_printf(ctx, out, "t%d = addr %s", minor_col_addr, minor_col_var);

        {
            int tmp_id = new_temp(ctx);
            snprintf(minor_name, sizeof(minor_name), "mat_tmp%d", tmp_id);
        }
        ir_printf(ctx, out, "decl_matrix %s, %d, %d", minor_name, n - 1, n - 1);
        add_matrix_info(ctx, minor_name, n - 1, n - 1);

        ir_printf(ctx, out, "store [t%d], t%d", row_addr, one);
        row_label = new_label(ctx);
        row_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", row_label);
        {
            int row_val = new_temp(ctx);
            int row_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
        }

        ir_printf(ctx, out, "store [t%d], t%d", minor_col_addr, zero);
        ir_printf(ctx, out, "store [t%d], t%d", src_col_addr, zero);
        src_col_label = new_label(ctx);
        src_col_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", src_col_label);
        {
            int sc_val = new_temp(ctx);
            int sc_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", sc_val, src_col_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", sc_cmp, sc_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", sc_cmp, src_col_end_label);
        }

        col_val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        {
            int sc_val = new_temp(ctx);
            int eq = new_temp(ctx);
            int skip_label = new_label(ctx);
            ir_printf(ctx, out, "t%d = load %s", sc_val, src_col_var);
            ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, sc_val, col_val);
            ir_printf(ctx, out, "ifz t%d goto L%d", eq, skip_label);
            {
                int sc_next = new_temp(ctx);
                ir_printf(ctx, out, "t%d = + t%d, t%d", sc_next, sc_val, one);
                ir_printf(ctx, out, "store [t%d], t%d", src_col_addr, sc_next);
                ir_printf(ctx, out, "goto L%d", src_col_label);
            }
            ir_printf(ctx, out, "label L%d", skip_label);
        }

        {
            int row_val = new_temp(ctx);
            int sc_val = new_temp(ctx);
            int mc_val = new_temp(ctx);
            int minor_row = new_temp(ctx);
            int addr_src;
            int addr_minor;
            int val_src;

            ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
            ir_printf(ctx, out, "t%d = load %s", sc_val, src_col_var);
            ir_printf(ctx, out, "t%d = load %s", mc_val, minor_col_var);
            ir_printf(ctx, out, "t%d = - t%d, t%d", minor_row, row_val, one);

            addr_src = emit_matrix_elem_addr(ctx, src, row_val, sc_val, out);
            addr_minor = emit_matrix_elem_addr(ctx, minor_name, minor_row, mc_val, out);

            val_src = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_src, addr_src);
            ir_printf(ctx, out, "store [t%d], t%d", addr_minor, val_src);
        }

        {
            int mc_val = new_temp(ctx);
            int mc_next = new_temp(ctx);
            int sc_val = new_temp(ctx);
            int sc_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", mc_val, minor_col_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", mc_next, mc_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", minor_col_addr, mc_next);
            ir_printf(ctx, out, "t%d = load %s", sc_val, src_col_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", sc_next, sc_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", src_col_addr, sc_next);
        }
        ir_printf(ctx, out, "goto L%d", src_col_label);
        ir_printf(ctx, out, "label L%d", src_col_end_label);

        {
            int row_val = new_temp(ctx);
            int row_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
        }
        ir_printf(ctx, out, "goto L%d", row_label);
        ir_printf(ctx, out, "label L%d", row_end_label);

        {
            int minor_det = emit_matrix_det(ctx, minor_name, n - 1, out);
            int row0 = emit_const(ctx, "int", "0", out);
            int addr_a;
            int val_a;
            int term;
            int col_val_inner;
            int mod_val;
            int is_odd;
            int det_cur;
            int det_next;

            col_val_inner = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", col_val_inner, col_var);
            addr_a = emit_matrix_elem_addr(ctx, src, row0, col_val_inner, out);
            val_a = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);

            term = new_temp(ctx);
            ir_printf(ctx, out, "t%d = * t%d, t%d", term, val_a, minor_det);

            mod_val = new_temp(ctx);
            is_odd = new_temp(ctx);
            {
                int sign_scale = new_temp(ctx);
                int sign_factor = new_temp(ctx);
                ir_printf(ctx, out, "t%d = %% t%d, t%d", mod_val, col_val_inner, two);
                ir_printf(ctx, out, "t%d = eq t%d, t%d", is_odd, mod_val, one);
                ir_printf(ctx, out, "t%d = * t%d, t%d", sign_scale, is_odd, two);
                ir_printf(ctx, out, "t%d = - t%d, t%d", sign_factor, one, sign_scale);
                {
                    int signed_term = new_temp(ctx);
                    ir_printf(ctx, out, "t%d = * t%d, t%d", signed_term, term, sign_factor);
                    term = signed_term;
                }
            }

            det_cur = new_temp(ctx);
            det_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", det_cur, det_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", det_next, det_cur, term);
            ir_printf(ctx, out, "store [t%d], t%d", det_addr, det_next);
        }
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int det_val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", det_val, det_var);
        return det_val;
    }
}

static void emit_safe_div(IRContext *ctx, int dst, int num, int den, FILE *out)
{
    int zero = new_temp(ctx);
    int one = new_temp(ctx);
    int num_f = new_temp(ctx);
    int den_f = new_temp(ctx);
    int is_zero = new_temp(ctx);
    int nonzero_label = new_label(ctx);
    int end_label = new_label(ctx);

    ir_printf(ctx, out, "t%d = const decimal 0.0", zero);
    ir_printf(ctx, out, "t%d = const decimal 1.0", one);
    ir_printf(ctx, out, "t%d = * t%d, t%d", num_f, num, one);
    ir_printf(ctx, out, "t%d = * t%d, t%d", den_f, den, one);
    ir_printf(ctx, out, "t%d = eq t%d, t%d", is_zero, den_f, zero);
    ir_printf(ctx, out, "ifz t%d goto L%d", is_zero, nonzero_label);
    ir_printf(ctx, out, "t%d = const decimal 0.0", dst);
    ir_printf(ctx, out, "goto L%d", end_label);
    ir_printf(ctx, out, "label L%d", nonzero_label);
    ir_printf(ctx, out, "t%d = / t%d, t%d", dst, num_f, den_f);
    ir_printf(ctx, out, "label L%d", end_label);
}

static int emit_matrix_inverse(IRContext *ctx, const char *dst, const char *src, int n, FILE *out)
{
    int i_var_id;
    int j_var_id;
    int r_var_id;
    char i_var[32];
    char j_var[32];
    char r_var[32];
    int i_addr;
    int j_addr;
    int r_addr;
    int zero;
    int one;
    int zero_dec;
    int one_dec;
    int n_const;
    int i_label;
    int i_end_label;
    int j_label;
    int j_end_label;
    int r_label;
    int r_end_label;
    char a_name[32];

    if (!dst || !src || n <= 0) {
        return 0;
    }

    {
        int tmp_id = new_temp(ctx);
        snprintf(a_name, sizeof(a_name), "mat_tmpA%d", tmp_id);
    }
    ir_printf(ctx, out, "decl_matrix %s, %d, %d", a_name, n, n);
    add_matrix_info(ctx, a_name, n, n);
    if (!emit_matrix_copy(ctx, a_name, src, n, n, out)) {
        return 0;
    }

    i_var_id = new_temp(ctx);
    j_var_id = new_temp(ctx);
    r_var_id = new_temp(ctx);
    snprintf(i_var, sizeof(i_var), "mat_i%d", i_var_id);
    snprintf(j_var, sizeof(j_var), "mat_j%d", j_var_id);
    snprintf(r_var, sizeof(r_var), "mat_r%d", r_var_id);

    ir_printf(ctx, out, "decl int %s", i_var);
    ir_printf(ctx, out, "decl int %s", j_var);
    ir_printf(ctx, out, "decl int %s", r_var);
    i_addr = new_temp(ctx);
    j_addr = new_temp(ctx);
    r_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", i_addr, i_var);
    ir_printf(ctx, out, "t%d = addr %s", j_addr, j_var);
    ir_printf(ctx, out, "t%d = addr %s", r_addr, r_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    zero_dec = new_temp(ctx);
    one_dec = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const decimal 0.0", zero_dec);
    ir_printf(ctx, out, "t%d = const decimal 1.0", one_dec);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);

    ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
    i_label = new_label(ctx);
    i_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", i_label);
    {
        int i_val = new_temp(ctx);
        int i_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", j_addr, zero);
    j_label = new_label(ctx);
    j_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", j_label);
    {
        int j_val = new_temp(ctx);
        int j_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
    }
    {
        int i_val = new_temp(ctx);
        int j_val = new_temp(ctx);
        int eq = new_temp(ctx);
        int set_one_label = new_label(ctx);
        int after_set_label = new_label(ctx);
        int addr_dst;

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, i_val, j_val);
        ir_printf(ctx, out, "ifz t%d goto L%d", eq, set_one_label);
        addr_dst = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, one_dec);
        ir_printf(ctx, out, "goto L%d", after_set_label);
        ir_printf(ctx, out, "label L%d", set_one_label);
        addr_dst = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, zero_dec);
        ir_printf(ctx, out, "label L%d", after_set_label);
    }
    {
        int j_val = new_temp(ctx);
        int j_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
    }
    ir_printf(ctx, out, "goto L%d", j_label);
    ir_printf(ctx, out, "label L%d", j_end_label);

    {
        int i_val = new_temp(ctx);
        int i_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
    }
    ir_printf(ctx, out, "goto L%d", i_label);
    ir_printf(ctx, out, "label L%d", i_end_label);

    ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
    i_label = new_label(ctx);
    i_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", i_label);
    {
        int i_val = new_temp(ctx);
        int i_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
    }

    {
        int i_val = new_temp(ctx);
        int addr_pivot;
        int pivot;
        int inv_pivot;
        int one_val;

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        addr_pivot = emit_matrix_elem_addr(ctx, a_name, i_val, i_val, out);
        pivot = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", pivot, addr_pivot);
        one_val = new_temp(ctx);
        inv_pivot = new_temp(ctx);
        ir_printf(ctx, out, "t%d = const decimal 1.0", one_val);
        emit_safe_div(ctx, inv_pivot, one_val, pivot, out);

        ir_printf(ctx, out, "store [t%d], t%d", j_addr, zero);
        j_label = new_label(ctx);
        j_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", j_label);
        {
            int j_val = new_temp(ctx);
            int j_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
        }
        {
            int j_val = new_temp(ctx);
            int addr_a;
            int addr_inv;
            int val_a;
            int val_inv;
            int val_a_f;
            int val_inv_f;
            int new_a;
            int new_inv;

            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            addr_a = emit_matrix_elem_addr(ctx, a_name, i_val, j_val, out);
            addr_inv = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);
            val_a = new_temp(ctx);
            val_inv = new_temp(ctx);
            val_a_f = new_temp(ctx);
            val_inv_f = new_temp(ctx);
            new_a = new_temp(ctx);
            new_inv = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv, addr_inv);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_a_f, val_a, one_dec);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_inv_f, val_inv, one_dec);
            ir_printf(ctx, out, "t%d = * t%d, t%d", new_a, val_a_f, inv_pivot);
            ir_printf(ctx, out, "t%d = * t%d, t%d", new_inv, val_inv_f, inv_pivot);
            ir_printf(ctx, out, "store [t%d], t%d", addr_a, new_a);
            ir_printf(ctx, out, "store [t%d], t%d", addr_inv, new_inv);
        }
        {
            int j_val = new_temp(ctx);
            int j_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
        }
        ir_printf(ctx, out, "goto L%d", j_label);
        ir_printf(ctx, out, "label L%d", j_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", r_addr, zero);
    r_label = new_label(ctx);
    r_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", r_label);
    {
        int r_val = new_temp(ctx);
        int r_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", r_val, r_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", r_cmp, r_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", r_cmp, r_end_label);
    }
    {
        int i_val = new_temp(ctx);
        int r_val = new_temp(ctx);
        int eq = new_temp(ctx);
        int skip_label = new_label(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = load %s", r_val, r_var);
        ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, r_val, i_val);
        ir_printf(ctx, out, "ifz t%d goto L%d", eq, skip_label);
        {
            int r_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", r_next, r_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", r_addr, r_next);
            ir_printf(ctx, out, "goto L%d", r_label);
        }
        ir_printf(ctx, out, "label L%d", skip_label);
    }
    {
        int i_val = new_temp(ctx);
        int r_val = new_temp(ctx);
        int factor;
        int factor_f;

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = load %s", r_val, r_var);
        {
            int addr_factor = emit_matrix_elem_addr(ctx, a_name, r_val, i_val, out);
            factor = new_temp(ctx);
            factor_f = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", factor, addr_factor);
            ir_printf(ctx, out, "t%d = * t%d, t%d", factor_f, factor, one_dec);
        }

        ir_printf(ctx, out, "store [t%d], t%d", j_addr, zero);
        j_label = new_label(ctx);
        j_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", j_label);
        {
            int j_val = new_temp(ctx);
            int j_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
        }
        {
            int j_val = new_temp(ctx);
            int addr_a_r;
            int addr_a_i;
            int addr_inv_r;
            int addr_inv_i;
            int val_a_r;
            int val_a_i;
            int val_inv_r;
            int val_inv_i;
            int val_a_r_f;
            int val_a_i_f;
            int val_inv_r_f;
            int val_inv_i_f;
            int mul_a;
            int mul_inv;
            int new_a;
            int new_inv;

            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            addr_a_r = emit_matrix_elem_addr(ctx, a_name, r_val, j_val, out);
            addr_a_i = emit_matrix_elem_addr(ctx, a_name, i_val, j_val, out);
            addr_inv_r = emit_matrix_elem_addr(ctx, dst, r_val, j_val, out);
            addr_inv_i = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);

            val_a_r = new_temp(ctx);
            val_a_i = new_temp(ctx);
            val_inv_r = new_temp(ctx);
            val_inv_i = new_temp(ctx);
            val_a_r_f = new_temp(ctx);
            val_a_i_f = new_temp(ctx);
            val_inv_r_f = new_temp(ctx);
            val_inv_i_f = new_temp(ctx);
            mul_a = new_temp(ctx);
            mul_inv = new_temp(ctx);
            new_a = new_temp(ctx);
            new_inv = new_temp(ctx);

            ir_printf(ctx, out, "t%d = load [t%d]", val_a_r, addr_a_r);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a_i, addr_a_i);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv_r, addr_inv_r);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv_i, addr_inv_i);

            ir_printf(ctx, out, "t%d = * t%d, t%d", val_a_r_f, val_a_r, one_dec);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_a_i_f, val_a_i, one_dec);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_inv_r_f, val_inv_r, one_dec);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_inv_i_f, val_inv_i, one_dec);

            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_a, factor_f, val_a_i_f);
            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_inv, factor_f, val_inv_i_f);
            ir_printf(ctx, out, "t%d = - t%d, t%d", new_a, val_a_r_f, mul_a);
            ir_printf(ctx, out, "t%d = - t%d, t%d", new_inv, val_inv_r_f, mul_inv);
            ir_printf(ctx, out, "store [t%d], t%d", addr_a_r, new_a);
            ir_printf(ctx, out, "store [t%d], t%d", addr_inv_r, new_inv);
        }
        {
            int j_val = new_temp(ctx);
            int j_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
        }
        ir_printf(ctx, out, "goto L%d", j_label);
        ir_printf(ctx, out, "label L%d", j_end_label);
    }
    {
        int r_val = new_temp(ctx);
        int r_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", r_val, r_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", r_next, r_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", r_addr, r_next);
    }
    ir_printf(ctx, out, "goto L%d", r_label);
    ir_printf(ctx, out, "label L%d", r_end_label);

    {
        int i_val = new_temp(ctx);
        int i_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
    }
    ir_printf(ctx, out, "goto L%d", i_label);
    ir_printf(ctx, out, "label L%d", i_end_label);

    return 1;
}

static int emit_array_sort(IRContext *ctx, const char *name, int size, FILE *out)
{
    int i_var_id;
    int j_var_id;
    char i_var[32];
    char j_var[32];
    int i_addr;
    int j_addr;
    int zero;
    int one;
    int size_const;
    int i_label;
    int i_end_label;
    int j_label;
    int j_end_label;

    if (!name) {
        return 0;
    }
    if (size <= 1) {
        return 1;
    }

    i_var_id = new_temp(ctx);
    j_var_id = new_temp(ctx);
    snprintf(i_var, sizeof(i_var), "arr_i%d", i_var_id);
    snprintf(j_var, sizeof(j_var), "arr_j%d", j_var_id);

    ir_printf(ctx, out, "decl int %s", i_var);
    ir_printf(ctx, out, "decl int %s", j_var);
    i_addr = new_temp(ctx);
    j_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", i_addr, i_var);
    ir_printf(ctx, out, "t%d = addr %s", j_addr, j_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    size_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", size_const, size);

    ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
    i_label = new_label(ctx);
    i_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", i_label);
    {
        int i_val = new_temp(ctx);
        int i_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, size_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", j_addr, zero);
    j_label = new_label(ctx);
    j_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", j_label);
    {
        int j_val = new_temp(ctx);
        int j_cmp = new_temp(ctx);
        int last = new_temp(ctx);
        int bound = new_temp(ctx);
        int i_val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = - t%d, t%d", last, size_const, one);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = - t%d, t%d", bound, last, i_val);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, bound);
        ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
    }

    {
        int j_val = new_temp(ctx);
        int j_next = new_temp(ctx);
        int addr_a;
        int addr_b;
        int val_a;
        int val_b;
        int gt;
        int no_swap_label;

        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);

        addr_a = emit_array_elem_addr(ctx, name, j_val, out);
        addr_b = emit_array_elem_addr(ctx, name, j_next, out);
        val_a = new_temp(ctx);
        val_b = new_temp(ctx);
        gt = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
        ir_printf(ctx, out, "t%d = gt t%d, t%d", gt, val_a, val_b);

        no_swap_label = new_label(ctx);
        ir_printf(ctx, out, "ifz t%d goto L%d", gt, no_swap_label);
        ir_printf(ctx, out, "store [t%d], t%d", addr_a, val_b);
        ir_printf(ctx, out, "store [t%d], t%d", addr_b, val_a);
        ir_printf(ctx, out, "label L%d", no_swap_label);
    }

    {
        int j_val = new_temp(ctx);
        int j_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
    }
    ir_printf(ctx, out, "goto L%d", j_label);
    ir_printf(ctx, out, "label L%d", j_end_label);

    {
        int i_val = new_temp(ctx);
        int i_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
    }
    ir_printf(ctx, out, "goto L%d", i_label);
    ir_printf(ctx, out, "label L%d", i_end_label);

    return 1;
}

static int emit_array_copy(IRContext *ctx, const char *dst, const char *src, int size, FILE *out)
{
    int idx_var_id;
    char idx_var[32];
    int idx_addr;
    int zero;
    int one;
    int size_const;
    int label;
    int end_label;

    if (!dst || !src || size <= 0) {
        return 0;
    }

    idx_var_id = new_temp(ctx);
    snprintf(idx_var, sizeof(idx_var), "arr_i%d", idx_var_id);
    ir_printf(ctx, out, "decl int %s", idx_var);
    idx_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", idx_addr, idx_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    size_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", size_const, size);

    ir_printf(ctx, out, "store [t%d], t%d", idx_addr, zero);
    label = new_label(ctx);
    end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", label);
    {
        int idx_val = new_temp(ctx);
        int cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", idx_val, idx_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", cmp, idx_val, size_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", cmp, end_label);
    }
    {
        int idx_val = new_temp(ctx);
        int addr_src;
        int addr_dst;
        int val_src;
        ir_printf(ctx, out, "t%d = load %s", idx_val, idx_var);
        addr_src = emit_array_elem_addr(ctx, src, idx_val, out);
        addr_dst = emit_array_elem_addr(ctx, dst, idx_val, out);
        val_src = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_src, addr_src);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_src);
    }
    {
        int idx_val = new_temp(ctx);
        int idx_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", idx_val, idx_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", idx_next, idx_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", idx_addr, idx_next);
    }
    ir_printf(ctx, out, "goto L%d", label);
    ir_printf(ctx, out, "label L%d", end_label);

    return 1;
}

static int emit_const(IRContext *ctx, const char *kind, const char *value, FILE *out)
{
    int t = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const %s %s", t, kind, safe_str(value));
    return t;
}

static int emit_lvalue_addr(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return -1;
    }
    if (strcmp(node->type, "id") == 0) {
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", t, safe_str(node->value));
        return t;
    }
    if (strcmp(node->type, "array_access") == 0) {
        int base = new_temp(ctx);
        int idx = emit_expr(ctx, node->left, out);
        int off = new_temp(ctx);
        int addr = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", base, safe_str(node->value));
        ir_printf(ctx, out, "t%d = mul t%d, 4", off, idx);
        ir_printf(ctx, out, "t%d = add t%d, t%d", addr, base, off);
        return addr;
    }
    if (strcmp(node->type, "matrix_access") == 0) {
        int base = new_temp(ctx);
        int r = emit_expr(ctx, node->left, out);
        int c = emit_expr(ctx, node->right, out);
        int row_off = new_temp(ctx);
        int col_off = new_temp(ctx);
        int off = new_temp(ctx);
        int addr = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", base, safe_str(node->value));
        ir_printf(ctx, out, "t%d = mul t%d, cols(%s)", row_off, r, safe_str(node->value));
        ir_printf(ctx, out, "t%d = add t%d, t%d", col_off, row_off, c);
        ir_printf(ctx, out, "t%d = mul t%d, 4", off, col_off);
        ir_printf(ctx, out, "t%d = add t%d, t%d", addr, base, off);
        return addr;
    }
    return -1;
}

static int emit_expr_internal(IRContext *ctx, ASTNode *node, FILE *out, ExprCache **cache, int cse_enabled)
{
    ConstVal folded;
    char *key = NULL;

    if (!node) {
        return -1;
    }
    if (cse_enabled && cache) {
        key = expr_key(node);
        if (key) {
            ExprCache *hit = expr_cache_find(*cache, key);
            if (hit) {
                free(key);
                return hit->temp;
            }
        }
    }
    if (ir_opt_enabled(ctx) && eval_const_expr(ctx, node, &folded)) {
        int temp = emit_const_value(ctx, &folded, out);
        if (cse_enabled && cache && key && temp >= 0) {
            expr_cache_add(cache, key, temp);
            return temp;
        }
        free(key);
        return temp;
    }

    if (strcmp(node->type, "int") == 0) {
        int temp = emit_const(ctx, "int", node->value, out);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, temp);
            return temp;
        }
        free(key);
        return temp;
    }
    if (strcmp(node->type, "decimal") == 0) {
        int temp = emit_const(ctx, "decimal", node->value, out);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, temp);
            return temp;
        }
        free(key);
        return temp;
    }
    if (strcmp(node->type, "char") == 0) {
        int temp = emit_const(ctx, "char", node->value, out);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, temp);
            return temp;
        }
        free(key);
        return temp;
    }
    if (strcmp(node->type, "string") == 0) {
        int temp = emit_const(ctx, "string", node->value, out);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, temp);
            return temp;
        }
        free(key);
        return temp;
    }
    if (strcmp(node->type, "id") == 0) {
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", t, safe_str(node->value));
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "array_access") == 0 || strcmp(node->type, "matrix_access") == 0) {
        int addr = emit_lvalue_addr(ctx, node, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", t, addr);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 ||
        strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0 ||
        strcmp(node->type, "%") == 0 ||
        strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0 ||
        strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0) {
        ConstVal left_val;
        ConstVal right_val;
        int left_is_const = ir_opt_enabled(ctx) && eval_const_expr(ctx, node->left, &left_val);
        int right_is_const = ir_opt_enabled(ctx) && eval_const_expr(ctx, node->right, &right_val);

        if ((strcmp(node->type, "+") == 0 || strcmp(node->type, "*") == 0) &&
            left_is_const && !right_is_const) {
            ASTNode *tmp = node->left;
            node->left = node->right;
            node->right = tmp;
            ConstVal tmp_val = left_val;
            left_val = right_val;
            right_val = tmp_val;
            left_is_const = right_is_const;
            right_is_const = 1;
        }

        if (strcmp(node->type, "+") == 0) {
            if (right_is_const && const_is_zero(&right_val)) {
                int t = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                free(key);
                return t;
            }
        }
        if (strcmp(node->type, "-") == 0) {
            if (right_is_const && const_is_zero(&right_val)) {
                int t = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                free(key);
                return t;
            }
            if (left_is_const && const_is_zero(&left_val)) {
                int v = emit_expr_internal(ctx, node->right, out, cache, cse_enabled);
                int t = new_temp(ctx);
                ir_printf(ctx, out, "t%d = uminus t%d", t, v);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
        }
        if (strcmp(node->type, "*") == 0) {
            if (right_is_const && const_is_zero(&right_val)) {
                int t = emit_const(ctx, "int", "0", out);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
            if (right_is_const && const_is_one(&right_val)) {
                int t = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                free(key);
                return t;
            }
            if (right_is_const && const_is_neg_one(&right_val)) {
                int v = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                int t = new_temp(ctx);
                ir_printf(ctx, out, "t%d = uminus t%d", t, v);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
            if (right_is_const && right_val.kind == CONST_INT && right_val.i == 2) {
                int v = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                int t = new_temp(ctx);
                ir_printf(ctx, out, "t%d = + t%d, t%d", t, v, v);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
        }
        if (strcmp(node->type, "/") == 0) {
            if (right_is_const && const_is_one(&right_val)) {
                int t = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                free(key);
                return t;
            }
            if (right_is_const && const_is_neg_one(&right_val)) {
                int v = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
                int t = new_temp(ctx);
                ir_printf(ctx, out, "t%d = uminus t%d", t, v);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
        }

        int lhs = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
        int rhs = emit_expr_internal(ctx, node->right, out, cache, cse_enabled);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", t, node->type, lhs, rhs);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "not") == 0 || strcmp(node->type, "uminus") == 0) {
        int v = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d", t, node->type, v);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "size") == 0 ||
        strcmp(node->type, "sort") == 0 ||
        strcmp(node->type, "transpose") == 0 ||
        strcmp(node->type, "det") == 0 ||
        strcmp(node->type, "inv") == 0 ||
        strcmp(node->type, "shape") == 0) {
        if (strcmp(node->type, "size") == 0 && node->left && strcmp(node->left->type, "id") == 0) {
            ArrayInfo *info = find_array_info(ctx, node->left->value);
            if (info && info->size > 0) {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", info->size);
                int t = emit_const(ctx, "int", buf, out);
                if (cse_enabled && cache && key) {
                    expr_cache_add(cache, key, t);
                    return t;
                }
                free(key);
                return t;
            }
        }
        if (strcmp(node->type, "det") == 0 && node->left && strcmp(node->left->type, "id") == 0) {
            MatrixInfo *info = find_matrix_info(ctx, node->left->value);
            if (info && info->rows > 0 && info->cols > 0 && info->rows == info->cols) {
                int det_val = -1;
                if (matrix_is_identity(info)) {
                    det_val = emit_const(ctx, "int", "1", out);
                } else if (matrix_is_diagonal(info) ||
                           matrix_is_upper_triangular(info) ||
                           matrix_is_lower_triangular(info)) {
                    det_val = emit_matrix_det_diag(ctx, node->left->value, info->rows, out);
                } else {
                    det_val = emit_matrix_det(ctx, node->left->value, info->rows, out);
                }
                if (det_val >= 0) {
                    if (cse_enabled && cache && key) {
                        expr_cache_add(cache, key, det_val);
                        return det_val;
                    }
                    free(key);
                    return det_val;
                }
            }
        }
        int v = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d", t, node->type, v);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "matadd") == 0 ||
        strcmp(node->type, "matsub") == 0 ||
        strcmp(node->type, "matmul") == 0) {
        int a = emit_expr_internal(ctx, node->left, out, cache, cse_enabled);
        int b = emit_expr_internal(ctx, node->right, out, cache, cse_enabled);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", t, node->type, a, b);
        if (cse_enabled && cache && key) {
            expr_cache_add(cache, key, t);
            return t;
        }
        free(key);
        return t;
    }
    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        const char *lvalue_name = NULL;
        ConstBinding *binding = NULL;
        ConstVal old_val;
        int has_old = 0;
        if (node->left && strcmp(node->left->type, "id") == 0) {
            lvalue_name = node->left->value;
            binding = const_find_binding(ctx, lvalue_name);
            if (binding) {
                old_val = binding->val;
                has_old = 1;
            }
            const_unbind(ctx, lvalue_name);
        }

        int addr = emit_lvalue_addr(ctx, node->left, out);
        int cur = new_temp(ctx);
        int next = new_temp(ctx);
        int delta = new_temp(ctx);
        int result = new_temp(ctx);
        const char *op = (strstr(node->type, "inc") != NULL) ? "add" : "sub";
        ir_printf(ctx, out, "t%d = load [t%d]", cur, addr);
        ir_printf(ctx, out, "t%d = const int 1", delta);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", next, op, cur, delta);
        ir_printf(ctx, out, "store [t%d], t%d", addr, next);
        if (strncmp(node->type, "pre", 3) == 0) {
            ir_printf(ctx, out, "t%d = mov t%d", result, next);
        } else {
            ir_printf(ctx, out, "t%d = mov t%d", result, cur);
        }
        if (lvalue_name && has_old && const_is_numeric(&old_val)) {
            ConstVal new_val;
            long long li = 0;
            double ld = 0.0;
            int delta_sign = (strstr(node->type, "inc") != NULL) ? 1 : -1;
            if (old_val.kind == CONST_DECIMAL) {
                if (const_to_double(&old_val, &ld)) {
                    new_val.kind = CONST_DECIMAL;
                    new_val.d = ld + (double) delta_sign;
                    new_val.i = 0;
                    new_val.s = NULL;
                    const_bind(ctx, lvalue_name, &new_val);
                }
            } else if (const_to_int(&old_val, &li)) {
                new_val.kind = CONST_INT;
                new_val.i = li + delta_sign;
                new_val.d = 0.0;
                new_val.s = NULL;
                const_bind(ctx, lvalue_name, &new_val);
            }
        }
        return result;
    }

    free(key);
    return -1;
}

static int emit_expr(IRContext *ctx, ASTNode *node, FILE *out)
{
    return emit_expr_internal(ctx, node, out, &ctx->cse_cache, ir_opt_enabled(ctx));
}

static void emit_assignment(IRContext *ctx, ASTNode *node, FILE *out)
{
    const char *lhs_name = NULL;
    int lhs_is_scalar = 0;

    if (!node) {
        return;
    }
    lhs_name = node->value;
    if (lhs_name && !find_matrix_info(ctx, lhs_name) && !find_array_info(ctx, lhs_name)) {
        lhs_is_scalar = 1;
        const_unbind(ctx, lhs_name);
    }
    if (node->value) {
        MatrixInfo *lhs_info = find_matrix_info(ctx, node->value);
        ArrayInfo *lhs_arr = find_array_info(ctx, node->value);
        ASTNode *rhs_node = node->right ? node->right : node->left;
        if (lhs_info && rhs_node && rhs_node->type) {
            if (strcmp(rhs_node->type, "transpose") == 0 &&
                rhs_node->left && rhs_node->left->type &&
                strcmp(rhs_node->left->type, "transpose") == 0) {
                const char *src_name = (rhs_node->left->left && strcmp(rhs_node->left->left->type, "id") == 0)
                                        ? rhs_node->left->left->value : NULL;
                MatrixInfo *src_info = find_matrix_info(ctx, src_name);
                int rows = lhs_info->rows;
                int cols = lhs_info->cols;
                if (src_info && (rows <= 0 || cols <= 0)) {
                    rows = src_info->rows;
                    cols = src_info->cols;
                }
                if (ir_opt_enabled(ctx) &&
                    src_info && src_name && strcmp(node->value, src_name) == 0 &&
                    rows > 0 && cols > 0 && rows == cols) {
                    if (emit_matrix_transpose_inplace(ctx, node->value, rows, out)) {
                        matrix_props_copy(lhs_info, src_info);
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (src_info && emit_matrix_copy(ctx, node->value, src_name, rows, cols, out)) {
                    matrix_props_copy(lhs_info, src_info);
                    matrix_invalidate_name(ctx, node->value);
                    return;
                }
            }
            if (strcmp(rhs_node->type, "transpose") == 0) {
                const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                MatrixInfo *src_info = find_matrix_info(ctx, src_name);
                int rows = lhs_info->rows;
                int cols = lhs_info->cols;
                if (src_info && (rows <= 0 || cols <= 0)) {
                    rows = src_info->cols;
                    cols = src_info->rows;
                }
                if (ir_opt_enabled(ctx) &&
                    src_info && src_name && strcmp(node->value, src_name) == 0 &&
                    rows > 0 && cols > 0 && rows == cols) {
                    if (emit_matrix_transpose_inplace(ctx, node->value, rows, out)) {
                        if (src_info->props_known) {
                            matrix_props_set(lhs_info, 1, src_info->is_zero, src_info->is_identity, src_info->is_diagonal,
                                             src_info->is_symmetric, src_info->is_lower_triangular, src_info->is_upper_triangular);
                        } else {
                            matrix_props_unknown(lhs_info);
                        }
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (ir_opt_enabled(ctx) && src_info && rows == 1 && cols == 1) {
                    if (emit_matrix_scalar_copy(ctx, node->value, src_name, out)) {
                        matrix_props_copy(lhs_info, src_info);
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (ir_opt_enabled(ctx) && src_info && matrix_is_symmetric(src_info)) {
                    if (src_name && strcmp(node->value, src_name) == 0) {
                        matrix_props_copy(lhs_info, src_info);
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                    if (emit_matrix_copy(ctx, node->value, src_name, rows, cols, out)) {
                        matrix_props_copy(lhs_info, src_info);
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (src_info && src_name && strcmp(node->value, src_name) == 0) {
                    if (emit_matrix_transpose_via_temp(ctx, node->value, src_name,
                                                      src_info->rows, src_info->cols,
                                                      rows, cols, out)) {
                        if (src_info->props_known) {
                            matrix_props_set(lhs_info, 1, src_info->is_zero, src_info->is_identity, src_info->is_diagonal,
                                             src_info->is_symmetric, src_info->is_lower_triangular, src_info->is_upper_triangular);
                        } else {
                            matrix_props_unknown(lhs_info);
                        }
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (src_info && emit_matrix_transpose(ctx, node->value, src_name, rows, cols, out)) {
                    if (src_info->props_known) {
                        matrix_props_set(lhs_info, 1, src_info->is_zero, src_info->is_identity, src_info->is_diagonal,
                                         src_info->is_symmetric, src_info->is_lower_triangular, src_info->is_upper_triangular);
                    } else {
                        matrix_props_unknown(lhs_info);
                    }
                    matrix_invalidate_name(ctx, node->value);
                    return;
                }
            }
            if (strcmp(rhs_node->type, "inv") == 0) {
                const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                MatrixInfo *src_info = find_matrix_info(ctx, src_name);
                if (src_info && src_info->rows > 0 && src_info->cols > 0 && src_info->rows == src_info->cols) {
                    char *inv_key = NULL;
                    if (ir_opt_enabled(ctx) && src_name) {
                        inv_key = mat_unary_key("inv", src_name);
                        if (inv_key) {
                            MatExprCache *hit = mat_cache_find(ctx->mat_cse_cache, inv_key);
                            if (hit && hit->name) {
                                MatrixInfo *cached_info = find_matrix_info(ctx, hit->name);
                                if (strcmp(hit->name, node->value) == 0) {
                                    matrix_props_copy(lhs_info, cached_info);
                                    matrix_invalidate_name(ctx, node->value);
                                    free(inv_key);
                                    return;
                                }
                                if (emit_matrix_copy(ctx, node->value, hit->name, src_info->rows, src_info->cols, out)) {
                                    matrix_props_copy(lhs_info, cached_info);
                                    matrix_invalidate_name(ctx, node->value);
                                    free(inv_key);
                                    return;
                                }
                            }
                        }
                    }
                    if (emit_matrix_inverse(ctx, node->value, src_name, src_info->rows, out)) {
                        matrix_props_unknown(lhs_info);
                        if (ir_opt_enabled(ctx) && inv_key) {
                            mat_cache_add(&ctx->mat_cse_cache, inv_key, node->value);
                            inv_key = NULL;
                        }
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                    free(inv_key);
                }
            }
            if (strcmp(rhs_node->type, "matadd") == 0 || strcmp(rhs_node->type, "matsub") == 0 ||
                strcmp(rhs_node->type, "matmul") == 0) {
                if (ir_opt_enabled(ctx) &&
                    strcmp(rhs_node->type, "matadd") == 0 &&
                    rhs_node->left && rhs_node->left->type &&
                    strcmp(rhs_node->left->type, "matadd") == 0) {
                    ASTNode *l1 = rhs_node->left->left;
                    ASTNode *l2 = rhs_node->left->right;
                    ASTNode *r = rhs_node->right;
                    const char *n1 = (l1 && strcmp(l1->type, "id") == 0) ? l1->value : NULL;
                    const char *n2 = (l2 && strcmp(l2->type, "id") == 0) ? l2->value : NULL;
                    const char *n3 = (r && strcmp(r->type, "id") == 0) ? r->value : NULL;
                    MatrixInfo *i1 = find_matrix_info(ctx, n1);
                    MatrixInfo *i2 = find_matrix_info(ctx, n2);
                    MatrixInfo *i3 = find_matrix_info(ctx, n3);
                    if (i1 && i2 && i3 &&
                        i1->rows == i2->rows && i1->cols == i2->cols &&
                        i1->rows == i3->rows && i1->cols == i3->cols) {
                        if (emit_matrix_add3(ctx, node->value, n1, n2, n3,
                                             i1->rows, i1->cols, out)) {
                            matrix_props_unknown(lhs_info);
                            {
                                char *key = str_printf("mop:matadd(id:%s,id:%s,id:%s)", n1, n2, n3);
                                mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                            }
                            matrix_invalidate_name(ctx, node->value);
                            return;
                        }
                    }
                }
                const char *left_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                const char *right_name = (rhs_node->right && strcmp(rhs_node->right->type, "id") == 0) ? rhs_node->right->value : NULL;
                MatrixInfo *left_info = find_matrix_info(ctx, left_name);
                MatrixInfo *right_info = find_matrix_info(ctx, right_name);
                if (left_info && right_info) {
                    if (ir_opt_enabled(ctx)) {
                        char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                        MatExprCache *hit = key ? mat_cache_find(ctx->mat_cse_cache, key) : NULL;
                        if (hit && hit->name) {
                            MatrixInfo *cached_info = find_matrix_info(ctx, hit->name);
                            if (strcmp(hit->name, node->value) == 0) {
                                matrix_props_copy(lhs_info, cached_info);
                                free(key);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                            if (emit_matrix_copy(ctx, node->value, hit->name, left_info->rows, left_info->cols, out)) {
                                matrix_props_copy(lhs_info, cached_info);
                                free(key);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (key) {
                            free(key);
                        }
                    }
                    if (strcmp(rhs_node->type, "matmul") == 0) {
                        if (ir_opt_enabled(ctx) &&
                            left_info->rows == 1 && left_info->cols == 1 &&
                            right_info->rows == 1 && right_info->cols == 1) {
                            if (emit_matrix_scalar_binop(ctx, node->value, left_name, right_name, "*", out)) {
                                matrix_props_unknown(lhs_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && matrix_is_diagonal(left_info)) {
                            if (emit_matrix_mul_diag_left(ctx, node->value, left_name, right_name,
                                                          left_info->rows, right_info->cols, out)) {
                                if (matrix_is_diagonal(right_info)) {
                                    matrix_props_set(lhs_info, 1, 0, 0, 1, 1, 1, 1);
                                } else {
                                    matrix_props_unknown(lhs_info);
                                }
                                if (ir_opt_enabled(ctx)) {
                                    char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                    mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                                }
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && matrix_is_diagonal(right_info)) {
                            if (emit_matrix_mul_diag_right(ctx, node->value, left_name, right_name,
                                                           left_info->rows, right_info->cols, out)) {
                                if (matrix_is_diagonal(left_info)) {
                                    matrix_props_set(lhs_info, 1, 0, 0, 1, 1, 1, 1);
                                } else {
                                    matrix_props_unknown(lhs_info);
                                }
                                if (ir_opt_enabled(ctx)) {
                                    char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                    mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                                }
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) &&
                            left_name && right_name && strcmp(left_name, right_name) == 0 &&
                            matrix_is_symmetric(left_info) && strcmp(node->value, left_name) != 0) {
                            if (emit_matrix_mul_sym_self(ctx, node->value, left_name, left_info->rows, out)) {
                                matrix_props_set(lhs_info, 1, 0, 0, 0, 1, 0, 0);
                                if (ir_opt_enabled(ctx)) {
                                    char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                    mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                                }
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) &&
                            (matrix_is_zero(left_info) || matrix_is_zero(right_info))) {
                            if (emit_matrix_zero_fill(ctx, node->value, left_info->rows, right_info->cols, out)) {
                                matrix_props_set(lhs_info, 1, 1, 0, 1, 1, 1, 1);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && matrix_is_identity(left_info)) {
                            if (emit_matrix_copy(ctx, node->value, right_name, left_info->rows, right_info->cols, out)) {
                                matrix_props_copy(lhs_info, right_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && matrix_is_identity(right_info)) {
                            if (emit_matrix_copy(ctx, node->value, left_name, left_info->rows, left_info->cols, out)) {
                                matrix_props_copy(lhs_info, left_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && left_info->cols == 1 && right_info->rows == 1) {
                            if (emit_matrix_mul_inner1(ctx, node->value, left_name, right_name,
                                                       left_info->rows, right_info->cols, out)) {
                                matrix_props_unknown(lhs_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) &&
                            left_info->rows == left_info->cols &&
                            left_info->rows == right_info->rows &&
                            right_info->rows == right_info->cols &&
                            left_info->rows == right_info->rows &&
                            (left_info->rows == 2 || left_info->rows == 3 || left_info->rows == 4)) {
                            if (emit_matrix_mul_unrolled(ctx, node->value, left_name, right_name,
                                                         left_info->rows, out)) {
                                matrix_props_unknown(lhs_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_o2_enabled(ctx) &&
                            ir_should_use_cache_local_matmul(ctx->opt_level, left_info->rows,
                                                             left_info->cols, right_info->cols)) {
                            if (emit_matrix_mul_cache_locality(ctx, node->value, left_name, right_name,
                                                               left_info->rows, left_info->cols,
                                                               right_info->cols, out)) {
                                matrix_props_unknown(lhs_info);
                                if (ir_opt_enabled(ctx)) {
                                    char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                    mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                                }
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (emit_matrix_mul(ctx, node->value, left_name, right_name,
                                            left_info->rows, left_info->cols, right_info->cols, out)) {
                            if (ir_opt_enabled(ctx) &&
                                matrix_is_diagonal(left_info) && matrix_is_diagonal(right_info)) {
                                matrix_props_set(lhs_info, 1, 0, 0, 1, 1, 1, 1);
                            } else {
                                matrix_props_unknown(lhs_info);
                            }
                            if (ir_opt_enabled(ctx)) {
                                char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                            }
                            matrix_invalidate_name(ctx, node->value);
                            return;
                        }
                    } else {
                        if (ir_opt_enabled(ctx) &&
                            left_info->rows == 1 && left_info->cols == 1 &&
                            right_info->rows == 1 && right_info->cols == 1) {
                            const char *op = (strcmp(rhs_node->type, "matadd") == 0) ? "+" : "-";
                            if (emit_matrix_scalar_binop(ctx, node->value, left_name, right_name, op, out)) {
                                matrix_props_unknown(lhs_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) && matrix_is_zero(right_info)) {
                            if (emit_matrix_copy(ctx, node->value, left_name, left_info->rows, left_info->cols, out)) {
                                matrix_props_copy(lhs_info, left_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        if (ir_opt_enabled(ctx) &&
                            strcmp(rhs_node->type, "matadd") == 0 && matrix_is_zero(left_info)) {
                            if (emit_matrix_copy(ctx, node->value, right_name, right_info->rows, right_info->cols, out)) {
                                matrix_props_copy(lhs_info, right_info);
                                matrix_invalidate_name(ctx, node->value);
                                return;
                            }
                        }
                        const char *op = (strcmp(rhs_node->type, "matadd") == 0) ? "+" : "-";
                        if (emit_matrix_add_sub(ctx, node->value, left_name, right_name,
                                                left_info->rows, left_info->cols, op, out)) {
                            if (ir_opt_enabled(ctx) &&
                                matrix_is_diagonal(left_info) && matrix_is_diagonal(right_info)) {
                                int is_zero = matrix_is_zero(left_info) && matrix_is_zero(right_info);
                                matrix_props_set(lhs_info, 1, is_zero, 0, 1, 1, 1, 1);
                            } else if (ir_opt_enabled(ctx) &&
                                       matrix_is_symmetric(left_info) && matrix_is_symmetric(right_info)) {
                                matrix_props_set(lhs_info, 1, 0, 0, 0, 1, 0, 0);
                            } else if (ir_opt_enabled(ctx) &&
                                       matrix_is_upper_triangular(left_info) && matrix_is_upper_triangular(right_info)) {
                                matrix_props_set(lhs_info, 1, 0, 0, 0, 0, 1, 0);
                            } else if (ir_opt_enabled(ctx) &&
                                       matrix_is_lower_triangular(left_info) && matrix_is_lower_triangular(right_info)) {
                                matrix_props_set(lhs_info, 1, 0, 0, 0, 0, 0, 1);
                            } else {
                                matrix_props_unknown(lhs_info);
                            }
                            if (ir_opt_enabled(ctx)) {
                                char *key = mat_expr_key(rhs_node->type, left_name, right_name);
                                mat_cache_add(&ctx->mat_cse_cache, key, node->value);
                            }
                            matrix_invalidate_name(ctx, node->value);
                            return;
                        }
                    }
                }
            }
            if (strcmp(rhs_node->type, "id") == 0) {
                MatrixInfo *rhs_info = find_matrix_info(ctx, rhs_node->value);
                int rows = lhs_info->rows;
                int cols = lhs_info->cols;
                if (rhs_info && (rows <= 0 || cols <= 0)) {
                    rows = rhs_info->rows;
                    cols = rhs_info->cols;
                }
                if (ir_opt_enabled(ctx) && rhs_info && rows == 1 && cols == 1) {
                    if (emit_matrix_scalar_copy(ctx, node->value, rhs_node->value, out)) {
                        matrix_props_copy(lhs_info, rhs_info);
                        matrix_invalidate_name(ctx, node->value);
                        return;
                    }
                }
                if (rhs_info && emit_matrix_copy(ctx, node->value, rhs_node->value, rows, cols, out)) {
                    matrix_props_copy(lhs_info, rhs_info);
                    matrix_invalidate_name(ctx, node->value);
                    return;
                }
            }
        }
        if (lhs_arr && rhs_node && rhs_node->type && strcmp(rhs_node->type, "sort") == 0) {
            const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
            ArrayInfo *src_info = find_array_info(ctx, src_name);
            int size = lhs_arr->size;
            if (src_info && size <= 0) {
                size = src_info->size;
            }
            if (src_info && size > 0) {
                if (strcmp(node->value, src_name) != 0) {
                    emit_array_copy(ctx, node->value, src_name, size, out);
                }
                if (emit_array_sort(ctx, node->value, size, out)) {
                    matrix_invalidate_name(ctx, node->value);
                    return;
                }
            }
        }
        if (lhs_arr && rhs_node && rhs_node->type && strcmp(rhs_node->type, "id") == 0) {
            ArrayInfo *rhs_arr = find_array_info(ctx, rhs_node->value);
            int size = lhs_arr->size;
            if (rhs_arr && size <= 0) {
                size = rhs_arr->size;
            }
            if (rhs_arr && size > 0) {
                if (emit_array_copy(ctx, node->value, rhs_node->value, size, out)) {
                    matrix_invalidate_name(ctx, node->value);
                    return;
                }
            }
        }
    }
    if (node->value) {
        ASTNode *rhs_node = node->right ? node->right : node->left;
        if (rhs_node && rhs_node->type && strcmp(rhs_node->type, "det") == 0) {
            const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
            MatrixInfo *src_info = find_matrix_info(ctx, src_name);
            if (src_info && src_info->rows > 0 && src_info->cols > 0 && src_info->rows == src_info->cols) {
                int det_temp = -1;
                char *key = expr_key(rhs_node);
                if (ir_opt_enabled(ctx) && key) {
                    ExprCache *hit = expr_cache_find(ctx->cse_cache, key);
                    if (hit) {
                        ASTNode id_node;
                        int addr;
                        memset(&id_node, 0, sizeof(id_node));
                        id_node.type = "id";
                        id_node.value = node->value;
                        addr = emit_lvalue_addr(ctx, &id_node, out);
                        ir_printf(ctx, out, "store [t%d], t%d", addr, hit->temp);
                        if (lhs_is_scalar) {
                            const_unbind(ctx, lhs_name);
                        }
                        matrix_invalidate_name(ctx, node->value);
                        free(key);
                        return;
                    }
                }
                if (ir_opt_enabled(ctx) && matrix_is_identity(src_info)) {
                    det_temp = emit_const(ctx, "int", "1", out);
                } else if (ir_opt_enabled(ctx) &&
                           (matrix_is_diagonal(src_info) ||
                            matrix_is_upper_triangular(src_info) ||
                            matrix_is_lower_triangular(src_info))) {
                    det_temp = emit_matrix_det_diag(ctx, src_name, src_info->rows, out);
                } else {
                    det_temp = emit_matrix_det(ctx, src_name, src_info->rows, out);
                }
                if (det_temp >= 0) {
                    ASTNode id_node;
                    int addr;
                    memset(&id_node, 0, sizeof(id_node));
                    id_node.type = "id";
                    id_node.value = node->value;
                    addr = emit_lvalue_addr(ctx, &id_node, out);
                    ir_printf(ctx, out, "store [t%d], t%d", addr, det_temp);
                    if (ir_opt_enabled(ctx) && key) {
                        expr_cache_add(&ctx->cse_cache, key, det_temp);
                        key = NULL;
                    }
                    if (lhs_is_scalar) {
                        const_unbind(ctx, lhs_name);
                    }
                    matrix_invalidate_name(ctx, node->value);
                    free(key);
                    return;
                }
                free(key);
            }
        }
    }
    int addr = -1;
    if (node->value != NULL) {
        ASTNode id_node;
        memset(&id_node, 0, sizeof(id_node));
        id_node.type = "id";
        id_node.value = node->value;
        addr = emit_lvalue_addr(ctx, &id_node, out);
    } else {
        addr = emit_lvalue_addr(ctx, node->left, out);
    }
    ASTNode *rhs_node = node->right ? node->right : node->left;
    int rhs = emit_expr(ctx, rhs_node, out);
    ir_printf(ctx, out, "store [t%d], t%d", addr, rhs);
    if (lhs_is_scalar) {
        ConstVal val;
        if (eval_const_expr(ctx, rhs_node, &val)) {
            const_bind(ctx, lhs_name, &val);
        }
        matrix_invalidate_name(ctx, lhs_name);
    } else if (node->value) {
        matrix_invalidate_name(ctx, node->value);
    } else if (node->left && node->left->value) {
        matrix_invalidate_name(ctx, node->left->value);
    }
}

static void emit_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    const char *type = safe_str(node->left ? node->left->value : NULL);
    const char *name = safe_str(node->right ? node->right->value : NULL);
    if (name && name[0] != '\0') {
        const_unbind(ctx, name);
    }
    ir_printf(ctx, out, "decl %s %s", type, name);
}

static void emit_decl_assign(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    emit_decl(ctx, node, out);
    if (node->third) {
        ASTNode assign_node;
        memset(&assign_node, 0, sizeof(assign_node));
        assign_node.type = "assign";
        assign_node.value = node->right ? node->right->value : NULL;
        assign_node.left = NULL;
        assign_node.right = node->third;
        emit_assignment(ctx, &assign_node, out);
    }
}

static void emit_array_init_list(IRContext *ctx, const char *name, ASTNode *node, int *index, FILE *out)
{
    if (!node || !index) {
        return;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        emit_array_init_list(ctx, name, node->left, index, out);
        if (node->right) {
            emit_array_init_list(ctx, name, node->right, index, out);
        }
        return;
    }
    {
        char buf[32];
        int idx_temp;
        int addr;
        int val;
        snprintf(buf, sizeof(buf), "%d", *index);
        idx_temp = emit_const(ctx, "int", buf, out);
        addr = emit_array_elem_addr(ctx, name, idx_temp, out);
        val = emit_expr(ctx, node, out);
        ir_printf(ctx, out, "store [t%d], t%d", addr, val);
        (*index)++;
    }
}

static int emit_matrix_add3(IRContext *ctx, const char *dst, const char *a, const char *b, const char *c,
                            int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int base_a;
    int base_b;
    int base_c;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !a || !b || !c || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_a = new_temp(ctx);
    base_b = new_temp(ctx);
    base_c = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_b, safe_str(b));
    ir_printf(ctx, out, "t%d = addr %s", base_c, safe_str(c));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int row_base_b = new_temp(ctx);
        int row_base_c = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_val = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_a;
        int addr_b;
        int addr_c;
        int addr_dst;
        int val_a;
        int val_b;
        int val_c;
        int sum1;
        int sum2;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_b, base_b, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_c, base_c, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_a = new_temp(ctx);
        addr_b = new_temp(ctx);
        addr_c = new_temp(ctx);
        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_b, row_base_b, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_c, row_base_c, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);

        val_a = new_temp(ctx);
        val_b = new_temp(ctx);
        val_c = new_temp(ctx);
        sum1 = new_temp(ctx);
        sum2 = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
        ir_printf(ctx, out, "t%d = load [t%d]", val_c, addr_c);
        ir_printf(ctx, out, "t%d = + t%d, t%d", sum1, val_a, val_b);
        ir_printf(ctx, out, "t%d = + t%d, t%d", sum2, sum1, val_c);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, sum2);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

typedef struct {
    int rows;
    int cols;
    int elems;
    int known;
    int is_zero;
    int is_identity;
    int is_diagonal;
    int is_symmetric;
    int is_upper_triangular;
    int is_lower_triangular;
} MatrixInitProps;

static int const_is_one_val(const ConstVal *val)
{
    if (!val) {
        return 0;
    }
    if (val->kind == CONST_INT) {
        return val->i == 1;
    }
    if (val->kind == CONST_DECIMAL) {
        return val->d == 1.0;
    }
    if (val->kind == CONST_CHAR) {
        int ch = 0;
        if (!parse_char_value(val->s, &ch)) {
            return 0;
        }
        return ch == 1;
    }
    return 0;
}

static void matrix_props_init(MatrixInitProps *props, int rows, int cols)
{
    if (!props) {
        return;
    }
    props->rows = rows;
    props->cols = cols;
    props->elems = 0;
    props->known = 1;
    props->is_zero = 1;
    props->is_identity = 1;
    props->is_diagonal = 1;
    props->is_symmetric = 1;
    props->is_upper_triangular = 1;
    props->is_lower_triangular = 1;
    if (rows <= 0 || cols <= 0 || rows != cols) {
        props->is_identity = 0;
        props->is_diagonal = 0;
        props->is_symmetric = 0;
        props->is_upper_triangular = 0;
        props->is_lower_triangular = 0;
    }
}

static void matrix_props_visit_list(IRContext *ctx, ASTNode *node, int row, int *col,
                                    MatrixInitProps *props)
{
    if (!node || !col || !props || !props->known) {
        return;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        matrix_props_visit_list(ctx, node->left, row, col, props);
        if (node->right) {
            matrix_props_visit_list(ctx, node->right, row, col, props);
        }
        return;
    }
    if (row >= props->rows || *col >= props->cols) {
        props->known = 0;
        return;
    }
    {
        ConstVal val;
        int is_const = eval_const_expr(ctx, node, &val);
        int is_diag = (row == *col);

        if (!is_const) {
            props->is_zero = 0;
            props->is_identity = 0;
            if (!is_diag) {
                props->is_diagonal = 0;
            }
            props->is_symmetric = 0;
            props->is_upper_triangular = 0;
            props->is_lower_triangular = 0;
        } else {
            int is_zero = const_is_zero(&val);
            int is_one = const_is_one_val(&val);
            if (is_diag) {
                if (!is_one) {
                    props->is_identity = 0;
                }
                if (!is_zero) {
                    props->is_zero = 0;
                }
            } else {
                if (!is_zero) {
                    props->is_zero = 0;
                    props->is_identity = 0;
                    props->is_diagonal = 0;
                }
            }
            if (row > *col && !is_zero) {
                props->is_upper_triangular = 0;
            }
            if (row < *col && !is_zero) {
                props->is_lower_triangular = 0;
            }
        }
        props->elems++;
        (*col)++;
    }
}

static void matrix_props_visit_rows(IRContext *ctx, ASTNode *node, int *row, MatrixInitProps *props)
{
    if (!node || !row || !props || !props->known) {
        return;
    }
    if (node->type && strcmp(node->type, "init_rows") == 0) {
        matrix_props_visit_rows(ctx, node->left, row, props);
        matrix_props_visit_rows(ctx, node->right, row, props);
        return;
    }
    if (node->type && strcmp(node->type, "init_row") == 0) {
        int col = 0;
        matrix_props_visit_list(ctx, node->left, *row, &col, props);
        (*row)++;
        return;
    }
    props->known = 0;
}

static void matrix_collect_list(IRContext *ctx, ASTNode *node, int row, int *col,
                                int rows, int cols, ConstVal *vals, int *ok)
{
    if (!ok || !*ok || !node || !col || !vals) {
        return;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        matrix_collect_list(ctx, node->left, row, col, rows, cols, vals, ok);
        if (node->right) {
            matrix_collect_list(ctx, node->right, row, col, rows, cols, vals, ok);
        }
        return;
    }
    if (row >= rows || *col >= cols) {
        *ok = 0;
        return;
    }
    if (!eval_const_expr(ctx, node, &vals[row * cols + *col])) {
        *ok = 0;
        return;
    }
    (*col)++;
}

static void matrix_collect_rows(IRContext *ctx, ASTNode *node, int *row,
                                int rows, int cols, ConstVal *vals, int *ok)
{
    if (!ok || !*ok || !node || !row || !vals) {
        return;
    }
    if (node->type && strcmp(node->type, "init_rows") == 0) {
        matrix_collect_rows(ctx, node->left, row, rows, cols, vals, ok);
        matrix_collect_rows(ctx, node->right, row, rows, cols, vals, ok);
        return;
    }
    if (node->type && strcmp(node->type, "init_row") == 0) {
        int col = 0;
        matrix_collect_list(ctx, node->left, *row, &col, rows, cols, vals, ok);
        (*row)++;
        return;
    }
    *ok = 0;
}

static int matrix_collect_init(IRContext *ctx, ASTNode *node, int rows, int cols, ConstVal *vals)
{
    int row = 0;
    int ok = 1;

    if (!node || rows <= 0 || cols <= 0 || !vals) {
        return 0;
    }
    matrix_collect_rows(ctx, node, &row, rows, cols, vals, &ok);
    if (!ok || row != rows) {
        return 0;
    }
    return 1;
}

static int emit_matrix_inverse_triangular(IRContext *ctx, const char *dst, const char *src, int n,
                                          int is_upper, FILE *out)
{
    int col_var_id;
    int i_var_id;
    int k_var_id;
    char col_var[32];
    char i_var[32];
    char k_var[32];
    int col_addr;
    int i_addr;
    int k_addr;
    int zero;
    int one;
    int n_const;
    int neg_one;
    int col_label;
    int col_end_label;
    int i_label;
    int i_end_label;
    int k_label;
    int k_end_label;

    if (!dst || !src || n <= 0) {
        return 0;
    }

    /* Ensure destination starts from a clean zero matrix so any
       reads of not-yet-computed entries are deterministic. */
    if (!emit_matrix_zero_fill(ctx, dst, n, n, out)) {
        return 0;
    }

    col_var_id = new_temp(ctx);
    i_var_id = new_temp(ctx);
    k_var_id = new_temp(ctx);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);
    snprintf(i_var, sizeof(i_var), "mat_i%d", i_var_id);
    snprintf(k_var, sizeof(k_var), "mat_k%d", k_var_id);

    ir_printf(ctx, out, "decl int %s", col_var);
    ir_printf(ctx, out, "decl int %s", i_var);
    ir_printf(ctx, out, "decl int %s", k_var);
    col_addr = new_temp(ctx);
    i_addr = new_temp(ctx);
    k_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);
    ir_printf(ctx, out, "t%d = addr %s", i_addr, i_var);
    ir_printf(ctx, out, "t%d = addr %s", k_addr, k_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    n_const = new_temp(ctx);
    neg_one = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);
    ir_printf(ctx, out, "t%d = const int -1", neg_one);

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    if (is_upper) {
        int start = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", start, n_const, neg_one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, start);
        i_label = new_label(ctx);
        i_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", i_label);
        {
            int i_val = new_temp(ctx);
            int i_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = ge t%d, t%d", i_cmp, i_val, zero);
            ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
        }
        {
            int sum_var_id = new_temp(ctx);
            char sum_var[32];
            int sum_addr;
            snprintf(sum_var, sizeof(sum_var), "mat_sum%d", sum_var_id);
            ir_printf(ctx, out, "decl int %s", sum_var);
            sum_addr = new_temp(ctx);
            ir_printf(ctx, out, "t%d = addr %s", sum_addr, sum_var);
            ir_printf(ctx, out, "store [t%d], t%d", sum_addr, zero);

            {
                int i_val = new_temp(ctx);
                int k_start = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = + t%d, t%d", k_start, i_val, one);
                ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_start);
            }
            k_label = new_label(ctx);
            k_end_label = new_label(ctx);
            ir_printf(ctx, out, "label L%d", k_label);
            {
                int k_val = new_temp(ctx);
                int k_cmp = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                ir_printf(ctx, out, "t%d = lt t%d, t%d", k_cmp, k_val, n_const);
                ir_printf(ctx, out, "ifz t%d goto L%d", k_cmp, k_end_label);
            }
            {
                int i_val = new_temp(ctx);
                int col_val = new_temp(ctx);
                int k_val = new_temp(ctx);
                int addr_u;
                int addr_inv;
                int val_u;
                int val_inv;
                int mul_val;
                int sum_val;
                int sum_next;

                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                addr_u = emit_matrix_elem_addr(ctx, src, i_val, k_val, out);
                addr_inv = emit_matrix_elem_addr(ctx, dst, k_val, col_val, out);
                val_u = new_temp(ctx);
                val_inv = new_temp(ctx);
                mul_val = new_temp(ctx);
                sum_val = new_temp(ctx);
                sum_next = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load [t%d]", val_u, addr_u);
                ir_printf(ctx, out, "t%d = load [t%d]", val_inv, addr_inv);
                ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_u, val_inv);
                ir_printf(ctx, out, "t%d = load %s", sum_val, sum_var);
                ir_printf(ctx, out, "t%d = + t%d, t%d", sum_next, sum_val, mul_val);
                ir_printf(ctx, out, "store [t%d], t%d", sum_addr, sum_next);
            }
            {
                int k_val = new_temp(ctx);
                int k_next = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                ir_printf(ctx, out, "t%d = + t%d, t%d", k_next, k_val, one);
                ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_next);
            }
            ir_printf(ctx, out, "goto L%d", k_label);
            ir_printf(ctx, out, "label L%d", k_end_label);

            {
                int i_val = new_temp(ctx);
                int col_val = new_temp(ctx);
                int addr_diag;
                int val_diag;
                int sum_val;
                int rhs;
                int diff;
                int val_out;
                int addr_out;
                int eq;
                int rhs_label;
                int rhs_end;

                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
                addr_diag = emit_matrix_elem_addr(ctx, src, i_val, i_val, out);
                val_diag = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load [t%d]", val_diag, addr_diag);
                sum_val = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", sum_val, sum_var);

                rhs = new_temp(ctx);
                eq = new_temp(ctx);
                rhs_label = new_label(ctx);
                rhs_end = new_label(ctx);
                ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, i_val, col_val);
                ir_printf(ctx, out, "ifz t%d goto L%d", eq, rhs_label);
                ir_printf(ctx, out, "t%d = mov t%d", rhs, one);
                ir_printf(ctx, out, "goto L%d", rhs_end);
                ir_printf(ctx, out, "label L%d", rhs_label);
                ir_printf(ctx, out, "t%d = mov t%d", rhs, zero);
                ir_printf(ctx, out, "label L%d", rhs_end);

                diff = new_temp(ctx);
                val_out = new_temp(ctx);
                ir_printf(ctx, out, "t%d = - t%d, t%d", diff, rhs, sum_val);
                emit_safe_div(ctx, val_out, diff, val_diag, out);
                addr_out = emit_matrix_elem_addr(ctx, dst, i_val, col_val, out);
                ir_printf(ctx, out, "store [t%d], t%d", addr_out, val_out);
            }
        }
        {
            int i_val = new_temp(ctx);
            int i_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, neg_one);
            ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
        }
        ir_printf(ctx, out, "goto L%d", i_label);
        ir_printf(ctx, out, "label L%d", i_end_label);
    } else {
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
        i_label = new_label(ctx);
        i_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", i_label);
        {
            int i_val = new_temp(ctx);
            int i_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
        }
        {
            int sum_var_id = new_temp(ctx);
            char sum_var[32];
            int sum_addr;
            snprintf(sum_var, sizeof(sum_var), "mat_sum%d", sum_var_id);
            ir_printf(ctx, out, "decl int %s", sum_var);
            sum_addr = new_temp(ctx);
            ir_printf(ctx, out, "t%d = addr %s", sum_addr, sum_var);
            ir_printf(ctx, out, "store [t%d], t%d", sum_addr, zero);

            ir_printf(ctx, out, "store [t%d], t%d", k_addr, zero);
            k_label = new_label(ctx);
            k_end_label = new_label(ctx);
            ir_printf(ctx, out, "label L%d", k_label);
            {
                int k_val = new_temp(ctx);
                int k_cmp = new_temp(ctx);
                int i_val = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = lt t%d, t%d", k_cmp, k_val, i_val);
                ir_printf(ctx, out, "ifz t%d goto L%d", k_cmp, k_end_label);
            }
            {
                int i_val = new_temp(ctx);
                int col_val = new_temp(ctx);
                int k_val = new_temp(ctx);
                int addr_l;
                int addr_inv;
                int val_l;
                int val_inv;
                int mul_val;
                int sum_val;
                int sum_next;

                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                addr_l = emit_matrix_elem_addr(ctx, src, i_val, k_val, out);
                addr_inv = emit_matrix_elem_addr(ctx, dst, k_val, col_val, out);
                val_l = new_temp(ctx);
                val_inv = new_temp(ctx);
                mul_val = new_temp(ctx);
                sum_val = new_temp(ctx);
                sum_next = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load [t%d]", val_l, addr_l);
                ir_printf(ctx, out, "t%d = load [t%d]", val_inv, addr_inv);
                ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_l, val_inv);
                ir_printf(ctx, out, "t%d = load %s", sum_val, sum_var);
                ir_printf(ctx, out, "t%d = + t%d, t%d", sum_next, sum_val, mul_val);
                ir_printf(ctx, out, "store [t%d], t%d", sum_addr, sum_next);
            }
            {
                int k_val = new_temp(ctx);
                int k_next = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
                ir_printf(ctx, out, "t%d = + t%d, t%d", k_next, k_val, one);
                ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_next);
            }
            ir_printf(ctx, out, "goto L%d", k_label);
            ir_printf(ctx, out, "label L%d", k_end_label);

            {
                int i_val = new_temp(ctx);
                int col_val = new_temp(ctx);
                int addr_diag;
                int val_diag;
                int sum_val;
                int rhs;
                int diff;
                int val_out;
                int addr_out;
                int eq;
                int rhs_label;
                int rhs_end;

                ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
                ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
                addr_diag = emit_matrix_elem_addr(ctx, src, i_val, i_val, out);
                val_diag = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load [t%d]", val_diag, addr_diag);
                sum_val = new_temp(ctx);
                ir_printf(ctx, out, "t%d = load %s", sum_val, sum_var);

                rhs = new_temp(ctx);
                eq = new_temp(ctx);
                rhs_label = new_label(ctx);
                rhs_end = new_label(ctx);
                ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, i_val, col_val);
                ir_printf(ctx, out, "ifz t%d goto L%d", eq, rhs_label);
                ir_printf(ctx, out, "t%d = mov t%d", rhs, one);
                ir_printf(ctx, out, "goto L%d", rhs_end);
                ir_printf(ctx, out, "label L%d", rhs_label);
                ir_printf(ctx, out, "t%d = mov t%d", rhs, zero);
                ir_printf(ctx, out, "label L%d", rhs_end);

                diff = new_temp(ctx);
                val_out = new_temp(ctx);
                ir_printf(ctx, out, "t%d = - t%d, t%d", diff, rhs, sum_val);
                emit_safe_div(ctx, val_out, diff, val_diag, out);
                addr_out = emit_matrix_elem_addr(ctx, dst, i_val, col_val, out);
                ir_printf(ctx, out, "store [t%d], t%d", addr_out, val_out);
            }
        }
        {
            int i_val = new_temp(ctx);
            int i_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
        }
        ir_printf(ctx, out, "goto L%d", i_label);
        ir_printf(ctx, out, "label L%d", i_end_label);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    return 1;
}

static int matrix_props_from_init(IRContext *ctx, ASTNode *node, int rows, int cols,
                                  int *is_zero, int *is_identity, int *is_diagonal,
                                  int *is_symmetric, int *is_upper, int *is_lower)
{
    MatrixInitProps props;
    int row = 0;

    if (!node || rows <= 0 || cols <= 0) {
        return 0;
    }
    matrix_props_init(&props, rows, cols);
    matrix_props_visit_rows(ctx, node, &row, &props);
    if (!props.known || props.elems != rows * cols) {
        return 0;
    }
    if (is_zero) {
        *is_zero = props.is_zero;
    }
    if (is_identity) {
        *is_identity = props.is_identity;
    }
    if (is_diagonal) {
        *is_diagonal = props.is_diagonal;
    }
    if (is_upper) {
        *is_upper = props.is_upper_triangular;
    }
    if (is_lower) {
        *is_lower = props.is_lower_triangular;
    }
    if (is_symmetric) {
        *is_symmetric = props.is_symmetric;
        if (*is_symmetric && rows == cols) {
            size_t total = (size_t) rows * (size_t) cols;
            ConstVal *vals = calloc(total, sizeof(ConstVal));
            if (vals) {
                int ok = matrix_collect_init(ctx, node, rows, cols, vals);
                if (ok) {
                    for (int i = 0; i < rows && ok; i++) {
                        for (int j = i + 1; j < cols; j++) {
                            if (!const_val_equal(&vals[i * cols + j], &vals[j * cols + i])) {
                                ok = 0;
                                break;
                            }
                        }
                    }
                }
                if (!ok) {
                    *is_symmetric = 0;
                }
                free(vals);
            } else {
                *is_symmetric = 0;
            }
        } else {
            *is_symmetric = 0;
        }
    }
    return 1;
}

static int emit_matrix_zero_fill(IRContext *ctx, const char *dst, int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_val = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_dst;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, zero);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_identity_fill(IRContext *ctx, const char *dst, int n, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int n_const;
    int four;
    int cols_bytes;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || n <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, n_const, four);

    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_val = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_dst;
        int eq;
        int set_one_label;
        int end_label;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);

        eq = new_temp(ctx);
        set_one_label = new_label(ctx);
        end_label = new_label(ctx);
        ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, row_val, col_val);
        ir_printf(ctx, out, "ifz t%d goto L%d", eq, set_one_label);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, one);
        ir_printf(ctx, out, "goto L%d", end_label);
        ir_printf(ctx, out, "label L%d", set_one_label);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, zero);
        ir_printf(ctx, out, "label L%d", end_label);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_scalar_copy(IRContext *ctx, const char *dst, const char *src, FILE *out)
{
    int zero = emit_const(ctx, "int", "0", out);
    int addr_src = emit_matrix_elem_addr(ctx, src, zero, zero, out);
    int addr_dst = emit_matrix_elem_addr(ctx, dst, zero, zero, out);
    int val = new_temp(ctx);
    ir_printf(ctx, out, "t%d = load [t%d]", val, addr_src);
    ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val);
    return 1;
}

static int emit_matrix_scalar_binop(IRContext *ctx, const char *dst, const char *a, const char *b,
                                    const char *op, FILE *out)
{
    int zero = emit_const(ctx, "int", "0", out);
    int addr_a = emit_matrix_elem_addr(ctx, a, zero, zero, out);
    int addr_b = emit_matrix_elem_addr(ctx, b, zero, zero, out);
    int addr_dst = emit_matrix_elem_addr(ctx, dst, zero, zero, out);
    int val_a = new_temp(ctx);
    int val_b = new_temp(ctx);
    int val_out = new_temp(ctx);
    ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
    ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
    ir_printf(ctx, out, "t%d = %s t%d, t%d", val_out, op, val_a, val_b);
    ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_out);
    return 1;
}

static int emit_matrix_mul_inner1(IRContext *ctx, const char *dst, const char *a, const char *b,
                                  int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int base_a;
    int base_b;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !a || !b || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);

    base_a = new_temp(ctx);
    base_b = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_b, safe_str(b));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_a;
        int addr_b;
        int addr_dst;
        int val_a;
        int val_b;
        int val_out;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, four);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        {
            int row_off_dst = new_temp(ctx);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_dst, row_val, cols_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off_dst);
        }

        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        addr_a = new_temp(ctx);
        addr_b = new_temp(ctx);
        addr_dst = new_temp(ctx);

        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, zero);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_b, base_b, col_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);

        val_a = new_temp(ctx);
        val_b = new_temp(ctx);
        val_out = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
        ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
        ir_printf(ctx, out, "t%d = * t%d, t%d", val_out, val_a, val_b);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_out);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_mul_diag_left(IRContext *ctx, const char *dst, const char *diag, const char *a,
                                     int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int diag_cols_bytes;
    int base_diag;
    int base_a;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !diag || !a || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    diag_cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", diag_cols_bytes, rows_const, four);

    base_diag = new_temp(ctx);
    base_a = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_diag, safe_str(diag));
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int row_off_diag = new_temp(ctx);
        int row_base_diag = new_temp(ctx);
        int col_off_diag = new_temp(ctx);
        int addr_diag;
        int diag_val;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_diag, row_val, diag_cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_diag, base_diag, row_off_diag);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off_diag, row_val, four);
        addr_diag = new_temp(ctx);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_diag, row_base_diag, col_off_diag);
        diag_val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", diag_val, addr_diag);

        ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
        col_label = new_label(ctx);
        col_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", col_label);
        {
            int col_val = new_temp(ctx);
            int col_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
        }
        {
            int col_val = new_temp(ctx);
            int col_off = new_temp(ctx);
            int addr_a;
            int addr_dst;
            int val_a;
            int val_out;

            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);
            addr_a = new_temp(ctx);
            addr_dst = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, col_off);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
            val_a = new_temp(ctx);
            val_out = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_out, diag_val, val_a);
            ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_out);
        }
        {
            int col_val = new_temp(ctx);
            int col_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
        }
        ir_printf(ctx, out, "goto L%d", col_label);
        ir_printf(ctx, out, "label L%d", col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_mul_diag_right(IRContext *ctx, const char *dst, const char *a, const char *diag,
                                      int rows, int cols, FILE *out)
{
    int row_var_id;
    int col_var_id;
    char row_var[32];
    char col_var[32];
    int row_addr;
    int col_addr;
    int zero;
    int one;
    int rows_const;
    int cols_const;
    int four;
    int cols_bytes;
    int diag_cols_bytes;
    int base_diag;
    int base_a;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !diag || !a || rows <= 0 || cols <= 0) {
        return 0;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    rows_const = new_temp(ctx);
    cols_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", rows_const, rows);
    ir_printf(ctx, out, "t%d = const int %d", cols_const, cols);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    diag_cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, cols_const, four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", diag_cols_bytes, cols_const, four);

    base_diag = new_temp(ctx);
    base_a = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_diag, safe_str(diag));
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, rows_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int row_base_dst = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, cols_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);

        ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
        col_label = new_label(ctx);
        col_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", col_label);
        {
            int col_val = new_temp(ctx);
            int col_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, cols_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
        }
        {
            int col_val = new_temp(ctx);
            int col_off = new_temp(ctx);
            int diag_row_off = new_temp(ctx);
            int diag_row_base = new_temp(ctx);
            int addr_diag;
            int addr_a;
            int addr_dst;
            int val_diag;
            int val_a;
            int val_out;

            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);
            ir_printf(ctx, out, "t%d = * t%d, t%d", diag_row_off, col_val, diag_cols_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", diag_row_base, base_diag, diag_row_off);
            addr_diag = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_diag, diag_row_base, col_off);

            addr_a = new_temp(ctx);
            addr_dst = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, col_off);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
            val_diag = new_temp(ctx);
            val_a = new_temp(ctx);
            val_out = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_diag, addr_diag);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
            ir_printf(ctx, out, "t%d = * t%d, t%d", val_out, val_a, val_diag);
            ir_printf(ctx, out, "store [t%d], t%d", addr_dst, val_out);
        }
        {
            int col_val = new_temp(ctx);
            int col_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
        }
        ir_printf(ctx, out, "goto L%d", col_label);
        ir_printf(ctx, out, "label L%d", col_end_label);
    }

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static int emit_matrix_mul_sym_self(IRContext *ctx, const char *dst, const char *a, int n, FILE *out)
{
    int i_var_id;
    int j_var_id;
    int k_var_id;
    char i_var[32];
    char j_var[32];
    char k_var[32];
    int i_addr;
    int j_addr;
    int k_addr;
    int zero;
    int one;
    int zero_dec;
    int one_dec;
    int n_const;
    int four;
    int cols_bytes;
    int base_a;
    int base_dst;
    int i_label;
    int i_end_label;
    int j_label;
    int j_end_label;
    int k_label;
    int k_end_label;

    if (!dst || !a || n <= 0) {
        return 0;
    }

    i_var_id = new_temp(ctx);
    j_var_id = new_temp(ctx);
    k_var_id = new_temp(ctx);
    snprintf(i_var, sizeof(i_var), "mat_i%d", i_var_id);
    snprintf(j_var, sizeof(j_var), "mat_j%d", j_var_id);
    snprintf(k_var, sizeof(k_var), "mat_k%d", k_var_id);

    ir_printf(ctx, out, "decl int %s", i_var);
    ir_printf(ctx, out, "decl int %s", j_var);
    ir_printf(ctx, out, "decl int %s", k_var);
    i_addr = new_temp(ctx);
    j_addr = new_temp(ctx);
    k_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", i_addr, i_var);
    ir_printf(ctx, out, "t%d = addr %s", j_addr, j_var);
    ir_printf(ctx, out, "t%d = addr %s", k_addr, k_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    zero_dec = new_temp(ctx);
    one_dec = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const decimal 0.0", zero_dec);
    ir_printf(ctx, out, "t%d = const decimal 1.0", one_dec);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);
    four = new_temp(ctx);
    cols_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", cols_bytes, n_const, four);

    base_a = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
    i_label = new_label(ctx);
    i_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", i_label);
    {
        int i_val = new_temp(ctx);
        int i_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
    }
    {
        int i_val = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, i_val);
    }
    j_label = new_label(ctx);
    j_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", j_label);
    {
        int j_val = new_temp(ctx);
        int j_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
    }
    {
        int acc_var_id = new_temp(ctx);
        char acc_var[32];
        int acc_addr;
        snprintf(acc_var, sizeof(acc_var), "mat_acc%d", acc_var_id);
        ir_printf(ctx, out, "decl int %s", acc_var);
        acc_addr = new_temp(ctx);
        ir_printf(ctx, out, "t%d = addr %s", acc_addr, acc_var);
        ir_printf(ctx, out, "store [t%d], t%d", acc_addr, zero);

        ir_printf(ctx, out, "store [t%d], t%d", k_addr, zero);
        k_label = new_label(ctx);
        k_end_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", k_label);
        {
            int k_val = new_temp(ctx);
            int k_cmp = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
            ir_printf(ctx, out, "t%d = lt t%d, t%d", k_cmp, k_val, n_const);
            ir_printf(ctx, out, "ifz t%d goto L%d", k_cmp, k_end_label);
        }
        {
            int i_val = new_temp(ctx);
            int j_val = new_temp(ctx);
            int k_val = new_temp(ctx);
            int row_off_i = new_temp(ctx);
            int row_off_j = new_temp(ctx);
            int row_base_i = new_temp(ctx);
            int row_base_j = new_temp(ctx);
            int col_off = new_temp(ctx);
            int addr_i;
            int addr_j;
            int val_i;
            int val_j;
            int mul_val;
            int acc_val;
            int acc_next;

            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_i, i_val, cols_bytes);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_j, j_val, cols_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_i, base_a, row_off_i);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_j, base_a, row_off_j);
            ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, k_val, four);
            addr_i = new_temp(ctx);
            addr_j = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_i, row_base_i, col_off);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_j, row_base_j, col_off);
            val_i = new_temp(ctx);
            val_j = new_temp(ctx);
            mul_val = new_temp(ctx);
            acc_val = new_temp(ctx);
            acc_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_i, addr_i);
            ir_printf(ctx, out, "t%d = load [t%d]", val_j, addr_j);
            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_i, val_j);
            ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", acc_next, acc_val, mul_val);
            ir_printf(ctx, out, "store [t%d], t%d", acc_addr, acc_next);
        }
        {
            int k_val = new_temp(ctx);
            int k_next = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load %s", k_val, k_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", k_next, k_val, one);
            ir_printf(ctx, out, "store [t%d], t%d", k_addr, k_next);
        }
        ir_printf(ctx, out, "goto L%d", k_label);
        ir_printf(ctx, out, "label L%d", k_end_label);

        {
            int i_val = new_temp(ctx);
            int j_val = new_temp(ctx);
            int acc_val = new_temp(ctx);
            int row_off_i = new_temp(ctx);
            int row_off_j = new_temp(ctx);
            int row_base_dst_i = new_temp(ctx);
            int row_base_dst_j = new_temp(ctx);
            int col_off = new_temp(ctx);
            int addr_ij;
            int addr_ji;
            int eq;
            int skip_label;

            ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_i, i_val, cols_bytes);
            ir_printf(ctx, out, "t%d = * t%d, t%d", row_off_j, j_val, cols_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst_i, base_dst, row_off_i);
            ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst_j, base_dst, row_off_j);
            ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, j_val, four);
            addr_ij = new_temp(ctx);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_ij, row_base_dst_i, col_off);
            ir_printf(ctx, out, "store [t%d], t%d", addr_ij, acc_val);

            eq = new_temp(ctx);
            skip_label = new_label(ctx);
            {
                int after_label = new_label(ctx);
                ir_printf(ctx, out, "t%d = eq t%d, t%d", eq, i_val, j_val);
                ir_printf(ctx, out, "ifz t%d goto L%d", eq, skip_label);
                ir_printf(ctx, out, "goto L%d", after_label);
                ir_printf(ctx, out, "label L%d", skip_label);

                addr_ji = new_temp(ctx);
                ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, i_val, four);
                ir_printf(ctx, out, "t%d = + t%d, t%d", addr_ji, row_base_dst_j, col_off);
                ir_printf(ctx, out, "store [t%d], t%d", addr_ji, acc_val);
                ir_printf(ctx, out, "label L%d", after_label);
            }
        }
    }
    {
        int j_val = new_temp(ctx);
        int j_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
    }
    ir_printf(ctx, out, "goto L%d", j_label);
    ir_printf(ctx, out, "label L%d", j_end_label);

    {
        int i_val = new_temp(ctx);
        int i_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
    }
    ir_printf(ctx, out, "goto L%d", i_label);
    ir_printf(ctx, out, "label L%d", i_end_label);

    return 1;
}

static int emit_matrix_transpose_inplace(IRContext *ctx, const char *name, int n, FILE *out)
{
    int i_var_id;
    int j_var_id;
    char i_var[32];
    char j_var[32];
    int i_addr;
    int j_addr;
    int zero;
    int one;
    int n_const;
    int i_label;
    int i_end_label;
    int j_label;
    int j_end_label;

    if (!name || n <= 1) {
        return 0;
    }

    i_var_id = new_temp(ctx);
    j_var_id = new_temp(ctx);
    snprintf(i_var, sizeof(i_var), "mat_i%d", i_var_id);
    snprintf(j_var, sizeof(j_var), "mat_j%d", j_var_id);

    ir_printf(ctx, out, "decl int %s", i_var);
    ir_printf(ctx, out, "decl int %s", j_var);
    i_addr = new_temp(ctx);
    j_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", i_addr, i_var);
    ir_printf(ctx, out, "t%d = addr %s", j_addr, j_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);

    ir_printf(ctx, out, "store [t%d], t%d", i_addr, zero);
    i_label = new_label(ctx);
    i_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", i_label);
    {
        int i_val = new_temp(ctx);
        int i_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", i_cmp, i_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", i_cmp, i_end_label);
    }

    {
        int i_val = new_temp(ctx);
        int j_start = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_start, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_start);
    }
    j_label = new_label(ctx);
    j_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", j_label);
    {
        int j_val = new_temp(ctx);
        int j_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", j_cmp, j_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", j_cmp, j_end_label);
    }
    {
        int i_val = new_temp(ctx);
        int j_val = new_temp(ctx);
        int addr_ij;
        int addr_ji;
        int val_ij;
        int val_ji;

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        addr_ij = emit_matrix_elem_addr(ctx, name, i_val, j_val, out);
        addr_ji = emit_matrix_elem_addr(ctx, name, j_val, i_val, out);
        val_ij = new_temp(ctx);
        val_ji = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", val_ij, addr_ij);
        ir_printf(ctx, out, "t%d = load [t%d]", val_ji, addr_ji);
        ir_printf(ctx, out, "store [t%d], t%d", addr_ij, val_ji);
        ir_printf(ctx, out, "store [t%d], t%d", addr_ji, val_ij);
    }
    {
        int j_val = new_temp(ctx);
        int j_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", j_next, j_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", j_addr, j_next);
    }
    ir_printf(ctx, out, "goto L%d", j_label);
    ir_printf(ctx, out, "label L%d", j_end_label);

    {
        int i_val = new_temp(ctx);
        int i_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", i_next, i_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", i_addr, i_next);
    }
    ir_printf(ctx, out, "goto L%d", i_label);
    ir_printf(ctx, out, "label L%d", i_end_label);

    return 1;
}

static int emit_matrix_mul_unrolled(IRContext *ctx, const char *dst, const char *a, const char *b,
                                    int n, FILE *out)
{
    int row_var_id;
    int col_var_id;
    int acc_var_id;
    char row_var[32];
    char col_var[32];
    char acc_var[32];
    int row_addr;
    int col_addr;
    int acc_addr;
    int zero;
    int one;
    int n_const;
    int four;
    int n_bytes;
    int base_a;
    int base_b;
    int base_dst;
    int row_label;
    int row_end_label;
    int col_label;
    int col_end_label;

    if (!dst || !a || !b || n <= 0) {
        return 0;
    }

    if (n == 2) {
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                int acc = -1;
                for (int k = 0; k < n; k++) {
                    char rbuf[16];
                    char cbuf[16];
                    char kbuf[16];
                    int rtemp;
                    int ctemp;
                    int ktemp;
                    int addr_a;
                    int addr_b;
                    int val_a;
                    int val_b;
                    int mul_val;

                    snprintf(rbuf, sizeof(rbuf), "%d", r);
                    snprintf(cbuf, sizeof(cbuf), "%d", c);
                    snprintf(kbuf, sizeof(kbuf), "%d", k);
                    rtemp = emit_const(ctx, "int", rbuf, out);
                    ctemp = emit_const(ctx, "int", cbuf, out);
                    ktemp = emit_const(ctx, "int", kbuf, out);

                    addr_a = emit_matrix_elem_addr(ctx, a, rtemp, ktemp, out);
                    addr_b = emit_matrix_elem_addr(ctx, b, ktemp, ctemp, out);
                    val_a = new_temp(ctx);
                    val_b = new_temp(ctx);
                    mul_val = new_temp(ctx);
                    ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
                    ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
                    ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_a, val_b);

                    if (acc < 0) {
                        acc = mul_val;
                    } else {
                        int sum = new_temp(ctx);
                        ir_printf(ctx, out, "t%d = + t%d, t%d", sum, acc, mul_val);
                        acc = sum;
                    }
                }
                {
                    char rbuf[16];
                    char cbuf[16];
                    int rtemp;
                    int ctemp;
                    int addr_dst;
                    snprintf(rbuf, sizeof(rbuf), "%d", r);
                    snprintf(cbuf, sizeof(cbuf), "%d", c);
                    rtemp = emit_const(ctx, "int", rbuf, out);
                    ctemp = emit_const(ctx, "int", cbuf, out);
                    addr_dst = emit_matrix_elem_addr(ctx, dst, rtemp, ctemp, out);
                    ir_printf(ctx, out, "store [t%d], t%d", addr_dst, acc);
                }
            }
        }
        return 1;
    }

    row_var_id = new_temp(ctx);
    col_var_id = new_temp(ctx);
    acc_var_id = new_temp(ctx);
    snprintf(row_var, sizeof(row_var), "mat_r%d", row_var_id);
    snprintf(col_var, sizeof(col_var), "mat_c%d", col_var_id);
    snprintf(acc_var, sizeof(acc_var), "mat_acc%d", acc_var_id);

    ir_printf(ctx, out, "decl int %s", row_var);
    ir_printf(ctx, out, "decl int %s", col_var);
    ir_printf(ctx, out, "decl int %s", acc_var);

    row_addr = new_temp(ctx);
    col_addr = new_temp(ctx);
    acc_addr = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", row_addr, row_var);
    ir_printf(ctx, out, "t%d = addr %s", col_addr, col_var);
    ir_printf(ctx, out, "t%d = addr %s", acc_addr, acc_var);

    zero = new_temp(ctx);
    one = new_temp(ctx);
    n_const = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 0", zero);
    ir_printf(ctx, out, "t%d = const int 1", one);
    ir_printf(ctx, out, "t%d = const int %d", n_const, n);

    four = new_temp(ctx);
    n_bytes = new_temp(ctx);
    ir_printf(ctx, out, "t%d = const int 4", four);
    ir_printf(ctx, out, "t%d = * t%d, t%d", n_bytes, n_const, four);

    base_a = new_temp(ctx);
    base_b = new_temp(ctx);
    base_dst = new_temp(ctx);
    ir_printf(ctx, out, "t%d = addr %s", base_a, safe_str(a));
    ir_printf(ctx, out, "t%d = addr %s", base_b, safe_str(b));
    ir_printf(ctx, out, "t%d = addr %s", base_dst, safe_str(dst));

    ir_printf(ctx, out, "store [t%d], t%d", row_addr, zero);
    row_label = new_label(ctx);
    row_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", row_label);
    {
        int row_val = new_temp(ctx);
        int row_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", row_cmp, row_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", row_cmp, row_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", col_addr, zero);
    col_label = new_label(ctx);
    col_end_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", col_label);
    {
        int col_val = new_temp(ctx);
        int col_cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = lt t%d, t%d", col_cmp, col_val, n_const);
        ir_printf(ctx, out, "ifz t%d goto L%d", col_cmp, col_end_label);
    }

    ir_printf(ctx, out, "store [t%d], t%d", acc_addr, zero);
    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_a = new_temp(ctx);
        int col_off = new_temp(ctx);

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, n_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_a, base_a, row_off);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);

        for (int k = 0; k < n; k++) {
            char kbuf[16];
            int ktemp;
            int k_off;
            int addr_a;
            int b_row_off;
            int b_off;
            int addr_b;
            int val_a;
            int val_b;
            int mul_val;
            int acc_val;
            int acc_next;

            snprintf(kbuf, sizeof(kbuf), "%d", k);
            ktemp = emit_const(ctx, "int", kbuf, out);
            k_off = new_temp(ctx);
            addr_a = new_temp(ctx);
            b_row_off = new_temp(ctx);
            b_off = new_temp(ctx);
            addr_b = new_temp(ctx);

            ir_printf(ctx, out, "t%d = * t%d, t%d", k_off, ktemp, four);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_a, row_base_a, k_off);
            ir_printf(ctx, out, "t%d = * t%d, t%d", b_row_off, ktemp, n_bytes);
            ir_printf(ctx, out, "t%d = + t%d, t%d", b_off, b_row_off, col_off);
            ir_printf(ctx, out, "t%d = + t%d, t%d", addr_b, base_b, b_off);

            val_a = new_temp(ctx);
            val_b = new_temp(ctx);
            mul_val = new_temp(ctx);
            acc_val = new_temp(ctx);
            acc_next = new_temp(ctx);

            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
            ir_printf(ctx, out, "t%d = load [t%d]", val_b, addr_b);
            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_val, val_a, val_b);
            ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
            ir_printf(ctx, out, "t%d = + t%d, t%d", acc_next, acc_val, mul_val);
            ir_printf(ctx, out, "store [t%d], t%d", acc_addr, acc_next);
        }
    }

    {
        int row_val = new_temp(ctx);
        int col_val = new_temp(ctx);
        int acc_val = new_temp(ctx);
        int row_off = new_temp(ctx);
        int row_base_dst = new_temp(ctx);
        int col_off = new_temp(ctx);
        int addr_dst = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = * t%d, t%d", row_off, row_val, n_bytes);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_base_dst, base_dst, row_off);
        ir_printf(ctx, out, "t%d = * t%d, t%d", col_off, col_val, four);
        ir_printf(ctx, out, "t%d = + t%d, t%d", addr_dst, row_base_dst, col_off);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, acc_val);
    }

    {
        int col_val = new_temp(ctx);
        int col_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", col_next, col_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", col_addr, col_next);
    }
    ir_printf(ctx, out, "goto L%d", col_label);
    ir_printf(ctx, out, "label L%d", col_end_label);

    {
        int row_val = new_temp(ctx);
        int row_next = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = + t%d, t%d", row_next, row_val, one);
        ir_printf(ctx, out, "store [t%d], t%d", row_addr, row_next);
    }
    ir_printf(ctx, out, "goto L%d", row_label);
    ir_printf(ctx, out, "label L%d", row_end_label);

    return 1;
}

static void emit_matrix_init_rows(IRContext *ctx, const char *name, ASTNode *node, int *row, FILE *out)
{
    if (!node || !row) {
        return;
    }
    if (node->type && strcmp(node->type, "init_rows") == 0) {
        emit_matrix_init_rows(ctx, name, node->left, row, out);
        emit_matrix_init_rows(ctx, name, node->right, row, out);
        return;
    }
    if (node->type && strcmp(node->type, "init_row") == 0) {
        int col = 0;
        emit_matrix_init_list(ctx, name, node->left, *row, &col, out);
        (*row)++;
        return;
    }
}

static void emit_matrix_init_list(IRContext *ctx, const char *name, ASTNode *node, int row, int *col, FILE *out)
{
    if (!node || !col) {
        return;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        emit_matrix_init_list(ctx, name, node->left, row, col, out);
        if (node->right) {
            emit_matrix_init_list(ctx, name, node->right, row, col, out);
        }
        return;
    }
    {
        char rbuf[32];
        char cbuf[32];
        int rtemp;
        int ctemp;
        int addr;
        int val;
        snprintf(rbuf, sizeof(rbuf), "%d", row);
        snprintf(cbuf, sizeof(cbuf), "%d", *col);
        rtemp = emit_const(ctx, "int", rbuf, out);
        ctemp = emit_const(ctx, "int", cbuf, out);
        addr = emit_matrix_elem_addr(ctx, name, rtemp, ctemp, out);
        val = emit_expr(ctx, node, out);
        ir_printf(ctx, out, "store [t%d], t%d", addr, val);
        (*col)++;
    }
}

static void emit_matrix_init(IRContext *ctx, const char *name, ASTNode *node, FILE *out)
{
    int row = 0;

    if (!node) {
        return;
    }
    emit_matrix_init_rows(ctx, name, node, &row, out);
}

static void emit_array_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    ir_printf(ctx, out, "decl_array %s, %s", safe_str(node->value), safe_str(node->left ? node->left->value : NULL));
    if (node->third && node->value) {
        int index = 0;
        emit_array_init_list(ctx, node->value, node->third, &index, out);
        const_unbind(ctx, node->value);
        matrix_invalidate_name(ctx, node->value);
    }
}

static void emit_matrix_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    const char *rows = safe_str(node->left ? node->left->value : NULL);
    const char *cols = safe_str(node->right ? node->right->value : NULL);
    ir_printf(ctx, out, "decl_matrix %s, %s, %s", safe_str(node->value), rows, cols);
    if (node->third && node->value) {
        emit_matrix_init(ctx, node->value, node->third, out);
        {
            MatrixInfo *info = find_matrix_info(ctx, node->value);
            int is_zero = 0;
            int is_identity = 0;
            int is_diagonal = 0;
            int is_symmetric = 0;
            int is_upper = 0;
            int is_lower = 0;
            if (info && matrix_props_from_init(ctx, node->third, info->rows, info->cols,
                                               &is_zero, &is_identity, &is_diagonal,
                                               &is_symmetric, &is_upper, &is_lower)) {
                matrix_props_set(info, 1, is_zero, is_identity, is_diagonal,
                                 is_symmetric, is_upper, is_lower);
            } else if (info) {
                matrix_props_unknown(info);
            }
        }
        const_unbind(ctx, node->value);
        matrix_invalidate_name(ctx, node->value);
    }
}

static void emit_if_else_chain(IRContext *ctx, ASTNode *node, FILE *out, int break_label, int continue_label)
{
    if (!node) {
        return;
    }
    cse_clear(ctx);
    if (strcmp(node->type, "if_else") == 0) {
        int else_label = new_label(ctx);
        int end_label = new_label(ctx);
        ASTNode *left = node->left;
        if (left && strcmp(left->type, "if") == 0) {
            ConstVal folded;
            int cond_bool = 0;
            if (eval_const_expr(ctx, left->left, &folded) &&
                const_to_bool(&folded, &cond_bool)) {
                if (cond_bool) {
                    emit_stmt(ctx, left->right, out, break_label, continue_label);
                } else {
                    emit_if_else_chain(ctx, node->right, out, break_label, continue_label);
                }
                return;
            }
            {
                ConstBinding *orig = const_clone_list(ctx->consts);
                ConstBinding *then_consts = NULL;
                ConstBinding *else_consts = NULL;

                int cond = emit_expr(ctx, left->left, out);
                ir_printf(ctx, out, "ifz t%d goto L%d", cond, else_label);

                const_set(ctx, const_clone_list(orig));
                emit_stmt(ctx, left->right, out, break_label, continue_label);
                then_consts = ctx->consts;
                ctx->consts = NULL;

                ir_printf(ctx, out, "goto L%d", end_label);
                ir_printf(ctx, out, "label L%d", else_label);

                const_set(ctx, const_clone_list(orig));
                emit_if_else_chain(ctx, node->right, out, break_label, continue_label);
                else_consts = ctx->consts;
                ctx->consts = NULL;

                ir_printf(ctx, out, "label L%d", end_label);

                const_set(ctx, const_intersect(then_consts, else_consts));
                const_free_list(then_consts);
                const_free_list(else_consts);
                const_free_list(orig);
                return;
            }
        }
        if (left) {
            ConstVal folded;
            int cond_bool = 0;
            if (eval_const_expr(ctx, left, &folded) &&
                const_to_bool(&folded, &cond_bool)) {
                if (cond_bool) {
                    emit_stmt(ctx, node->right, out, break_label, continue_label);
                } else {
                    emit_stmt(ctx, node->third, out, break_label, continue_label);
                }
                return;
            }
            {
                ConstBinding *orig = const_clone_list(ctx->consts);
                ConstBinding *then_consts = NULL;
                ConstBinding *else_consts = NULL;

                int cond = emit_expr(ctx, left, out);
                ir_printf(ctx, out, "ifz t%d goto L%d", cond, else_label);

                const_set(ctx, const_clone_list(orig));
                emit_stmt(ctx, node->right, out, break_label, continue_label);
                then_consts = ctx->consts;
                ctx->consts = NULL;

                ir_printf(ctx, out, "goto L%d", end_label);
                ir_printf(ctx, out, "label L%d", else_label);

                const_set(ctx, const_clone_list(orig));
                emit_stmt(ctx, node->third, out, break_label, continue_label);
                else_consts = ctx->consts;
                ctx->consts = NULL;

                ir_printf(ctx, out, "label L%d", end_label);

                const_set(ctx, const_intersect(then_consts, else_consts));
                const_free_list(then_consts);
                const_free_list(else_consts);
                const_free_list(orig);
                return;
            }
        }
    }
    if (strcmp(node->type, "if") == 0) {
        ConstVal folded;
        int cond_bool = 0;
        if (eval_const_expr(ctx, node->left, &folded) &&
            const_to_bool(&folded, &cond_bool)) {
            if (cond_bool) {
                emit_stmt(ctx, node->right, out, break_label, continue_label);
            }
            return;
        }
        {
            int end_label = new_label(ctx);
            ConstBinding *orig = const_clone_list(ctx->consts);
            ConstBinding *then_consts = NULL;

            int cond = emit_expr(ctx, node->left, out);
            ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
            const_set(ctx, const_clone_list(orig));
            emit_stmt(ctx, node->right, out, break_label, continue_label);
            then_consts = ctx->consts;
            ctx->consts = NULL;
            ir_printf(ctx, out, "label L%d", end_label);

            const_set(ctx, const_intersect(then_consts, orig));
            const_free_list(then_consts);
            const_free_list(orig);
            return;
        }
    }
    emit_stmt(ctx, node, out, break_label, continue_label);
}

static int contains_break_or_continue(ASTNode *node)
{
    if (!node || !node->type) {
        return 0;
    }
    if (strcmp(node->type, "break") == 0 || strcmp(node->type, "continue") == 0) {
        return 1;
    }
    return contains_break_or_continue(node->left) ||
           contains_break_or_continue(node->right) ||
           contains_break_or_continue(node->third);
}

static int modifies_var(ASTNode *node, const char *name)
{
    if (!node || !node->type || !name) {
        return 0;
    }
    if (strcmp(node->type, "assign") == 0) {
        if (node->value && strcmp(node->value, name) == 0) {
            return 1;
        }
        if (!node->value && node->left &&
            strcmp(node->left->type, "id") == 0 &&
            node->left->value && strcmp(node->left->value, name) == 0) {
            return 1;
        }
    }
    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        if (node->left && strcmp(node->left->type, "id") == 0 &&
            node->left->value && strcmp(node->left->value, name) == 0) {
            return 1;
        }
    }
    if (strcmp(node->type, "scan") == 0) {
        if (node->left && strcmp(node->left->type, "id") == 0 &&
            node->left->value && strcmp(node->left->value, name) == 0) {
            return 1;
        }
    }
    return modifies_var(node->left, name) ||
           modifies_var(node->right, name) ||
           modifies_var(node->third, name);
}

static int extract_loop_init(IRContext *ctx, ASTNode *init, const char **var_name, long long *start)
{
    ConstVal val;

    if (!ctx || !init || !var_name || !start) {
        return 0;
    }
    if (strcmp(init->type, "assign") != 0) {
        return 0;
    }
    if (!init->left || strcmp(init->left->type, "id") != 0 || !init->left->value) {
        return 0;
    }
    if (!eval_const_expr(ctx, init->right, &val) || !const_to_int(&val, start)) {
        return 0;
    }
    *var_name = init->left->value;
    return 1;
}

static int extract_loop_condition(IRContext *ctx, ASTNode *cond, const char *var_name,
                                  long long *bound, int *inclusive, int *direction)
{
    ConstVal val;

    if (!ctx || !cond || !var_name || !bound || !inclusive || !direction) {
        return 0;
    }
    if (!(strcmp(cond->type, "lt") == 0 || strcmp(cond->type, "le") == 0 ||
          strcmp(cond->type, "gt") == 0 || strcmp(cond->type, "ge") == 0)) {
        return 0;
    }
    if (!cond->left || strcmp(cond->left->type, "id") != 0 || !cond->left->value) {
        return 0;
    }
    if (strcmp(cond->left->value, var_name) != 0) {
        return 0;
    }
    if (!eval_const_expr(ctx, cond->right, &val) || !const_to_int(&val, bound)) {
        return 0;
    }
    if (strcmp(cond->type, "lt") == 0 || strcmp(cond->type, "le") == 0) {
        *direction = 1;
        *inclusive = (strcmp(cond->type, "le") == 0);
        return 1;
    }
    *direction = -1;
    *inclusive = (strcmp(cond->type, "ge") == 0);
    return 1;
}

static int extract_loop_step(IRContext *ctx, ASTNode *update, const char *var_name, long long *step)
{
    ConstVal val;

    if (!ctx || !update || !var_name || !step) {
        return 0;
    }
    if ((strcmp(update->type, "pre_inc") == 0 || strcmp(update->type, "post_inc") == 0) &&
        update->left && strcmp(update->left->type, "id") == 0 &&
        update->left->value && strcmp(update->left->value, var_name) == 0) {
        *step = 1;
        return 1;
    }
    if ((strcmp(update->type, "pre_dec") == 0 || strcmp(update->type, "post_dec") == 0) &&
        update->left && strcmp(update->left->type, "id") == 0 &&
        update->left->value && strcmp(update->left->value, var_name) == 0) {
        *step = -1;
        return 1;
    }
    if (strcmp(update->type, "assign") == 0 &&
        update->left && strcmp(update->left->type, "id") == 0 &&
        update->left->value && strcmp(update->left->value, var_name) == 0 &&
        update->right && update->right->type) {
        ASTNode *rhs = update->right;
        if (strcmp(rhs->type, "+") == 0 || strcmp(rhs->type, "-") == 0) {
            ASTNode *lhs = rhs->left;
            ASTNode *r = rhs->right;
            if (lhs && r && lhs->type && r->type) {
                if (strcmp(lhs->type, "id") == 0 && lhs->value &&
                    strcmp(lhs->value, var_name) == 0 &&
                    eval_const_expr(ctx, r, &val) && const_to_int(&val, step)) {
                    if (strcmp(rhs->type, "-") == 0) {
                        *step = -*step;
                    }
                    return (*step != 0);
                }
                if (strcmp(rhs->type, "+") == 0 &&
                    strcmp(r->type, "id") == 0 && r->value &&
                    strcmp(r->value, var_name) == 0 &&
                    eval_const_expr(ctx, lhs, &val) && const_to_int(&val, step)) {
                    return (*step != 0);
                }
            }
        }
    }
    return 0;
}

static int compute_unroll_count(long long start, long long bound, long long step,
                                int inclusive, int direction, int *count)
{
    long long diff;
    long long step_abs;

    if (!count || step == 0) {
        return 0;
    }
    if (direction > 0) {
        if (step <= 0) {
            return 0;
        }
        if (inclusive) {
            if (start > bound) {
                *count = 0;
                return 1;
            }
            diff = bound - start;
            *count = (int) (diff / step + 1);
            return 1;
        }
        if (start >= bound) {
            *count = 0;
            return 1;
        }
        diff = bound - start;
        *count = (int) ((diff + step - 1) / step);
        return 1;
    }
    if (step >= 0) {
        return 0;
    }
    step_abs = -step;
    if (inclusive) {
        if (start < bound) {
            *count = 0;
            return 1;
        }
        diff = start - bound;
        *count = (int) (diff / step_abs + 1);
        return 1;
    }
    if (start <= bound) {
        *count = 0;
        return 1;
    }
    diff = start - bound;
    *count = (int) ((diff + step_abs - 1) / step_abs);
    return 1;
}

static int emit_for_body_unrolled(IRContext *ctx, ASTNode *node, FILE *out)
{
    const char *var_name = NULL;
    long long start = 0;
    long long bound = 0;
    long long step = 0;
    int inclusive = 0;
    int direction = 0;
    int count = 0;

    if (!ctx || !node || !out || !node->left) {
        return 0;
    }
    ASTNode *for_node = node->left;
    ASTNode *body = node->right;
    if (!for_node || strcmp(for_node->type, "for") != 0) {
        return 0;
    }
    if (!extract_loop_init(ctx, for_node->left, &var_name, &start)) {
        return 0;
    }
    if (!extract_loop_condition(ctx, for_node->right, var_name, &bound, &inclusive, &direction)) {
        return 0;
    }
    if (!extract_loop_step(ctx, for_node->third, var_name, &step)) {
        return 0;
    }
    if (!compute_unroll_count(start, bound, step, inclusive, direction, &count)) {
        return 0;
    }
    if (contains_break_or_continue(body)) {
        return 0;
    }
    if (modifies_var(body, var_name)) {
        return 0;
    }
    if (count > 8) {
        return 0;
    }

    emit_stmt(ctx, for_node->left, out, -1, -1);
    for (int i = 0; i < count; i++) {
        emit_stmt(ctx, body, out, -1, -1);
        emit_stmt(ctx, for_node->third, out, -1, -1);
    }
    const_clear(ctx);
    return 1;
}

static void emit_for_body(IRContext *ctx, ASTNode *node, FILE *out)
{
    ASTNode *for_node = node->left;
    ASTNode *body = node->right;
    int start_label = new_label(ctx);
    int end_label = new_label(ctx);
    int continue_label = new_label(ctx);
    int cond_is_const = 0;
    int cond_bool = 0;
    ConstVal folded;

    cse_clear(ctx);
    if (emit_for_body_unrolled(ctx, node, out)) {
        return;
    }

    emit_stmt(ctx, for_node->left, out, end_label, continue_label);
    if (for_node->right && for_node->right->type &&
        (strcmp(for_node->right->type, "int") == 0 ||
         strcmp(for_node->right->type, "decimal") == 0 ||
         strcmp(for_node->right->type, "char") == 0 ||
         strcmp(for_node->right->type, "string") == 0)) {
        if (eval_const_expr(ctx, for_node->right, &folded) &&
            const_to_bool(&folded, &cond_bool)) {
            cond_is_const = 1;
            if (!cond_bool) {
                return;
            }
        }
    }

    ir_printf(ctx, out, "label L%d", start_label);
    if (for_node->right && !(cond_is_const && cond_bool)) {
        int cond = emit_expr(ctx, for_node->right, out);
        ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
    }
    emit_stmt(ctx, body, out, end_label, continue_label);
    ir_printf(ctx, out, "label L%d", continue_label);
    emit_stmt(ctx, for_node->third, out, end_label, continue_label);
    ir_printf(ctx, out, "goto L%d", start_label);
    ir_printf(ctx, out, "label L%d", end_label);
    const_clear(ctx);
}

static void emit_while(IRContext *ctx, ASTNode *node, FILE *out)
{
    int start_label = new_label(ctx);
    int end_label = new_label(ctx);
    int continue_label = new_label(ctx);
    int cond_is_const = 0;
    int cond_bool = 0;
    ConstVal folded;

    cse_clear(ctx);
    /* Only fold while-conditions that are literal constants.
       Folding on variables can create incorrect infinite loops. */
    if (node->left && node->left->type &&
        (strcmp(node->left->type, "int") == 0 ||
         strcmp(node->left->type, "decimal") == 0 ||
         strcmp(node->left->type, "char") == 0 ||
         strcmp(node->left->type, "string") == 0)) {
        if (eval_const_expr(ctx, node->left, &folded) &&
            const_to_bool(&folded, &cond_bool)) {
            cond_is_const = 1;
            if (!cond_bool) {
                return;
            }
        }
    }
    ir_printf(ctx, out, "label L%d", start_label);
    if (!cond_is_const) {
        int cond = emit_expr(ctx, node->left, out);
        ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
    }
    emit_stmt(ctx, node->right, out, end_label, continue_label);
    ir_printf(ctx, out, "label L%d", continue_label);
    ir_printf(ctx, out, "goto L%d", start_label);
    ir_printf(ctx, out, "label L%d", end_label);
    const_clear(ctx);
}

static void emit_switch_cases(IRContext *ctx, ASTNode *node, FILE *out, int switch_temp, int end_label, ConstBinding *orig_consts)
{
    if (!node) {
        return;
    }
    if (strcmp(node->type, "cases") == 0) {
        emit_switch_cases(ctx, node->left, out, switch_temp, end_label, orig_consts);
        emit_switch_cases(ctx, node->right, out, switch_temp, end_label, orig_consts);
        return;
    }
    if (strcmp(node->type, "case") == 0) {
        int case_label = new_label(ctx);
        int next_label = new_label(ctx);
        int val = emit_expr(ctx, node->left, out);
        int cmp = new_temp(ctx);
        ir_printf(ctx, out, "t%d = eq t%d, t%d", cmp, switch_temp, val);
        ir_printf(ctx, out, "ifz t%d goto L%d", cmp, next_label);
        ir_printf(ctx, out, "label L%d", case_label);
        const_set(ctx, const_clone_list(orig_consts));
        emit_stmt(ctx, node->right, out, end_label, -1);
        const_set(ctx, NULL);
        ir_printf(ctx, out, "goto L%d", end_label);
        ir_printf(ctx, out, "label L%d", next_label);
        return;
    }
    if (strcmp(node->type, "default") == 0) {
        int def_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", def_label);
        const_set(ctx, const_clone_list(orig_consts));
        emit_stmt(ctx, node->left, out, end_label, -1);
        const_set(ctx, NULL);
        return;
    }
}

static ASTNode *find_const_case(IRContext *ctx, ASTNode *node, const ConstVal *switch_val, ASTNode **default_node)
{
    ConstVal case_val;

    if (!node) {
        return NULL;
    }
    if (strcmp(node->type, "cases") == 0) {
        ASTNode *found = find_const_case(ctx, node->left, switch_val, default_node);
        if (found) {
            return found;
        }
        return find_const_case(ctx, node->right, switch_val, default_node);
    }
    if (strcmp(node->type, "case") == 0) {
        if (eval_const_expr(ctx, node->left, &case_val) &&
            const_compare_equal(&case_val, switch_val)) {
            return node->right;
        }
        return NULL;
    }
    if (strcmp(node->type, "default") == 0) {
        if (default_node) {
            *default_node = node->left;
        }
        return NULL;
    }
    return NULL;
}

static void emit_switch(IRContext *ctx, ASTNode *node, FILE *out)
{
    int end_label;
    int switch_temp;
    ConstVal folded;
    ASTNode *default_node = NULL;
    ASTNode *case_node = NULL;

    if (!node) {
        return;
    }

    cse_clear(ctx);
    if (eval_const_expr(ctx, node->left, &folded)) {
        case_node = find_const_case(ctx, node->right, &folded, &default_node);
        if (case_node) {
            emit_stmt(ctx, case_node, out, -1, -1);
            return;
        }
        if (default_node) {
            emit_stmt(ctx, default_node, out, -1, -1);
            return;
        }
        return;
    }

    end_label = new_label(ctx);
    switch_temp = emit_expr(ctx, node->left, out);
    {
        ConstBinding *orig = const_clone_list(ctx->consts);
        emit_switch_cases(ctx, node->right, out, switch_temp, end_label, orig);
        const_free_list(orig);
    }
    ir_printf(ctx, out, "label L%d", end_label);
    const_clear(ctx);
}

static void emit_print_args(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    if (strcmp(node->type, "print_args") == 0) {
        emit_print_args(ctx, node->left, out);
        emit_print_args(ctx, node->right, out);
        return;
    }
    if (strcmp(node->type, "string") == 0) {
        ir_printf(ctx, out, "print_str %s", safe_str(node->value));
        return;
    }
    int v = emit_expr(ctx, node, out);
    ir_printf(ctx, out, "print t%d", v);
}

static void emit_stmt(IRContext *ctx, ASTNode *node, FILE *out, int break_label, int continue_label)
{
    if (!node) {
        return;
    }
    if (strcmp(node->type, "statements") == 0) {
        emit_stmt(ctx, node->left, out, break_label, continue_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "block") == 0) {
        emit_stmt(ctx, node->left, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "main_block") == 0) {
        emit_stmt(ctx, node->left, out, break_label, continue_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "program") == 0) {
        emit_stmt(ctx, node->left, out, break_label, continue_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "global") == 0) {
        emit_stmt(ctx, node->left, out, break_label, continue_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "decl") == 0) {
        emit_decl(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "decl_assign") == 0) {
        emit_decl_assign(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "array_decl") == 0) {
        emit_array_decl(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "matrix_decl") == 0) {
        emit_matrix_decl(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "assign") == 0) {
        emit_assignment(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        emit_expr(ctx, node, out);
        if (node->left && node->left->value) {
            matrix_invalidate_name(ctx, node->left->value);
        }
        return;
    }
    if (strcmp(node->type, "if") == 0) {
        emit_if_else_chain(ctx, node, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "if_else") == 0) {
        emit_if_else_chain(ctx, node, out, break_label, continue_label);
        return;
    }
    if (strcmp(node->type, "for_body") == 0) {
        emit_for_body(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "while") == 0) {
        emit_while(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "switch") == 0) {
        emit_switch(ctx, node, out);
        return;
    }
    if (strcmp(node->type, "break") == 0) {
        if (break_label >= 0) {
            ir_printf(ctx, out, "goto L%d", break_label);
        }
        return;
    }
    if (strcmp(node->type, "continue") == 0) {
        if (continue_label >= 0) {
            ir_printf(ctx, out, "goto L%d", continue_label);
        }
        return;
    }
    if (strcmp(node->type, "return") == 0) {
        if (node->left) {
            int v = emit_expr(ctx, node->left, out);
            ir_printf(ctx, out, "ret t%d", v);
        } else {
            ir_printf(ctx, out, "ret");
        }
        return;
    }
    if (strcmp(node->type, "print") == 0) {
        emit_print_args(ctx, node->left, out);
        return;
    }
    if (strcmp(node->type, "scan") == 0) {
        int addr = emit_lvalue_addr(ctx, node->left, out);
        ir_printf(ctx, out, "scan [t%d]", addr);
        if (node->left && strcmp(node->left->type, "id") == 0) {
            const_unbind(ctx, node->left->value);
            matrix_invalidate_name(ctx, node->left->value);
        } else if (node->left && node->left->value) {
            matrix_invalidate_name(ctx, node->left->value);
        }
        return;
    }
    if (strcmp(node->type, "empty") == 0) {
        return;
    }

    emit_expr(ctx, node, out);
}

static void ir_const_clear(IRConst *val)
{
    if (!val) {
        return;
    }
    free(val->s);
    val->s = NULL;
    val->kind = CONST_NONE;
    val->i = 0;
    val->d = 0.0;
}

static int ir_const_from_tokens(const char *kind, const char *value, IRConst *out)
{
    long long i = 0;
    double d = 0.0;
    int ch = 0;

    if (!kind || !value || !out) {
        return 0;
    }
    ir_const_clear(out);
    if (strcmp(kind, "int") == 0) {
        int tmp = 0;
        if (!parse_int_value(value, &tmp)) {
            return 0;
        }
        i = tmp;
        out->kind = CONST_INT;
        out->i = i;
    } else if (strcmp(kind, "decimal") == 0) {
        if (!parse_double_value(value, &d)) {
            return 0;
        }
        out->kind = CONST_DECIMAL;
        out->d = d;
    } else if (strcmp(kind, "char") == 0) {
        if (!parse_char_value(value, &ch)) {
            return 0;
        }
        out->kind = CONST_CHAR;
        out->i = ch;
    } else if (strcmp(kind, "string") == 0) {
        out->kind = CONST_STRING;
    } else {
        return 0;
    }
    out->s = strdup(value);
    return out->s != NULL;
}

static char *ir_const_to_string(const IRConst *val)
{
    if (!val) {
        return NULL;
    }
    if (val->s) {
        return strdup(val->s);
    }
    if (val->kind == CONST_INT) {
        return str_printf("%lld", val->i);
    }
    if (val->kind == CONST_DECIMAL) {
        return str_printf("%.15g", val->d);
    }
    return NULL;
}

static void name_map_free(NameInfo *head)
{
    while (head) {
        NameInfo *next = head->next;
        free(head->name);
        free(head->copy_name);
        ir_const_clear(&head->val);
        free(head);
        head = next;
    }
}

static NameInfo *name_map_find(NameInfo *head, const char *name)
{
    while (head) {
        if (strcmp(head->name, name) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

static NameInfo *name_map_get(NameInfo **head, const char *name)
{
    NameInfo *node;

    if (!head || !name) {
        return NULL;
    }
    node = name_map_find(*head, name);
    if (node) {
        return node;
    }
    node = calloc(1, sizeof(NameInfo));
    if (!node) {
        return NULL;
    }
    node->name = strdup(name);
    node->next = *head;
    *head = node;
    return node;
}

static void name_map_clear(NameInfo **head, const char *name)
{
    NameInfo *node;

    if (!head || !name) {
        return;
    }
    node = name_map_find(*head, name);
    if (!node) {
        return;
    }
    node->has_const = 0;
    node->has_copy = 0;
    node->has_last_store = 0;
    node->last_store_temp = 0;
    ir_const_clear(&node->val);
    free(node->copy_name);
    node->copy_name = NULL;
}

static void name_map_set_const(NameInfo **head, const char *name, const IRConst *val)
{
    NameInfo *node;

    if (!head || !name || !val) {
        return;
    }
    node = name_map_get(head, name);
    if (!node) {
        return;
    }
    name_map_clear(head, name);
    node->has_const = 1;
    node->val.kind = val->kind;
    node->val.i = val->i;
    node->val.d = val->d;
    node->val.s = val->s ? strdup(val->s) : NULL;
    node->has_last_store = 0;
    node->last_store_temp = 0;
}

static void name_map_set_copy(NameInfo **head, const char *name, const char *src)
{
    NameInfo *node;

    if (!head || !name || !src) {
        return;
    }
    node = name_map_get(head, name);
    if (!node) {
        return;
    }
    name_map_clear(head, name);
    node->has_copy = 1;
    node->copy_name = strdup(src);
    node->has_last_store = 0;
    node->last_store_temp = 0;
}

static void name_map_set_last_store(NameInfo **head, const char *name, int temp)
{
    NameInfo *node;

    if (!head || !name) {
        return;
    }
    node = name_map_get(head, name);
    if (!node) {
        return;
    }
    node->has_last_store = 1;
    node->last_store_temp = temp;
}

static void name_map_clear_all_last_store(NameInfo *head)
{
    while (head) {
        head->has_last_store = 0;
        head->last_store_temp = 0;
        head = head->next;
    }
}

static void temp_info_clear(TempInfo *info)
{
    if (!info) {
        return;
    }
    info->has_const = 0;
    info->has_copy = 0;
    info->copy_temp = 0;
    ir_const_clear(&info->val);
    free(info->name_src);
    info->name_src = NULL;
}

static int parse_label_line(const char *line, int *label)
{
    return line && label && sscanf(line, "label L%d", label) == 1;
}

static int parse_goto_line(const char *line, int *label)
{
    return line && label && sscanf(line, "goto L%d", label) == 1;
}

static int parse_ifz_line(const char *line, int *temp, int *label)
{
    return line && temp && label && sscanf(line, "ifz t%d goto L%d", temp, label) == 2;
}

static int parse_temp_def(const char *line, int *temp)
{
    return line && temp && sscanf(line, "t%d =", temp) == 1;
}

static int parse_const_def(const char *line, int *temp, char *kind, size_t kind_size, char *value, size_t value_size)
{
    if (!line || !temp || !kind || !value) {
        return 0;
    }
    return sscanf(line, "t%d = const %31s %127s", temp, kind, value) == 3;
}

static int parse_mov_def(const char *line, int *temp, int *src)
{
    return line && temp && src && sscanf(line, "t%d = mov t%d", temp, src) == 2;
}

static int parse_addr_def(const char *line, int *temp, char *name, size_t name_size)
{
    if (!line || !temp || !name) {
        return 0;
    }
    return sscanf(line, "t%d = addr %127s", temp, name) == 2;
}

static int parse_load_name(const char *line, int *temp, char *name, size_t name_size)
{
    if (!line || !temp || !name) {
        return 0;
    }
    if (sscanf(line, "t%d = load [%*[^]]]", temp) == 1) {
        return 0;
    }
    return sscanf(line, "t%d = load %127s", temp, name) == 2;
}

static int parse_load_addr(const char *line, int *temp, int *addr)
{
    return line && temp && addr && sscanf(line, "t%d = load [t%d]", temp, addr) == 2;
}

static int parse_store_line(const char *line, int *addr, int *val)
{
    return line && addr && val && sscanf(line, "store [t%d], t%d", addr, val) == 2;
}

static int parse_scan_line(const char *line, int *addr)
{
    return line && addr && sscanf(line, "scan [t%d]", addr) == 1;
}

static int parse_print_line(const char *line, int *temp)
{
    return line && temp && sscanf(line, "print t%d", temp) == 1;
}

static int parse_binary_op(const char *line, int *dst, char *op, size_t op_size, int *lhs, int *rhs)
{
    if (!line || !dst || !op || !lhs || !rhs) {
        return 0;
    }
    return sscanf(line, "t%d = %31s t%d, t%d", dst, op, lhs, rhs) == 4;
}

static int is_unary_ir_op(const char *op)
{
    if (!op) {
        return 0;
    }
    return strcmp(op, "uminus") == 0 ||
           strcmp(op, "not") == 0 ||
           strcmp(op, "size") == 0 ||
           strcmp(op, "sort") == 0 ||
           strcmp(op, "transpose") == 0 ||
           strcmp(op, "det") == 0 ||
           strcmp(op, "inv") == 0 ||
           strcmp(op, "shape") == 0;
}

static int parse_unary_op(const char *line, int *dst, char *op, size_t op_size, int *arg)
{
    if (!line || !dst || !op || !arg) {
        return 0;
    }
    {
        int n = 0;
        if (sscanf(line, "t%d = %31s t%d %n", dst, op, arg, &n) != 3) {
            return 0;
        }
        if (!is_unary_ir_op(op)) {
            return 0;
        }
        for (const char *p = line + n; *p; p++) {
            if (!isspace((unsigned char) *p)) {
                return 0;
            }
        }
    }
    return 1;
}

static int temp_resolve(TempInfo *temps, int max_temp, int temp)
{
    int t = temp;
    int guard = 0;

    if (!temps || t <= 0 || t > max_temp) {
        return temp;
    }
    while (temps[t].has_copy && guard < max_temp) {
        t = temps[t].copy_temp;
        guard++;
        if (t <= 0 || t > max_temp) {
            return temp;
        }
    }
    return t;
}

static int is_identity_op(const char *op, int *is_add, int *is_sub, int *is_mul, int *is_div)
{
    if (!op) {
        return 0;
    }
    *is_add = (strcmp(op, "+") == 0 || strcmp(op, "add") == 0);
    *is_sub = (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0);
    *is_mul = (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0);
    *is_div = (strcmp(op, "/") == 0);
    return *is_add || *is_sub || *is_mul || *is_div;
}

static int is_foldable_op(const char *op)
{
    if (!op) {
        return 0;
    }
    return strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 ||
           strcmp(op, "/") == 0 || strcmp(op, "%") == 0 ||
           strcmp(op, "add") == 0 || strcmp(op, "sub") == 0 || strcmp(op, "mul") == 0 ||
           strcmp(op, "eq") == 0 || strcmp(op, "ne") == 0 ||
           strcmp(op, "lt") == 0 || strcmp(op, "gt") == 0 ||
           strcmp(op, "le") == 0 || strcmp(op, "ge") == 0 ||
           strcmp(op, "and") == 0 || strcmp(op, "or") == 0;
}

static int fold_int_binop(const char *op, long long a, long long b, long long *out)
{
    if (!op || !out) {
        return 0;
    }
    if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) {
        *out = a + b;
        return 1;
    }
    if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) {
        *out = a - b;
        return 1;
    }
    if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) {
        *out = a * b;
        return 1;
    }
    if (strcmp(op, "/") == 0) {
        if (b == 0) {
            return 0;
        }
        *out = a / b;
        return 1;
    }
    if (strcmp(op, "%") == 0) {
        if (b == 0) {
            return 0;
        }
        *out = a % b;
        return 1;
    }
    if (strcmp(op, "eq") == 0) {
        *out = (a == b);
        return 1;
    }
    if (strcmp(op, "ne") == 0) {
        *out = (a != b);
        return 1;
    }
    if (strcmp(op, "lt") == 0) {
        *out = (a < b);
        return 1;
    }
    if (strcmp(op, "gt") == 0) {
        *out = (a > b);
        return 1;
    }
    if (strcmp(op, "le") == 0) {
        *out = (a <= b);
        return 1;
    }
    if (strcmp(op, "ge") == 0) {
        *out = (a >= b);
        return 1;
    }
    if (strcmp(op, "and") == 0) {
        *out = (a != 0 && b != 0);
        return 1;
    }
    if (strcmp(op, "or") == 0) {
        *out = (a != 0 || b != 0);
        return 1;
    }
    return 0;
}

static int build_label_index(char **lines, int line_count, int **out_labels, int **out_indices, int *out_count)
{
    int *labels = NULL;
    int *indices = NULL;
    int count = 0;
    int cap = 0;

    if (!lines || !out_labels || !out_indices || !out_count) {
        return 0;
    }
    for (int i = 0; i < line_count; i++) {
        int label = 0;
        if (parse_label_line(lines[i], &label)) {
            if (count == cap) {
                int new_cap = cap == 0 ? 64 : cap * 2;
                int *new_labels = realloc(labels, (size_t) new_cap * sizeof(int));
                int *new_indices = realloc(indices, (size_t) new_cap * sizeof(int));
                if (!new_labels || !new_indices) {
                    free(new_labels);
                    free(new_indices);
                    free(labels);
                    free(indices);
                    return 0;
                }
                labels = new_labels;
                indices = new_indices;
                cap = new_cap;
            }
            labels[count] = label;
            indices[count] = i;
            count++;
        }
    }
    *out_labels = labels;
    *out_indices = indices;
    *out_count = count;
    return 1;
}

static int label_index_find(int *labels, int *indices, int count, int label)
{
    for (int i = 0; i < count; i++) {
        if (labels[i] == label) {
            return indices[i];
        }
    }
    return -1;
}

static void apply_jump_threading(IRContext *ctx)
{
    int *labels = NULL;
    int *indices = NULL;
    int label_count = 0;

    if (!ctx || !ctx->lines) {
        return;
    }
    if (!build_label_index(ctx->lines, ctx->line_count, &labels, &indices, &label_count)) {
        return;
    }
    for (int i = 0; i < ctx->line_count; i++) {
        int label = 0;
        if (!parse_goto_line(ctx->lines[i], &label)) {
            continue;
        }
        int next_label = label;
        int guard = 0;
        while (guard < label_count) {
            int idx = label_index_find(labels, indices, label_count, next_label);
            if (idx < 0 || idx + 1 >= ctx->line_count) {
                break;
            }
            {
                int forwarded = 0;
                if (parse_goto_line(ctx->lines[idx + 1], &forwarded)) {
                    next_label = forwarded;
                    guard++;
                    continue;
                }
            }
            break;
        }
        if (next_label != label) {
            char *new_line = str_printf("goto L%d", next_label);
            if (new_line) {
                free(ctx->lines[i]);
                ctx->lines[i] = new_line;
            }
        }
    }
    free(labels);
    free(indices);
}

static void apply_jump_simplify(IRContext *ctx)
{
    if (!ctx || !ctx->lines) {
        return;
    }
    for (int i = 0; i + 1 < ctx->line_count; i++) {
        int label = 0;
        if (!parse_goto_line(ctx->lines[i], &label)) {
            continue;
        }
        {
            int next_label = 0;
            if (parse_label_line(ctx->lines[i + 1], &next_label) && next_label == label) {
                free(ctx->lines[i]);
                ctx->lines[i] = NULL;
            }
        }
    }
    {
        int out = 0;
        for (int i = 0; i < ctx->line_count; i++) {
            if (ctx->lines[i]) {
                ctx->lines[out++] = ctx->lines[i];
            }
        }
        ctx->line_count = out;
    }
}

static void apply_unreachable_elim(IRContext *ctx)
{
    int skip = 0;

    if (!ctx || !ctx->lines) {
        return;
    }
    for (int i = 0; i < ctx->line_count; i++) {
        int label = 0;
        if (parse_label_line(ctx->lines[i], &label)) {
            skip = 0;
            continue;
        }
        if (skip) {
            if (strcmp(ctx->lines[i], "IR_BEGIN") == 0 || strcmp(ctx->lines[i], "IR_END") == 0) {
                skip = 0;
            } else {
                free(ctx->lines[i]);
                ctx->lines[i] = NULL;
            }
            continue;
        }
        if (parse_goto_line(ctx->lines[i], &label)) {
            skip = 1;
        }
    }
    {
        int out = 0;
        for (int i = 0; i < ctx->line_count; i++) {
            if (ctx->lines[i]) {
                ctx->lines[out++] = ctx->lines[i];
            }
        }
        ctx->line_count = out;
    }
}

static int find_max_temp(char **lines, int line_count)
{
    int max_temp = 0;
    for (int i = 0; i < line_count; i++) {
        int t = 0;
        const char *line = lines[i];
        while (line && *line) {
            if (*line == 't' && isdigit((unsigned char) line[1])) {
                int val = 0;
                int j = 1;
                while (line[j] && isdigit((unsigned char) line[j])) {
                    val = val * 10 + (line[j] - '0');
                    j++;
                }
                if (val > max_temp) {
                    max_temp = val;
                }
                line += j;
                continue;
            }
            line++;
        }
    }
    return max_temp;
}

static char **build_addr_name_map(char **lines, int line_count, int max_temp)
{
    char **map = calloc((size_t) max_temp + 1, sizeof(char *));
    if (!map) {
        return NULL;
    }
    for (int i = 0; i < line_count; i++) {
        int temp = 0;
        char name[128];
        if (parse_addr_def(lines[i], &temp, name, sizeof(name))) {
            if (temp > 0 && temp <= max_temp && !map[temp]) {
                map[temp] = strdup(name);
            }
        }
    }
    for (int i = 0; i < line_count; i++) {
        int dst = 0;
        int src = 0;
        int lhs = 0;
        int rhs = 0;
        char op[32];
        if (parse_mov_def(lines[i], &dst, &src)) {
            if (dst > 0 && dst <= max_temp && src > 0 && src <= max_temp &&
                map[src] && !map[dst]) {
                map[dst] = strdup(map[src]);
            }
            continue;
        }
        if (parse_binary_op(lines[i], &dst, op, sizeof(op), &lhs, &rhs)) {
            if (dst <= 0 || dst > max_temp) {
                continue;
            }
            if (!(strcmp(op, "add") == 0 || strcmp(op, "+") == 0)) {
                continue;
            }
            if (lhs > 0 && lhs <= max_temp && rhs > 0 && rhs <= max_temp) {
                if (map[lhs] && !map[rhs] && !map[dst]) {
                    map[dst] = strdup(map[lhs]);
                } else if (map[rhs] && !map[lhs] && !map[dst]) {
                    map[dst] = strdup(map[rhs]);
                }
            }
        }
    }
    return map;
}

static void free_addr_name_map(char **map, int max_temp)
{
    if (!map) {
        return;
    }
    for (int i = 0; i <= max_temp; i++) {
        free(map[i]);
    }
    free(map);
}

static void apply_propagation(IRContext *ctx)
{
    int max_temp = 0;
    TempInfo *temps = NULL;
    NameInfo *names = NULL;
    char **addr_map = NULL;
    char **const_key = NULL;
    int *const_temp = NULL;
    int const_count = 0;
    int const_cap = 0;

    if (!ctx || !ctx->lines) {
        return;
    }
    max_temp = find_max_temp(ctx->lines, ctx->line_count);
    if (max_temp <= 0) {
        return;
    }
    temps = calloc((size_t) max_temp + 1, sizeof(TempInfo));
    if (!temps) {
        return;
    }
    addr_map = build_addr_name_map(ctx->lines, ctx->line_count, max_temp);
    for (int i = 0; i < ctx->line_count; i++) {
        int label = 0;
        int def = 0;
        int src = 0;
        int addr = 0;
        int val = 0;
        int lhs = 0;
        int rhs = 0;
        int arg = 0;
        char kind[32];
        char value[128];
        char op[32];
        char name[128];

        if (parse_label_line(ctx->lines[i], &label)) {
            for (int t = 0; t <= max_temp; t++) {
                temp_info_clear(&temps[t]);
            }
            name_map_free(names);
            names = NULL;
            for (int c = 0; c < const_count; c++) {
                free(const_key[c]);
            }
            const_count = 0;
            continue;
        }

        if (parse_const_def(ctx->lines[i], &def, kind, sizeof(kind), value, sizeof(value))) {
            char *key = str_printf("%s:%s", kind, value);
            if (key) {
                if (const_count == const_cap) {
                    int new_cap = const_cap == 0 ? 16 : const_cap * 2;
                    char **new_keys = realloc(const_key, (size_t) new_cap * sizeof(char *));
                    int *new_temps = realloc(const_temp, (size_t) new_cap * sizeof(int));
                    if (!new_keys || !new_temps) {
                        free(new_keys);
                        free(new_temps);
                        free(key);
                        key = NULL;
                    } else {
                        const_key = new_keys;
                        const_temp = new_temps;
                        const_cap = new_cap;
                    }
                }
                if (key) {
                    const_key[const_count] = key;
                    const_temp[const_count] = def;
                    const_count++;
                }
            }
            temp_info_clear(&temps[def]);
            if (ir_const_from_tokens(kind, value, &temps[def].val)) {
                temps[def].has_const = 1;
            }
            continue;
        }

        if (parse_mov_def(ctx->lines[i], &def, &src)) {
            int src_res = temp_resolve(temps, max_temp, src);
            temp_info_clear(&temps[def]);
            if (src_res > 0 && src_res <= max_temp && temps[src_res].has_const) {
                char *val_str = ir_const_to_string(&temps[src_res].val);
                if (val_str) {
                    const char *kind_str = temps[src_res].val.kind == CONST_INT ? "int" :
                                           temps[src_res].val.kind == CONST_DECIMAL ? "decimal" :
                                           temps[src_res].val.kind == CONST_CHAR ? "char" : "string";
                    char *new_line = str_printf("t%d = const %s %s", def, kind_str, val_str);
                    free(val_str);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                        temps[def].has_const = 1;
                        temps[def].val.kind = temps[src_res].val.kind;
                        temps[def].val.i = temps[src_res].val.i;
                        temps[def].val.d = temps[src_res].val.d;
                        temps[def].val.s = temps[src_res].val.s ? strdup(temps[src_res].val.s) : NULL;
                    }
                }
            } else {
                temps[def].has_copy = 1;
                temps[def].copy_temp = src_res;
                if (src_res != src) {
                    char *new_line = str_printf("t%d = mov t%d", def, src_res);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                    }
                }
                if (src_res > 0 && src_res <= max_temp && temps[src_res].name_src) {
                    temps[def].name_src = strdup(temps[src_res].name_src);
                }
            }
            continue;
        }

        if (parse_addr_def(ctx->lines[i], &def, name, sizeof(name))) {
            temp_info_clear(&temps[def]);
            continue;
        }

        if (parse_load_name(ctx->lines[i], &def, name, sizeof(name))) {
            NameInfo *info = name_map_find(names, name);
            temp_info_clear(&temps[def]);
            if (info && info->has_const) {
                char *val_str = ir_const_to_string(&info->val);
                if (val_str) {
                    const char *kind_str = info->val.kind == CONST_INT ? "int" :
                                           info->val.kind == CONST_DECIMAL ? "decimal" :
                                           info->val.kind == CONST_CHAR ? "char" : "string";
                    char *new_line = str_printf("t%d = const %s %s", def, kind_str, val_str);
                    free(val_str);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                        temps[def].has_const = 1;
                        temps[def].val.kind = info->val.kind;
                        temps[def].val.i = info->val.i;
                        temps[def].val.d = info->val.d;
                        temps[def].val.s = info->val.s ? strdup(info->val.s) : NULL;
                    }
                }
            } else if (info && info->has_last_store) {
                int src_temp = temp_resolve(temps, max_temp, info->last_store_temp);
                if (src_temp > 0 && src_temp <= max_temp) {
                    char *new_line = str_printf("t%d = mov t%d", def, src_temp);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                    }
                    temps[def].has_copy = 1;
                    temps[def].copy_temp = src_temp;
                }
            } else if (info && info->has_copy && info->copy_name) {
                char *new_line = str_printf("t%d = load %s", def, info->copy_name);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
                temps[def].name_src = strdup(info->copy_name);
            } else {
                temps[def].name_src = strdup(name);
            }
            continue;
        }

        if (parse_load_addr(ctx->lines[i], &def, &addr)) {
            int addr_res = temp_resolve(temps, max_temp, addr);
            temp_info_clear(&temps[def]);
            if (addr_res != addr) {
                char *new_line = str_printf("t%d = load [t%d]", def, addr_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            name_map_clear_all_last_store(names);
            continue;
        }

        if (parse_store_line(ctx->lines[i], &addr, &val)) {
            int addr_res = temp_resolve(temps, max_temp, addr);
            int val_res = temp_resolve(temps, max_temp, val);
            if (addr_res != addr || val_res != val) {
                char *new_line = str_printf("store [t%d], t%d", addr_res, val_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            if (addr_res > 0 && addr_res <= max_temp && addr_map && addr_map[addr_res]) {
                const char *var = addr_map[addr_res];
                NameInfo *info = name_map_find(names, var);
                if (info && info->has_last_store && info->last_store_temp == val_res) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = NULL;
                    continue;
                }
                name_map_clear(&names, var);
                if (val_res > 0 && val_res <= max_temp) {
                    if (temps[val_res].has_const) {
                        name_map_set_const(&names, var, &temps[val_res].val);
                    } else if (temps[val_res].name_src) {
                        name_map_set_copy(&names, var, temps[val_res].name_src);
                    }
                }
                name_map_set_last_store(&names, var, val_res);
            } else {
                name_map_clear_all_last_store(names);
            }
            continue;
        }

        if (parse_scan_line(ctx->lines[i], &addr)) {
            int addr_res = temp_resolve(temps, max_temp, addr);
            if (addr_res != addr) {
                char *new_line = str_printf("scan [t%d]", addr_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            if (addr_res > 0 && addr_res <= max_temp && addr_map && addr_map[addr_res]) {
                name_map_clear(&names, addr_map[addr_res]);
            } else {
                name_map_clear_all_last_store(names);
            }
            continue;
        }

        if (parse_ifz_line(ctx->lines[i], &arg, &label)) {
            int arg_res = temp_resolve(temps, max_temp, arg);
            if (arg_res != arg) {
                char *new_line = str_printf("ifz t%d goto L%d", arg_res, label);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            continue;
        }

        if (parse_print_line(ctx->lines[i], &arg)) {
            int arg_res = temp_resolve(temps, max_temp, arg);
            if (arg_res != arg) {
                char *new_line = str_printf("print t%d", arg_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            continue;
        }

        if (parse_binary_op(ctx->lines[i], &def, op, sizeof(op), &lhs, &rhs)) {
            int lhs_res = temp_resolve(temps, max_temp, lhs);
            int rhs_res = temp_resolve(temps, max_temp, rhs);
            int is_add = 0, is_sub = 0, is_mul = 0, is_div = 0;
            temp_info_clear(&temps[def]);
            if (strcmp(op, "add") == 0 || strcmp(op, "sub") == 0 || strcmp(op, "mul") == 0) {
                name_map_clear_all_last_store(names);
            }

            if (is_foldable_op(op) &&
                lhs_res > 0 && lhs_res <= max_temp && rhs_res > 0 && rhs_res <= max_temp &&
                temps[lhs_res].has_const && temps[rhs_res].has_const &&
                temps[lhs_res].val.kind == CONST_INT && temps[rhs_res].val.kind == CONST_INT) {
                long long folded = 0;
                if (fold_int_binop(op, temps[lhs_res].val.i, temps[rhs_res].val.i, &folded)) {
                    char *new_line = str_printf("t%d = const int %lld", def, folded);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                        temps[def].has_const = 1;
                        temps[def].val.kind = CONST_INT;
                        temps[def].val.i = folded;
                        temps[def].val.s = str_printf("%lld", folded);
                    }
                    continue;
                }
            }

            if (lhs_res > 0 && lhs_res <= max_temp && rhs_res > 0 && rhs_res <= max_temp) {
                is_identity_op(op, &is_add, &is_sub, &is_mul, &is_div);
                if (is_add || is_sub || is_mul || is_div) {
                    int lhs_const = temps[lhs_res].has_const && temps[lhs_res].val.kind == CONST_INT;
                    int rhs_const = temps[rhs_res].has_const && temps[rhs_res].val.kind == CONST_INT;
                    long long lhs_val = lhs_const ? temps[lhs_res].val.i : 0;
                    long long rhs_val = rhs_const ? temps[rhs_res].val.i : 0;
                    if (is_add) {
                        if (rhs_const && rhs_val == 0) {
                            char *new_line = str_printf("t%d = mov t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = lhs_res;
                            continue;
                        }
                        if (lhs_const && lhs_val == 0) {
                            char *new_line = str_printf("t%d = mov t%d", def, rhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = rhs_res;
                            continue;
                        }
                    }
                    if (is_sub) {
                        if (rhs_const && rhs_val == 0) {
                            char *new_line = str_printf("t%d = mov t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = lhs_res;
                            continue;
                        }
                        if (lhs_const && lhs_val == 0) {
                            char *new_line = str_printf("t%d = uminus t%d", def, rhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            continue;
                        }
                    }
                    if (is_mul) {
                        if ((lhs_const && lhs_val == 0) || (rhs_const && rhs_val == 0)) {
                            char *new_line = str_printf("t%d = const int 0", def);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                                temps[def].has_const = 1;
                                temps[def].val.kind = CONST_INT;
                                temps[def].val.i = 0;
                                temps[def].val.s = strdup("0");
                            }
                            continue;
                        }
                        if (lhs_const && lhs_val == 1) {
                            char *new_line = str_printf("t%d = mov t%d", def, rhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = rhs_res;
                            continue;
                        }
                        if (rhs_const && rhs_val == 1) {
                            char *new_line = str_printf("t%d = mov t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = lhs_res;
                            continue;
                        }
                        if (lhs_const && lhs_val == -1) {
                            char *new_line = str_printf("t%d = uminus t%d", def, rhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            continue;
                        }
                        if (rhs_const && rhs_val == -1) {
                            char *new_line = str_printf("t%d = uminus t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            continue;
                        }
                    }
                    if (is_div) {
                        if (rhs_const && rhs_val == 1) {
                            char *new_line = str_printf("t%d = mov t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            temps[def].has_copy = 1;
                            temps[def].copy_temp = lhs_res;
                            continue;
                        }
                        if (rhs_const && rhs_val == -1) {
                            char *new_line = str_printf("t%d = uminus t%d", def, lhs_res);
                            if (new_line) {
                                free(ctx->lines[i]);
                                ctx->lines[i] = new_line;
                            }
                            continue;
                        }
                    }
                }
            }

            if (lhs_res != lhs || rhs_res != rhs) {
                char *new_line = str_printf("t%d = %s t%d, t%d", def, op, lhs_res, rhs_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            continue;
        }

        if (parse_unary_op(ctx->lines[i], &def, op, sizeof(op), &arg)) {
            int arg_res = temp_resolve(temps, max_temp, arg);
            temp_info_clear(&temps[def]);
            if (strcmp(op, "addr") == 0) {
                name_map_clear_all_last_store(names);
            }
            if ((strcmp(op, "uminus") == 0 || strcmp(op, "not") == 0) &&
                arg_res > 0 && arg_res <= max_temp &&
                temps[arg_res].has_const && temps[arg_res].val.kind == CONST_INT) {
                long long folded = 0;
                if (strcmp(op, "uminus") == 0) {
                    folded = -temps[arg_res].val.i;
                } else {
                    folded = temps[arg_res].val.i == 0 ? 1 : 0;
                }
                {
                    char *new_line = str_printf("t%d = const int %lld", def, folded);
                    if (new_line) {
                        free(ctx->lines[i]);
                        ctx->lines[i] = new_line;
                        temps[def].has_const = 1;
                        temps[def].val.kind = CONST_INT;
                        temps[def].val.i = folded;
                        temps[def].val.s = str_printf("%lld", folded);
                    }
                }
            } else if (arg_res != arg) {
                char *new_line = str_printf("t%d = %s t%d", def, op, arg_res);
                if (new_line) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = new_line;
                }
            }
            continue;
        }
    }

    for (int t = 0; t <= max_temp; t++) {
        temp_info_clear(&temps[t]);
    }
    for (int c = 0; c < const_count; c++) {
        free(const_key[c]);
    }
    free(const_key);
    free(const_temp);
    free(temps);
    name_map_free(names);
    free_addr_name_map(addr_map, max_temp);
}

static int name_is_in_set(const char *name, char **set, int count)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(set[i], name) == 0) {
            return 1;
        }
    }
    return 0;
}

static void apply_dead_named_elim(IRContext *ctx)
{
    char **addr_map = NULL;
    int max_temp = 0;
    char **scalar_names = NULL;
    int scalar_count = 0;
    int scalar_cap = 0;
    char **array_names = NULL;
    int array_count = 0;
    int array_cap = 0;
    char **matrix_names = NULL;
    int matrix_count = 0;
    int matrix_cap = 0;
    char **live = NULL;
    int live_count = 0;
    int live_cap = 0;

    if (!ctx || !ctx->lines) {
        return;
    }
    max_temp = find_max_temp(ctx->lines, ctx->line_count);
    addr_map = build_addr_name_map(ctx->lines, ctx->line_count, max_temp);

    for (int i = 0; i < ctx->line_count; i++) {
        char name[128];
        if (sscanf(ctx->lines[i], "decl %*s %127s", name) == 1) {
            if (scalar_count == scalar_cap) {
                int new_cap = scalar_cap == 0 ? 32 : scalar_cap * 2;
                char **next = realloc(scalar_names, (size_t) new_cap * sizeof(char *));
                if (!next) {
                    continue;
                }
                scalar_names = next;
                scalar_cap = new_cap;
            }
            scalar_names[scalar_count++] = strdup(name);
            continue;
        }
        if (sscanf(ctx->lines[i], "decl_array %127[^,],", name) == 1) {
            if (array_count == array_cap) {
                int new_cap = array_cap == 0 ? 32 : array_cap * 2;
                char **next = realloc(array_names, (size_t) new_cap * sizeof(char *));
                if (!next) {
                    continue;
                }
                array_names = next;
                array_cap = new_cap;
            }
            array_names[array_count++] = strdup(name);
            continue;
        }
        if (sscanf(ctx->lines[i], "decl_matrix %127[^,],", name) == 1) {
            if (matrix_count == matrix_cap) {
                int new_cap = matrix_cap == 0 ? 32 : matrix_cap * 2;
                char **next = realloc(matrix_names, (size_t) new_cap * sizeof(char *));
                if (!next) {
                    continue;
                }
                matrix_names = next;
                matrix_cap = new_cap;
            }
            matrix_names[matrix_count++] = strdup(name);
            continue;
        }
    }

    for (int i = ctx->line_count - 1; i >= 0; i--) {
        int temp = 0;
        int addr = 0;
        int val = 0;
        char name[128];

        if (parse_load_name(ctx->lines[i], &temp, name, sizeof(name))) {
            if (!name_is_in_set(name, live, live_count)) {
                if (live_count == live_cap) {
                    int new_cap = live_cap == 0 ? 32 : live_cap * 2;
                    char **next = realloc(live, (size_t) new_cap * sizeof(char *));
                    if (!next) {
                        continue;
                    }
                    live = next;
                    live_cap = new_cap;
                }
                live[live_count++] = strdup(name);
            }
            continue;
        }
        if (parse_load_addr(ctx->lines[i], &temp, &addr)) {
            const char *var = NULL;
            if (addr_map && addr > 0 && addr <= max_temp) {
                var = addr_map[addr];
            }
            if (var && (name_is_in_set(var, matrix_names, matrix_count) ||
                        name_is_in_set(var, array_names, array_count))) {
                if (!name_is_in_set(var, live, live_count)) {
                    if (live_count == live_cap) {
                        int new_cap = live_cap == 0 ? 32 : live_cap * 2;
                        char **next = realloc(live, (size_t) new_cap * sizeof(char *));
                        if (!next) {
                            continue;
                        }
                        live = next;
                        live_cap = new_cap;
                    }
                    live[live_count++] = strdup(var);
                }
            }
            continue;
        }
        if (parse_store_line(ctx->lines[i], &addr, &val)) {
            const char *var = NULL;
            if (addr_map && addr > 0 && addr <= max_temp) {
                var = addr_map[addr];
            }
            if (var && name_is_in_set(var, scalar_names, scalar_count) &&
                !name_is_in_set(var, array_names, array_count) &&
                !name_is_in_set(var, matrix_names, matrix_count)) {
                if (!name_is_in_set(var, live, live_count)) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = NULL;
                } else {
                    for (int j = 0; j < live_count; j++) {
                        if (strcmp(live[j], var) == 0) {
                            free(live[j]);
                            live[j] = live[live_count - 1];
                            live_count--;
                            break;
                        }
                    }
                }
            }
            if (var && name_is_in_set(var, matrix_names, matrix_count)) {
                if (!name_is_in_set(var, live, live_count)) {
                    free(ctx->lines[i]);
                    ctx->lines[i] = NULL;
                } else {
                    for (int j = 0; j < live_count; j++) {
                        if (strcmp(live[j], var) == 0) {
                            free(live[j]);
                            live[j] = live[live_count - 1];
                            live_count--;
                            break;
                        }
                    }
                }
            }
            continue;
        }
        if (parse_scan_line(ctx->lines[i], &addr)) {
            const char *var = NULL;
            if (addr_map && addr > 0 && addr <= max_temp) {
                var = addr_map[addr];
            }
            if (var && name_is_in_set(var, scalar_names, scalar_count) &&
                !name_is_in_set(var, array_names, array_count) &&
                !name_is_in_set(var, matrix_names, matrix_count)) {
                for (int j = 0; j < live_count; j++) {
                    if (strcmp(live[j], var) == 0) {
                        free(live[j]);
                        live[j] = live[live_count - 1];
                        live_count--;
                        break;
                    }
                }
            }
            if (var && name_is_in_set(var, matrix_names, matrix_count)) {
                for (int j = 0; j < live_count; j++) {
                    if (strcmp(live[j], var) == 0) {
                        free(live[j]);
                        live[j] = live[live_count - 1];
                        live_count--;
                        break;
                    }
                }
            }
            continue;
        }
    }

    {
        int out = 0;
        for (int i = 0; i < ctx->line_count; i++) {
            if (ctx->lines[i]) {
                ctx->lines[out++] = ctx->lines[i];
            }
        }
        ctx->line_count = out;
    }

    for (int i = 0; i < scalar_count; i++) {
        free(scalar_names[i]);
    }
    for (int i = 0; i < array_count; i++) {
        free(array_names[i]);
    }
    for (int i = 0; i < matrix_count; i++) {
        free(matrix_names[i]);
    }
    for (int i = 0; i < live_count; i++) {
        free(live[i]);
    }
    free(scalar_names);
    free(array_names);
    free(matrix_names);
    free(live);
    free_addr_name_map(addr_map, max_temp);
}

static void apply_dead_temp_elim(IRContext *ctx)
{
    int max_temp = 0;
    int *use_counts = NULL;
    int *def_line = NULL;
    int *queue = NULL;
    int queue_count = 0;

    if (!ctx || !ctx->lines) {
        return;
    }
    max_temp = find_max_temp(ctx->lines, ctx->line_count);
    if (max_temp <= 0) {
        return;
    }
    use_counts = calloc((size_t) max_temp + 1, sizeof(int));
    def_line = calloc((size_t) max_temp + 1, sizeof(int));
    queue = calloc((size_t) max_temp + 1, sizeof(int));
    if (!use_counts || !def_line || !queue) {
        free(use_counts);
        free(def_line);
        free(queue);
        return;
    }
    for (int i = 0; i <= max_temp; i++) {
        def_line[i] = -1;
    }

    for (int i = 0; i < ctx->line_count; i++) {
        int def = 0;
        if (parse_temp_def(ctx->lines[i], &def)) {
            def_line[def] = i;
        }
        {
            const char *line = ctx->lines[i];
            int pos = 0;
            int skip_def = 0;
            if (parse_temp_def(line, &def) && line[0] == 't') {
                skip_def = 1;
            }
            while (line && line[pos]) {
                if (line[pos] == 't' && isdigit((unsigned char) line[pos + 1])) {
                    int val = 0;
                    int j = pos + 1;
                    while (line[j] && isdigit((unsigned char) line[j])) {
                        val = val * 10 + (line[j] - '0');
                        j++;
                    }
                    if (!(skip_def && pos == 0)) {
                        if (val > 0 && val <= max_temp) {
                            use_counts[val]++;
                        }
                    }
                    pos = j;
                    continue;
                }
                pos++;
            }
        }
    }

    for (int t = 1; t <= max_temp; t++) {
        if (use_counts[t] == 0 && def_line[t] >= 0) {
            queue[queue_count++] = t;
        }
    }

    while (queue_count > 0) {
        int t = queue[--queue_count];
        int line_idx = def_line[t];
        if (line_idx < 0 || !ctx->lines[line_idx]) {
            continue;
        }
        {
            const char *line = ctx->lines[line_idx];
            int def = 0;
            if (!parse_temp_def(line, &def)) {
                continue;
            }
            {
                const char *p = line;
                int pos = 0;
                while (p && p[pos]) {
                    if (p[pos] == 't' && isdigit((unsigned char) p[pos + 1])) {
                        int val = 0;
                        int j = pos + 1;
                        while (p[j] && isdigit((unsigned char) p[j])) {
                            val = val * 10 + (p[j] - '0');
                            j++;
                        }
                        if (pos != 0) {
                            if (val > 0 && val <= max_temp) {
                                use_counts[val]--;
                                if (use_counts[val] == 0 && def_line[val] >= 0) {
                                    queue[queue_count++] = val;
                                }
                            }
                        }
                        pos = j;
                        continue;
                    }
                    pos++;
                }
            }
        }
        free(ctx->lines[line_idx]);
        ctx->lines[line_idx] = NULL;
    }

    {
        int out = 0;
        for (int i = 0; i < ctx->line_count; i++) {
            if (ctx->lines[i]) {
                ctx->lines[out++] = ctx->lines[i];
            }
        }
        ctx->line_count = out;
    }

    free(use_counts);
    free(def_line);
    free(queue);
}

static void optimize_ir(IRContext *ctx)
{
    if (!ctx || !ctx->lines) {
        return;
    }
    if (ctx->opt_level <= IR_OPT_O0) {
        ir_run_o0_pipeline(ctx);
        return;
    }
    apply_jump_threading(ctx);
    apply_jump_simplify(ctx);
    apply_unreachable_elim(ctx);
    apply_propagation(ctx);
}

void generate_ir(ASTNode *root, FILE *out)
{
    generate_ir_level(root, out, IR_OPT_O1);
}

void generate_ir_level(ASTNode *root, FILE *out, IROptLevel opt_level)
{
    IRContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.opt_level = opt_level;
    collect_matrix_decls(&ctx, root);
    ctx.capture = 1;
    ir_printf(&ctx, out, "IR_BEGIN");
    emit_stmt(&ctx, root, out, -1, -1);
    ir_printf(&ctx, out, "IR_END");
    optimize_ir(&ctx);
    ctx.capture = 0;
    for (int i = 0; i < ctx.line_count; i++) {
        if (ctx.lines[i]) {
            fputs(ctx.lines[i], out);
            fputc('\n', out);
        }
    }
    free_matrix_info(&ctx);
    free_array_info(&ctx);
    const_clear(&ctx);
    cse_clear(&ctx);
    ir_lines_free(&ctx);
}
