#include "ir.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int temp_id;
    int label_id;
    int indent;
    struct MatrixInfo *matrices;
    struct ArrayInfo *arrays;
    struct ConstBinding *consts;
    struct ExprCache *cse_cache;
} IRContext;

typedef struct MatrixInfo {
    char *name;
    int rows;
    int cols;
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

static void ir_printf(IRContext *ctx, FILE *out, const char *fmt, ...)
{
    va_list args;
    for (int i = 0; i < ctx->indent; i++) {
        fputs("  ", out);
    }
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);
    fputc('\n', out);
}

static int new_temp(IRContext *ctx)
{
    ctx->temp_id++;
    return ctx->temp_id;
}

static int new_label(IRContext *ctx)
{
    ctx->label_id++;
    return ctx->label_id;
}

static const char *safe_str(const char *s)
{
    return s ? s : "";
}

static int emit_expr(IRContext *ctx, ASTNode *node, FILE *out);
static int emit_const(IRContext *ctx, const char *kind, const char *value, FILE *out);
static void emit_stmt(IRContext *ctx, ASTNode *node, FILE *out, int break_label, int continue_label);

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
    info->next = ctx->matrices;
    ctx->matrices = info;
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
    ir_printf(ctx, out, "t%d = mul t%d, 1", off, idx_temp);
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
        int addr_a;
        int addr_b;
        int addr_dst;
        int val_a;
        int val_b;
        int val_out;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);

        addr_a = emit_matrix_elem_addr(ctx, a, row_val, col_val, out);
        addr_b = emit_matrix_elem_addr(ctx, b, row_val, col_val, out);
        addr_dst = emit_matrix_elem_addr(ctx, dst, row_val, col_val, out);

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
        int addr_src;
        int addr_dst;
        int val_src;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);

        addr_src = emit_matrix_elem_addr(ctx, src, row_val, col_val, out);
        addr_dst = emit_matrix_elem_addr(ctx, dst, row_val, col_val, out);

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
        int addr_a;
        int addr_b;
        int val_a;
        int val_b;
        int mul_val;
        int acc_val;
        int acc_next;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = load %s", k_val, k_var);

        addr_a = emit_matrix_elem_addr(ctx, a, row_val, k_val, out);
        addr_b = emit_matrix_elem_addr(ctx, b, k_val, col_val, out);

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
        int addr_dst;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);
        ir_printf(ctx, out, "t%d = load %s", acc_val, acc_var);

        addr_dst = emit_matrix_elem_addr(ctx, dst, row_val, col_val, out);
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
        int addr_src;
        int addr_dst;
        int val_src;

        ir_printf(ctx, out, "t%d = load %s", row_val, row_var);
        ir_printf(ctx, out, "t%d = load %s", col_val, col_var);

        addr_src = emit_matrix_elem_addr(ctx, src, col_val, row_val, out);
        addr_dst = emit_matrix_elem_addr(ctx, dst, row_val, col_val, out);

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
            int even_label;
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
            ir_printf(ctx, out, "t%d = % t%d, t%d", mod_val, col_val_inner, two);
            ir_printf(ctx, out, "t%d = eq t%d, t%d", is_odd, mod_val, one);
            even_label = new_label(ctx);
            ir_printf(ctx, out, "ifz t%d goto L%d", is_odd, even_label);
            {
                int neg = new_temp(ctx);
                ir_printf(ctx, out, "t%d = uminus t%d", neg, term);
                term = neg;
            }
            ir_printf(ctx, out, "label L%d", even_label);

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
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, one);
        ir_printf(ctx, out, "goto L%d", after_set_label);
        ir_printf(ctx, out, "label L%d", set_one_label);
        addr_dst = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);
        ir_printf(ctx, out, "store [t%d], t%d", addr_dst, zero);
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

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        addr_pivot = emit_matrix_elem_addr(ctx, a_name, i_val, i_val, out);
        pivot = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", pivot, addr_pivot);

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
            int new_a;
            int new_inv;

            ir_printf(ctx, out, "t%d = load %s", j_val, j_var);
            addr_a = emit_matrix_elem_addr(ctx, a_name, i_val, j_val, out);
            addr_inv = emit_matrix_elem_addr(ctx, dst, i_val, j_val, out);
            val_a = new_temp(ctx);
            val_inv = new_temp(ctx);
            new_a = new_temp(ctx);
            new_inv = new_temp(ctx);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a, addr_a);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv, addr_inv);
            ir_printf(ctx, out, "t%d = / t%d, t%d", new_a, val_a, pivot);
            ir_printf(ctx, out, "t%d = / t%d, t%d", new_inv, val_inv, pivot);
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
        int addr_factor;
        int factor;

        ir_printf(ctx, out, "t%d = load %s", i_val, i_var);
        ir_printf(ctx, out, "t%d = load %s", r_val, r_var);
        addr_factor = emit_matrix_elem_addr(ctx, a_name, r_val, i_val, out);
        factor = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", factor, addr_factor);

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
            mul_a = new_temp(ctx);
            mul_inv = new_temp(ctx);
            new_a = new_temp(ctx);
            new_inv = new_temp(ctx);

            ir_printf(ctx, out, "t%d = load [t%d]", val_a_r, addr_a_r);
            ir_printf(ctx, out, "t%d = load [t%d]", val_a_i, addr_a_i);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv_r, addr_inv_r);
            ir_printf(ctx, out, "t%d = load [t%d]", val_inv_i, addr_inv_i);

            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_a, factor, val_a_i);
            ir_printf(ctx, out, "t%d = * t%d, t%d", mul_inv, factor, val_inv_i);
            ir_printf(ctx, out, "t%d = - t%d, t%d", new_a, val_a_r, mul_a);
            ir_printf(ctx, out, "t%d = - t%d, t%d", new_inv, val_inv_r, mul_inv);
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
        ir_printf(ctx, out, "t%d = mul t%d, 1", off, idx);
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
        ir_printf(ctx, out, "t%d = mul t%d, 1", off, col_off);
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
    if (eval_const_expr(ctx, node, &folded)) {
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
        int left_is_const = eval_const_expr(ctx, node->left, &left_val);
        int right_is_const = eval_const_expr(ctx, node->right, &right_val);

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
                int det_val = emit_matrix_det(ctx, node->left->value, info->rows, out);
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
    return emit_expr_internal(ctx, node, out, &ctx->cse_cache, 1);
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
            if (strcmp(rhs_node->type, "transpose") == 0) {
                const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                MatrixInfo *src_info = find_matrix_info(ctx, src_name);
                int rows = lhs_info->rows;
                int cols = lhs_info->cols;
                if (src_info && (rows <= 0 || cols <= 0)) {
                    rows = src_info->cols;
                    cols = src_info->rows;
                }
                if (src_info && emit_matrix_transpose(ctx, node->value, src_name, rows, cols, out)) {
                    cse_invalidate_name(ctx, node->value);
                    return;
                }
            }
            if (strcmp(rhs_node->type, "inv") == 0) {
                const char *src_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                MatrixInfo *src_info = find_matrix_info(ctx, src_name);
                if (src_info && src_info->rows > 0 && src_info->cols > 0 && src_info->rows == src_info->cols) {
                    if (emit_matrix_inverse(ctx, node->value, src_name, src_info->rows, out)) {
                        cse_invalidate_name(ctx, node->value);
                        return;
                    }
                }
            }
            if (strcmp(rhs_node->type, "matadd") == 0 || strcmp(rhs_node->type, "matsub") == 0 ||
                strcmp(rhs_node->type, "matmul") == 0) {
                const char *left_name = (rhs_node->left && strcmp(rhs_node->left->type, "id") == 0) ? rhs_node->left->value : NULL;
                const char *right_name = (rhs_node->right && strcmp(rhs_node->right->type, "id") == 0) ? rhs_node->right->value : NULL;
                MatrixInfo *left_info = find_matrix_info(ctx, left_name);
                MatrixInfo *right_info = find_matrix_info(ctx, right_name);
                if (left_info && right_info) {
                    if (strcmp(rhs_node->type, "matmul") == 0) {
                        if (emit_matrix_mul(ctx, node->value, left_name, right_name,
                                            left_info->rows, left_info->cols, right_info->cols, out)) {
                            cse_invalidate_name(ctx, node->value);
                            return;
                        }
                    } else {
                        const char *op = (strcmp(rhs_node->type, "matadd") == 0) ? "+" : "-";
                        if (emit_matrix_add_sub(ctx, node->value, left_name, right_name,
                                                left_info->rows, left_info->cols, op, out)) {
                            cse_invalidate_name(ctx, node->value);
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
                if (rhs_info && emit_matrix_copy(ctx, node->value, rhs_node->value, rows, cols, out)) {
                    cse_invalidate_name(ctx, node->value);
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
                    cse_invalidate_name(ctx, node->value);
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
                    cse_invalidate_name(ctx, node->value);
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
                int det_temp = emit_matrix_det(ctx, src_name, src_info->rows, out);
                if (det_temp >= 0) {
                    ASTNode id_node;
                    int addr;
                    memset(&id_node, 0, sizeof(id_node));
                    id_node.type = "id";
                    id_node.value = node->value;
                    addr = emit_lvalue_addr(ctx, &id_node, out);
                    ir_printf(ctx, out, "store [t%d], t%d", addr, det_temp);
                    if (lhs_is_scalar) {
                        const_unbind(ctx, lhs_name);
                    }
                    cse_invalidate_name(ctx, node->value);
                    return;
                }
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
        cse_invalidate_name(ctx, lhs_name);
    } else if (node->value) {
        cse_invalidate_name(ctx, node->value);
    } else if (node->left && node->left->value) {
        cse_invalidate_name(ctx, node->left->value);
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

static void emit_array_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    ir_printf(ctx, out, "decl_array %s, %s", safe_str(node->value), safe_str(node->left ? node->left->value : NULL));
}

static void emit_matrix_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    const char *rows = safe_str(node->left ? node->left->value : NULL);
    const char *cols = safe_str(node->right ? node->right->value : NULL);
    ir_printf(ctx, out, "decl_matrix %s, %s, %s", safe_str(node->value), rows, cols);
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
    if (for_node->right && eval_const_expr(ctx, for_node->right, &folded) &&
        const_to_bool(&folded, &cond_bool)) {
        cond_is_const = 1;
        if (!cond_bool) {
            return;
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
    if (eval_const_expr(ctx, node->left, &folded) &&
        const_to_bool(&folded, &cond_bool)) {
        cond_is_const = 1;
        if (!cond_bool) {
            return;
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
            cse_invalidate_name(ctx, node->left->value);
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
            cse_invalidate_name(ctx, node->left->value);
        } else if (node->left && node->left->value) {
            cse_invalidate_name(ctx, node->left->value);
        }
        return;
    }
    if (strcmp(node->type, "empty") == 0) {
        return;
    }

    emit_expr(ctx, node, out);
}

void generate_ir(ASTNode *root, FILE *out)
{
    IRContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    collect_matrix_decls(&ctx, root);
    ir_printf(&ctx, out, "IR_BEGIN");
    emit_stmt(&ctx, root, out, -1, -1);
    ir_printf(&ctx, out, "IR_END");
    free_matrix_info(&ctx);
    free_array_info(&ctx);
    const_clear(&ctx);
    cse_clear(&ctx);
}
