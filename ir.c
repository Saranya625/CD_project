#include "ir.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int temp_id;
    int label_id;
    int indent;
} IRContext;

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
static void emit_stmt(IRContext *ctx, ASTNode *node, FILE *out, int break_label, int continue_label);

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

static int emit_expr(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return -1;
    }

    if (strcmp(node->type, "int") == 0) {
        return emit_const(ctx, "int", node->value, out);
    }
    if (strcmp(node->type, "decimal") == 0) {
        return emit_const(ctx, "decimal", node->value, out);
    }
    if (strcmp(node->type, "char") == 0) {
        return emit_const(ctx, "char", node->value, out);
    }
    if (strcmp(node->type, "string") == 0) {
        return emit_const(ctx, "string", node->value, out);
    }
    if (strcmp(node->type, "id") == 0) {
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load %s", t, safe_str(node->value));
        return t;
    }
    if (strcmp(node->type, "array_access") == 0 || strcmp(node->type, "matrix_access") == 0) {
        int addr = emit_lvalue_addr(ctx, node, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = load [t%d]", t, addr);
        return t;
    }
    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 ||
        strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0 ||
        strcmp(node->type, "%") == 0 ||
        strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0 ||
        strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0) {
        int lhs = emit_expr(ctx, node->left, out);
        int rhs = emit_expr(ctx, node->right, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", t, node->type, lhs, rhs);
        return t;
    }
    if (strcmp(node->type, "not") == 0 || strcmp(node->type, "uminus") == 0) {
        int v = emit_expr(ctx, node->left, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d", t, node->type, v);
        return t;
    }
    if (strcmp(node->type, "size") == 0 ||
        strcmp(node->type, "sort") == 0 ||
        strcmp(node->type, "transpose") == 0 ||
        strcmp(node->type, "det") == 0 ||
        strcmp(node->type, "inv") == 0 ||
        strcmp(node->type, "shape") == 0) {
        int v = emit_expr(ctx, node->left, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d", t, node->type, v);
        return t;
    }
    if (strcmp(node->type, "matadd") == 0 ||
        strcmp(node->type, "matsub") == 0 ||
        strcmp(node->type, "matmul") == 0) {
        int a = emit_expr(ctx, node->left, out);
        int b = emit_expr(ctx, node->right, out);
        int t = new_temp(ctx);
        ir_printf(ctx, out, "t%d = %s t%d, t%d", t, node->type, a, b);
        return t;
    }
    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
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
        return result;
    }

    return -1;
}

static void emit_assignment(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
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
}

static void emit_decl(IRContext *ctx, ASTNode *node, FILE *out)
{
    if (!node) {
        return;
    }
    const char *type = safe_str(node->left ? node->left->value : NULL);
    const char *name = safe_str(node->right ? node->right->value : NULL);
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
    if (strcmp(node->type, "if_else") == 0) {
        int else_label = new_label(ctx);
        int end_label = new_label(ctx);
        ASTNode *left = node->left;
        if (left && strcmp(left->type, "if") == 0) {
            int cond = emit_expr(ctx, left->left, out);
            ir_printf(ctx, out, "ifz t%d goto L%d", cond, else_label);
            emit_stmt(ctx, left->right, out, break_label, continue_label);
            ir_printf(ctx, out, "goto L%d", end_label);
            ir_printf(ctx, out, "label L%d", else_label);
            emit_if_else_chain(ctx, node->right, out, break_label, continue_label);
            ir_printf(ctx, out, "label L%d", end_label);
            return;
        }
        if (left) {
            int cond = emit_expr(ctx, left, out);
            ir_printf(ctx, out, "ifz t%d goto L%d", cond, else_label);
            emit_stmt(ctx, node->right, out, break_label, continue_label);
            ir_printf(ctx, out, "goto L%d", end_label);
            ir_printf(ctx, out, "label L%d", else_label);
            emit_stmt(ctx, node->third, out, break_label, continue_label);
            ir_printf(ctx, out, "label L%d", end_label);
            return;
        }
    }
    if (strcmp(node->type, "if") == 0) {
        int end_label = new_label(ctx);
        int cond = emit_expr(ctx, node->left, out);
        ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        ir_printf(ctx, out, "label L%d", end_label);
        return;
    }
    emit_stmt(ctx, node, out, break_label, continue_label);
}

static void emit_for_body(IRContext *ctx, ASTNode *node, FILE *out)
{
    ASTNode *for_node = node->left;
    ASTNode *body = node->right;
    int start_label = new_label(ctx);
    int end_label = new_label(ctx);
    int continue_label = new_label(ctx);

    emit_stmt(ctx, for_node->left, out, end_label, continue_label);
    ir_printf(ctx, out, "label L%d", start_label);
    if (for_node->right) {
        int cond = emit_expr(ctx, for_node->right, out);
        ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
    }
    emit_stmt(ctx, body, out, end_label, continue_label);
    ir_printf(ctx, out, "label L%d", continue_label);
    emit_stmt(ctx, for_node->third, out, end_label, continue_label);
    ir_printf(ctx, out, "goto L%d", start_label);
    ir_printf(ctx, out, "label L%d", end_label);
}

static void emit_while(IRContext *ctx, ASTNode *node, FILE *out)
{
    int start_label = new_label(ctx);
    int end_label = new_label(ctx);
    int continue_label = new_label(ctx);
    ir_printf(ctx, out, "label L%d", start_label);
    int cond = emit_expr(ctx, node->left, out);
    ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
    emit_stmt(ctx, node->right, out, end_label, continue_label);
    ir_printf(ctx, out, "label L%d", continue_label);
    ir_printf(ctx, out, "goto L%d", start_label);
    ir_printf(ctx, out, "label L%d", end_label);
}

static void emit_switch_cases(IRContext *ctx, ASTNode *node, FILE *out, int switch_temp, int end_label)
{
    if (!node) {
        return;
    }
    if (strcmp(node->type, "cases") == 0) {
        emit_switch_cases(ctx, node->left, out, switch_temp, end_label);
        emit_switch_cases(ctx, node->right, out, switch_temp, end_label);
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
        emit_stmt(ctx, node->right, out, end_label, -1);
        ir_printf(ctx, out, "goto L%d", end_label);
        ir_printf(ctx, out, "label L%d", next_label);
        return;
    }
    if (strcmp(node->type, "default") == 0) {
        int def_label = new_label(ctx);
        ir_printf(ctx, out, "label L%d", def_label);
        emit_stmt(ctx, node->left, out, end_label, -1);
        return;
    }
}

static void emit_switch(IRContext *ctx, ASTNode *node, FILE *out)
{
    int end_label = new_label(ctx);
    int switch_temp = emit_expr(ctx, node->left, out);
    emit_switch_cases(ctx, node->right, out, switch_temp, end_label);
    ir_printf(ctx, out, "label L%d", end_label);
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
        return;
    }
    if (strcmp(node->type, "if") == 0) {
        int end_label = new_label(ctx);
        int cond = emit_expr(ctx, node->left, out);
        ir_printf(ctx, out, "ifz t%d goto L%d", cond, end_label);
        emit_stmt(ctx, node->right, out, break_label, continue_label);
        ir_printf(ctx, out, "label L%d", end_label);
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
    ir_printf(&ctx, out, "IR_BEGIN");
    emit_stmt(&ctx, root, out, -1, -1);
    ir_printf(&ctx, out, "IR_END");
}
