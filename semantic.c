#include "semantic.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum SymbolType {
    SYM_INVALID = 0,
    SYM_INT,
    SYM_DECIMAL,
    SYM_CHAR,
    SYM_STRING,
    SYM_ARRAY,
    SYM_MATRIX
} SymbolType;

typedef struct Symbol {
    char *name;
    SymbolType type;
    int size;
    int rows;
    int cols;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
} Scope;

typedef struct ExprInfo {
    SymbolType type;
    int size;
    int rows;
    int cols;
    int has_const_int;
    long long const_int;
} ExprInfo;

static Scope *current_scope = NULL;
static int semantic_errors = 0;

static const char *classify_semantic_error(const char *message)
{
    if (!message) {
        return "Semantic Error";
    }

    if (strstr(message, "always true") != NULL ||
        strstr(message, "always false") != NULL ||
        strstr(message, "never executes") != NULL ||
        strstr(message, "division by zero") != NULL ||
        strstr(message, "modulo by zero") != NULL) {
        return "Logical Error";
    }
    if (strstr(message, "bounds check failed") != NULL ||
        strstr(message, "possible runtime crash") != NULL) {
        return "Runtime Semantic Error";
    }
    if (strstr(message, "undeclared") != NULL ||
        strstr(message, "redeclaration") != NULL ||
        strstr(message, "outside loop") != NULL ||
        strstr(message, "outside loop/switch") != NULL) {
        return "Scope Error";
    }
    if (strstr(message, "type mismatch") != NULL ||
        strstr(message, "requires") != NULL ||
        strstr(message, "incompatible") != NULL ||
        strstr(message, "not an array") != NULL ||
        strstr(message, "not a matrix") != NULL ||
        strstr(message, "must be") != NULL ||
        strstr(message, "matrix shape mismatch") != NULL ||
        strstr(message, "array size mismatch") != NULL) {
        return "Type Error";
    }

    return "Semantic Error";
}

static ExprInfo make_info(SymbolType type)
{
    ExprInfo info;
    info.type = type;
    info.size = -1;
    info.rows = -1;
    info.cols = -1;
    info.has_const_int = 0;
    info.const_int = 0;
    return info;
}

static ExprInfo invalid_info(void)
{
    return make_info(SYM_INVALID);
}

static ExprInfo int_const_info(long long value)
{
    ExprInfo info = make_info(SYM_INT);
    info.has_const_int = 1;
    info.const_int = value;
    return info;
}

static void semantic_error(const char *fmt, ...)
{
    va_list args;
    char message[1024];
    const char *category;

    semantic_errors++;

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    category = classify_semantic_error(message);
    fprintf(stderr, "Semantic error [%s]: %s\n", category, message);
}

static const char *type_name(SymbolType type)
{
    switch (type) {
        case SYM_INT:
            return "int";
        case SYM_DECIMAL:
            return "decimal";
        case SYM_CHAR:
            return "char";
        case SYM_STRING:
            return "string";
        case SYM_ARRAY:
            return "array";
        case SYM_MATRIX:
            return "matrix";
        default:
            return "invalid";
    }
}

static int is_numeric_scalar(SymbolType type)
{
    return type == SYM_INT || type == SYM_DECIMAL || type == SYM_CHAR;
}

static int is_int_like(SymbolType type)
{
    return type == SYM_INT || type == SYM_CHAR;
}

static int is_scalar(SymbolType type)
{
    return is_numeric_scalar(type) || type == SYM_STRING;
}

static SymbolType parse_declared_scalar_type(const char *type_name_str)
{
    if (!type_name_str) {
        return SYM_INVALID;
    }
    if (strcmp(type_name_str, "int") == 0) {
        return SYM_INT;
    }
    if (strcmp(type_name_str, "decimal") == 0) {
        return SYM_DECIMAL;
    }
    if (strcmp(type_name_str, "char") == 0) {
        return SYM_CHAR;
    }
    if (strcmp(type_name_str, "string") == 0) {
        return SYM_STRING;
    }
    return SYM_INVALID;
}

static int is_assignable(SymbolType lhs, SymbolType rhs)
{
    if (lhs == SYM_INVALID || rhs == SYM_INVALID) {
        return 1;
    }
    if (lhs == rhs) {
        return 1;
    }
    if (lhs == SYM_DECIMAL && (rhs == SYM_INT || rhs == SYM_CHAR)) {
        return 1;
    }
    if ((lhs == SYM_INT && rhs == SYM_CHAR) || (lhs == SYM_CHAR && rhs == SYM_INT)) {
        return 1;
    }
    return 0;
}

static void enter_scope(void)
{
    Scope *scope = malloc(sizeof(Scope));
    scope->symbols = NULL;
    scope->parent = current_scope;
    current_scope = scope;
}

static void exit_scope(void)
{
    Symbol *sym;
    Symbol *next;
    Scope *parent;

    if (!current_scope) {
        return;
    }

    sym = current_scope->symbols;
    while (sym) {
        next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }

    parent = current_scope->parent;
    free(current_scope);
    current_scope = parent;
}

static Symbol *lookup_current_scope(const char *name)
{
    Symbol *sym;

    if (!current_scope || !name) {
        return NULL;
    }

    sym = current_scope->symbols;
    while (sym) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
        sym = sym->next;
    }
    return NULL;
}

static Symbol *lookup_symbol(const char *name)
{
    Scope *scope;

    if (!name) {
        return NULL;
    }

    scope = current_scope;
    while (scope) {
        Symbol *sym = scope->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
            sym = sym->next;
        }
        scope = scope->parent;
    }

    return NULL;
}

static void declare_symbol(const char *name, SymbolType type, int size, int rows, int cols)
{
    Symbol *sym;

    if (!name) {
        return;
    }

    if (lookup_current_scope(name)) {
        semantic_error("redeclaration of '%s'", name);
        return;
    }

    sym = malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = type;
    sym->size = size;
    sym->rows = rows;
    sym->cols = cols;
    sym->next = current_scope->symbols;
    current_scope->symbols = sym;
}

static int parse_int_literal(const char *text, long long *value)
{
    char *end = NULL;
    long long v;

    if (!text || !value) {
        return 0;
    }

    v = strtoll(text, &end, 10);
    if (end == text || *end != '\0') {
        return 0;
    }

    *value = v;
    return 1;
}

static ExprInfo analyze_expression(ASTNode *node, int loop_depth, int switch_depth);

static int count_init_list(ASTNode *node)
{
    if (!node) {
        return 0;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        return count_init_list(node->left) + 1;
    }
    return 1;
}

static int count_row_list(ASTNode *node)
{
    if (!node) {
        return 0;
    }
    if (node->type && strcmp(node->type, "init_rows") == 0) {
        return count_row_list(node->left) + 1;
    }
    if (node->type && strcmp(node->type, "init_row") == 0) {
        return 1;
    }
    return 0;
}

static void validate_init_list(ASTNode *node, int loop_depth, int switch_depth)
{
    if (!node) {
        return;
    }
    if (node->type && strcmp(node->type, "init_list") == 0) {
        validate_init_list(node->left, loop_depth, switch_depth);
        if (node->right) {
            ExprInfo expr = analyze_expression(node->right, loop_depth, switch_depth);
            if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
                semantic_error("initializer element must be scalar");
            }
        }
        return;
    }
    {
        ExprInfo expr = analyze_expression(node, loop_depth, switch_depth);
        if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
            semantic_error("initializer element must be scalar");
        }
    }
}

static void validate_matrix_init(ASTNode *node, int expected_cols, int loop_depth, int switch_depth)
{
    if (!node) {
        return;
    }
    if (node->type && strcmp(node->type, "init_rows") == 0) {
        validate_matrix_init(node->left, expected_cols, loop_depth, switch_depth);
        validate_matrix_init(node->right, expected_cols, loop_depth, switch_depth);
        return;
    }
    if (node->type && strcmp(node->type, "init_row") == 0) {
        int cols = count_init_list(node->left);
        if (expected_cols > 0 && cols != expected_cols) {
            semantic_error("matrix initializer row has %d columns, expected %d", cols, expected_cols);
        }
        validate_init_list(node->left, loop_depth, switch_depth);
        return;
    }
}

static int parse_char_literal(const char *text, long long *value)
{
    if (!text || !value) {
        return 0;
    }

    if (strlen(text) < 3 || text[0] != '\'' || text[strlen(text) - 1] != '\'') {
        return 0;
    }

    if (text[1] == '\\') {
        switch (text[2]) {
            case 'n':
                *value = '\n';
                return 1;
            case 't':
                *value = '\t';
                return 1;
            case 'r':
                *value = '\r';
                return 1;
            case '\\':
                *value = '\\';
                return 1;
            case '\'':
                *value = '\'';
                return 1;
            case '0':
                *value = '\0';
                return 1;
            default:
                return 0;
        }
    }

    *value = (unsigned char) text[1];
    return 1;
}

static ExprInfo analyze_expression(ASTNode *node, int loop_depth, int switch_depth);

static ExprInfo symbol_to_expr(Symbol *sym)
{
    ExprInfo info;

    if (!sym) {
        return invalid_info();
    }

    info = make_info(sym->type);
    info.size = sym->size;
    info.rows = sym->rows;
    info.cols = sym->cols;
    return info;
}

static void check_array_bounds_if_constant(const char *name, ExprInfo index_info, int size)
{
    if (!index_info.has_const_int || size < 0) {
        return;
    }

    if (index_info.const_int < 0 || index_info.const_int >= size) {
        semantic_error("compile-time bounds check failed: %s[%lld] with size %d", name, index_info.const_int, size);
    }
}

static void check_matrix_bounds_if_constant(const char *name, ExprInfo row_info, ExprInfo col_info, int rows, int cols)
{
    if (row_info.has_const_int && rows >= 0) {
        if (row_info.const_int < 0 || row_info.const_int >= rows) {
            semantic_error("compile-time bounds check failed: %s row index %lld out of [0,%d)", name, row_info.const_int, rows);
        }
    }

    if (col_info.has_const_int && cols >= 0) {
        if (col_info.const_int < 0 || col_info.const_int >= cols) {
            semantic_error("compile-time bounds check failed: %s column index %lld out of [0,%d)", name, col_info.const_int, cols);
        }
    }
}

static ExprInfo analyze_lvalue(ASTNode *node, int loop_depth, int switch_depth)
{
    Symbol *sym;
    ExprInfo index_info;
    ExprInfo row_info;
    ExprInfo col_info;

    if (!node || !node->type) {
        return invalid_info();
    }

    if (strcmp(node->type, "id") == 0) {
        sym = lookup_symbol(node->value);
        if (!sym) {
            semantic_error("use of undeclared identifier '%s'", node->value ? node->value : "<null>");
            return invalid_info();
        }
        return symbol_to_expr(sym);
    }

    if (strcmp(node->type, "array_access") == 0) {
        sym = lookup_symbol(node->value);
        if (!sym) {
            semantic_error("use of undeclared array '%s'", node->value ? node->value : "<null>");
            return invalid_info();
        }
        if (sym->type != SYM_ARRAY) {
            semantic_error("'%s' is not an array", node->value);
            return invalid_info();
        }

        index_info = analyze_expression(node->left, loop_depth, switch_depth);
        if (index_info.type != SYM_INVALID && !is_int_like(index_info.type)) {
            semantic_error("array index for '%s' must be int/char", node->value);
        }
        check_array_bounds_if_constant(node->value, index_info, sym->size);
        return make_info(SYM_INT);
    }

    if (strcmp(node->type, "matrix_access") == 0) {
        sym = lookup_symbol(node->value);
        if (!sym) {
            semantic_error("use of undeclared matrix '%s'", node->value ? node->value : "<null>");
            return invalid_info();
        }
        if (sym->type != SYM_MATRIX) {
            semantic_error("'%s' is not a matrix", node->value);
            return invalid_info();
        }

        row_info = analyze_expression(node->left, loop_depth, switch_depth);
        col_info = analyze_expression(node->right, loop_depth, switch_depth);

        if (row_info.type != SYM_INVALID && !is_int_like(row_info.type)) {
            semantic_error("row index for matrix '%s' must be int/char", node->value);
        }
        if (col_info.type != SYM_INVALID && !is_int_like(col_info.type)) {
            semantic_error("column index for matrix '%s' must be int/char", node->value);
        }

        check_matrix_bounds_if_constant(node->value, row_info, col_info, sym->rows, sym->cols);
        return make_info(SYM_INT);
    }

    semantic_error("invalid lvalue expression");
    return invalid_info();
}

static ExprInfo analyze_assignment(ASTNode *node, int loop_depth, int switch_depth)
{
    ExprInfo lhs;
    ExprInfo rhs;
    Symbol *sym;

    if (!node || !node->type || strcmp(node->type, "assign") != 0) {
        return invalid_info();
    }

    if (node->value) {
        sym = lookup_symbol(node->value);
        if (!sym) {
            semantic_error("assignment to undeclared identifier '%s'", node->value);
            lhs = invalid_info();
        } else {
            lhs = symbol_to_expr(sym);
        }
        rhs = analyze_expression(node->left, loop_depth, switch_depth);
    } else {
        lhs = analyze_lvalue(node->left, loop_depth, switch_depth);
        rhs = analyze_expression(node->right, loop_depth, switch_depth);
    }

    if (!is_assignable(lhs.type, rhs.type)) {
        semantic_error("type mismatch in assignment: cannot assign %s to %s", type_name(rhs.type), type_name(lhs.type));
        return lhs;
    }

    if (lhs.type == SYM_ARRAY && rhs.type == SYM_ARRAY && lhs.size >= 0 && rhs.size >= 0 && lhs.size != rhs.size) {
        semantic_error("array size mismatch in assignment (%d vs %d)", lhs.size, rhs.size);
    }

    if (lhs.type == SYM_MATRIX && rhs.type == SYM_MATRIX &&
        lhs.rows >= 0 && lhs.cols >= 0 && rhs.rows >= 0 && rhs.cols >= 0 &&
        (lhs.rows != rhs.rows || lhs.cols != rhs.cols)) {
        semantic_error("matrix shape mismatch in assignment (%dx%d vs %dx%d)", lhs.rows, lhs.cols, rhs.rows, rhs.cols);
    }

    return lhs;
}

static SymbolType numeric_result_type(SymbolType left, SymbolType right)
{
    if (left == SYM_DECIMAL || right == SYM_DECIMAL) {
        return SYM_DECIMAL;
    }
    return SYM_INT;
}

static ExprInfo analyze_expression(ASTNode *node, int loop_depth, int switch_depth)
{
    ExprInfo left;
    ExprInfo right;
    ExprInfo result;
    long long literal_value;

    if (!node || !node->type) {
        return invalid_info();
    }

    if (strcmp(node->type, "int") == 0) {
        if (parse_int_literal(node->value, &literal_value)) {
            return int_const_info(literal_value);
        }
        return make_info(SYM_INT);
    }

    if (strcmp(node->type, "char") == 0) {
        result = make_info(SYM_CHAR);
        if (parse_char_literal(node->value, &literal_value)) {
            result.has_const_int = 1;
            result.const_int = literal_value;
        }
        return result;
    }

    if (strcmp(node->type, "decimal") == 0) {
        return make_info(SYM_DECIMAL);
    }

    if (strcmp(node->type, "string") == 0) {
        return make_info(SYM_STRING);
    }

    if (strcmp(node->type, "id") == 0 || strcmp(node->type, "array_access") == 0 || strcmp(node->type, "matrix_access") == 0) {
        return analyze_lvalue(node, loop_depth, switch_depth);
    }

    if (strcmp(node->type, "assign") == 0) {
        return analyze_assignment(node, loop_depth, switch_depth);
    }

    if (strcmp(node->type, "+") == 0 || strcmp(node->type, "-") == 0 || strcmp(node->type, "*") == 0 || strcmp(node->type, "/") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        right = analyze_expression(node->right, loop_depth, switch_depth);

        if (left.type == SYM_INVALID || right.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_numeric_scalar(left.type) || !is_numeric_scalar(right.type)) {
            semantic_error("arithmetic operators require numeric operands");
            return invalid_info();
        }

        result = make_info(numeric_result_type(left.type, right.type));
        if (strcmp(node->type, "/") == 0 && right.has_const_int && right.const_int == 0) {
            semantic_error("possible runtime crash: division by zero");
        }
        if ((strcmp(node->type, "/") != 0) && left.has_const_int && right.has_const_int) {
            result.has_const_int = 1;
            if (strcmp(node->type, "+") == 0) {
                result.const_int = left.const_int + right.const_int;
            } else if (strcmp(node->type, "-") == 0) {
                result.const_int = left.const_int - right.const_int;
            } else {
                result.const_int = left.const_int * right.const_int;
            }
        }
        return result;
    }

    if (strcmp(node->type, "%") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        right = analyze_expression(node->right, loop_depth, switch_depth);

        if (left.type == SYM_INVALID || right.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_int_like(left.type) || !is_int_like(right.type)) {
            semantic_error("modulo operator requires int/char operands");
            return invalid_info();
        }
        if (right.has_const_int && right.const_int == 0) {
            semantic_error("possible runtime crash: modulo by zero");
        }

        result = make_info(SYM_INT);
        if (left.has_const_int && right.has_const_int && right.const_int != 0) {
            result.has_const_int = 1;
            result.const_int = left.const_int % right.const_int;
        }
        return result;
    }

    if (strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0 ||
        strcmp(node->type, "lt") == 0 || strcmp(node->type, "gt") == 0 ||
        strcmp(node->type, "le") == 0 || strcmp(node->type, "ge") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        right = analyze_expression(node->right, loop_depth, switch_depth);

        if (left.type == SYM_INVALID || right.type == SYM_INVALID) {
            return invalid_info();
        }

        if (strcmp(node->type, "eq") == 0 || strcmp(node->type, "ne") == 0) {
            if (!(left.type == right.type || (is_numeric_scalar(left.type) && is_numeric_scalar(right.type)))) {
                semantic_error("incompatible operands for equality comparison");
            }
        } else {
            if (!is_numeric_scalar(left.type) || !is_numeric_scalar(right.type)) {
                semantic_error("relational operators require numeric operands");
            }
        }
        result = make_info(SYM_INT);
        if (left.has_const_int && right.has_const_int) {
            result.has_const_int = 1;
            if (strcmp(node->type, "eq") == 0) {
                result.const_int = (left.const_int == right.const_int);
            } else if (strcmp(node->type, "ne") == 0) {
                result.const_int = (left.const_int != right.const_int);
            } else if (strcmp(node->type, "lt") == 0) {
                result.const_int = (left.const_int < right.const_int);
            } else if (strcmp(node->type, "gt") == 0) {
                result.const_int = (left.const_int > right.const_int);
            } else if (strcmp(node->type, "le") == 0) {
                result.const_int = (left.const_int <= right.const_int);
            } else {
                result.const_int = (left.const_int >= right.const_int);
            }
        }
        return result;
    }

    if (strcmp(node->type, "and") == 0 || strcmp(node->type, "or") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        right = analyze_expression(node->right, loop_depth, switch_depth);

        if (left.type == SYM_INVALID || right.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_scalar(left.type) || !is_scalar(right.type)) {
            semantic_error("logical operators require scalar operands");
        }
        result = make_info(SYM_INT);
        if (left.has_const_int && right.has_const_int) {
            result.has_const_int = 1;
            if (strcmp(node->type, "and") == 0) {
                result.const_int = (left.const_int != 0 && right.const_int != 0);
            } else {
                result.const_int = (left.const_int != 0 || right.const_int != 0);
            }
        }
        return result;
    }

    if (strcmp(node->type, "not") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_scalar(left.type)) {
            semantic_error("logical NOT requires scalar operand");
        }
        result = make_info(SYM_INT);
        if (left.has_const_int) {
            result.has_const_int = 1;
            result.const_int = (left.const_int == 0);
        }
        return result;
    }

    if (strcmp(node->type, "uminus") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_numeric_scalar(left.type)) {
            semantic_error("unary minus requires numeric operand");
            return invalid_info();
        }
        if (left.has_const_int) {
            return int_const_info(-left.const_int);
        }
        return make_info(left.type == SYM_DECIMAL ? SYM_DECIMAL : SYM_INT);
    }

    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        if (!node->left || !node->left->type ||
            (strcmp(node->left->type, "id") != 0 && strcmp(node->left->type, "array_access") != 0 && strcmp(node->left->type, "matrix_access") != 0)) {
            semantic_error("increment/decrement requires assignable operand");
            return invalid_info();
        }
        left = analyze_lvalue(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (!is_numeric_scalar(left.type)) {
            semantic_error("increment/decrement requires numeric operand");
            return invalid_info();
        }
        return left;
    }

    if (strcmp(node->type, "size") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (left.type != SYM_ARRAY && left.type != SYM_MATRIX) {
            semantic_error(".size is valid only for arrays or matrices");
            return invalid_info();
        }
        return make_info(SYM_INT);
    }

    if (strcmp(node->type, "sort") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (left.type != SYM_ARRAY) {
            semantic_error(".sort is valid only for arrays");
            return invalid_info();
        }
        result = make_info(SYM_ARRAY);
        result.size = left.size;
        return result;
    }

    if (strcmp(node->type, "transpose") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (left.type != SYM_MATRIX) {
            semantic_error(".T is valid only for matrices");
            return invalid_info();
        }
        result = make_info(SYM_MATRIX);
        result.rows = left.cols;
        result.cols = left.rows;
        return result;
    }

    if (strcmp(node->type, "det") == 0 || strcmp(node->type, "inv") == 0 || strcmp(node->type, "shape") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        if (left.type == SYM_INVALID) {
            return invalid_info();
        }
        if (left.type != SYM_MATRIX) {
            if (strcmp(node->type, "shape") == 0) {
                semantic_error(".shape is valid only for matrices");
            } else if (strcmp(node->type, "det") == 0) {
                semantic_error(".det is valid only for matrices");
            } else {
                semantic_error(".inv is valid only for matrices");
            }
            return invalid_info();
        }

        if ((strcmp(node->type, "det") == 0 || strcmp(node->type, "inv") == 0) &&
            left.rows >= 0 && left.cols >= 0 && left.rows != left.cols) {
            semantic_error(".%s requires a square matrix but got %dx%d", strcmp(node->type, "det") == 0 ? "det" : "inv", left.rows, left.cols);
        }

        if (strcmp(node->type, "det") == 0) {
            return make_info(SYM_DECIMAL);
        }
        if (strcmp(node->type, "shape") == 0) {
            return make_info(SYM_STRING);
        }

        result = make_info(SYM_MATRIX);
        result.rows = left.rows;
        result.cols = left.cols;
        return result;
    }

    if (strcmp(node->type, "matadd") == 0 || strcmp(node->type, "matsub") == 0 || strcmp(node->type, "matmul") == 0) {
        left = analyze_expression(node->left, loop_depth, switch_depth);
        right = analyze_expression(node->right, loop_depth, switch_depth);

        if (left.type == SYM_INVALID || right.type == SYM_INVALID) {
            return invalid_info();
        }
        if (left.type != SYM_MATRIX || right.type != SYM_MATRIX) {
            semantic_error("matrix operations require matrix operands");
            return invalid_info();
        }

        result = make_info(SYM_MATRIX);
        if (strcmp(node->type, "matmul") == 0) {
            if (left.cols >= 0 && right.rows >= 0 && left.cols != right.rows) {
                semantic_error("matrix shape mismatch for multiplication (%dx%d) * (%dx%d)", left.rows, left.cols, right.rows, right.cols);
            }
            result.rows = left.rows;
            result.cols = right.cols;
        } else {
            if (left.rows >= 0 && left.cols >= 0 && right.rows >= 0 && right.cols >= 0 &&
                (left.rows != right.rows || left.cols != right.cols)) {
                semantic_error("matrix shape mismatch for %s (%dx%d vs %dx%d)", strcmp(node->type, "matadd") == 0 ? "addition" : "subtraction", left.rows, left.cols, right.rows, right.cols);
            }
            result.rows = left.rows;
            result.cols = left.cols;
        }
        return result;
    }

    semantic_error("unsupported expression node '%s'", node->type);
    return invalid_info();
}

static void analyze_node(ASTNode *node, int loop_depth, int switch_depth)
{
    ExprInfo expr;
    SymbolType decl_type;
    long long declared_extent;

    if (!node || !node->type) {
        return;
    }

    if (strcmp(node->type, "program") == 0 || strcmp(node->type, "global") == 0 ||
        strcmp(node->type, "statements") == 0 || strcmp(node->type, "cases") == 0 ||
        strcmp(node->type, "print_args") == 0) {
        analyze_node(node->left, loop_depth, switch_depth);
        analyze_node(node->right, loop_depth, switch_depth);
        analyze_node(node->third, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "main_block") == 0 || strcmp(node->type, "block") == 0) {
        enter_scope();
        analyze_node(node->left, loop_depth, switch_depth);
        analyze_node(node->right, loop_depth, switch_depth);
        analyze_node(node->third, loop_depth, switch_depth);
        exit_scope();
        return;
    }

    if (strcmp(node->type, "decl") == 0) {
        decl_type = parse_declared_scalar_type(node->left ? node->left->value : NULL);
        declare_symbol(node->right ? node->right->value : NULL, decl_type, -1, -1, -1);
        return;
    }

    if (strcmp(node->type, "decl_assign") == 0) {
        decl_type = parse_declared_scalar_type(node->left ? node->left->value : NULL);
        declare_symbol(node->right ? node->right->value : NULL, decl_type, -1, -1, -1);
        expr = analyze_expression(node->third, loop_depth, switch_depth);
        if (!is_assignable(decl_type, expr.type)) {
            semantic_error("type mismatch in initialization of '%s'", node->right ? node->right->value : "<null>");
        }
        return;
    }

    if (strcmp(node->type, "array_decl") == 0) {
        if (!parse_int_literal(node->left ? node->left->value : NULL, &declared_extent)) {
            semantic_error("invalid array size for '%s'", node->value ? node->value : "<null>");
            declare_symbol(node->value, SYM_ARRAY, -1, -1, -1);
            return;
        }

        declare_symbol(node->value, SYM_ARRAY, (int) declared_extent, -1, -1);
        if (declared_extent <= 0) {
            semantic_error("array '%s' size must be positive", node->value);
        }
        if (node->third) {
            int count = count_init_list(node->third);
            if (declared_extent > 0 && count != declared_extent) {
                semantic_error("array initializer for '%s' has %d elements, expected %lld",
                               node->value ? node->value : "<null>", count, declared_extent);
            }
            validate_init_list(node->third, loop_depth, switch_depth);
        }
        return;
    }

    if (strcmp(node->type, "matrix_decl") == 0) {
        long long row_count;
        long long col_count;

        if (!parse_int_literal(node->left ? node->left->value : NULL, &row_count) ||
            !parse_int_literal(node->right ? node->right->value : NULL, &col_count)) {
            semantic_error("invalid matrix dimensions for '%s'", node->value ? node->value : "<null>");
            declare_symbol(node->value, SYM_MATRIX, -1, -1, -1);
            return;
        }

        declare_symbol(node->value, SYM_MATRIX, -1, (int) row_count, (int) col_count);

        if (row_count <= 0 || col_count <= 0) {
            semantic_error("matrix '%s' dimensions must be positive", node->value);
        }
        if (node->third) {
            int rows = count_row_list(node->third);
            if (row_count > 0 && rows != row_count) {
                semantic_error("matrix initializer for '%s' has %d rows, expected %lld",
                               node->value ? node->value : "<null>", rows, row_count);
            }
            validate_matrix_init(node->third, (int) col_count, loop_depth, switch_depth);
        }
        return;
    }

    if (strcmp(node->type, "assign") == 0) {
        analyze_assignment(node, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "if") == 0) {
        expr = analyze_expression(node->left, loop_depth, switch_depth);
        if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
            semantic_error("if condition must be scalar");
        }
        if (expr.has_const_int) {
            semantic_error("if condition is always %s", expr.const_int ? "true" : "false");
        }
        analyze_node(node->right, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "if_else") == 0) {
        if (node->left && node->left->type && strcmp(node->left->type, "if") == 0) {
            analyze_node(node->left, loop_depth, switch_depth);
        } else {
            expr = analyze_expression(node->left, loop_depth, switch_depth);
            if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
                semantic_error("if condition must be scalar");
            }
            if (expr.has_const_int) {
                semantic_error("if condition is always %s", expr.const_int ? "true" : "false");
            }
            analyze_node(node->right, loop_depth, switch_depth);
            analyze_node(node->third, loop_depth, switch_depth);
        }
        return;
    }

    if (strcmp(node->type, "for_body") == 0) {
        analyze_node(node->left, loop_depth, switch_depth);
        analyze_node(node->right, loop_depth + 1, switch_depth);
        return;
    }

    if (strcmp(node->type, "for") == 0) {
        analyze_expression(node->left, loop_depth, switch_depth);
        expr = analyze_expression(node->right, loop_depth, switch_depth);
        if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
            semantic_error("for condition must be scalar");
        }
        if (expr.has_const_int && expr.const_int == 0) {
            semantic_error("for-loop condition is always false; loop never executes");
        }
        analyze_expression(node->third, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "while") == 0) {
        expr = analyze_expression(node->left, loop_depth, switch_depth);
        if (expr.type != SYM_INVALID && !is_scalar(expr.type)) {
            semantic_error("while condition must be scalar");
        }
        if (expr.has_const_int && expr.const_int == 0) {
            semantic_error("while-loop condition is always false; loop never executes");
        }
        analyze_node(node->right, loop_depth + 1, switch_depth);
        return;
    }

    if (strcmp(node->type, "switch") == 0) {
        expr = analyze_expression(node->left, loop_depth, switch_depth);
        if (expr.type != SYM_INVALID && !is_int_like(expr.type)) {
            semantic_error("switch expression must be int/char");
        }
        analyze_node(node->right, loop_depth, switch_depth + 1);
        return;
    }

    if (strcmp(node->type, "case") == 0) {
        analyze_expression(node->left, loop_depth, switch_depth);
        analyze_node(node->right, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "default") == 0) {
        analyze_node(node->left, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "break") == 0) {
        if (loop_depth == 0 && switch_depth == 0) {
            semantic_error("break used outside loop/switch");
        }
        return;
    }

    if (strcmp(node->type, "continue") == 0) {
        if (loop_depth == 0) {
            semantic_error("continue used outside loop");
        }
        return;
    }

    if (strcmp(node->type, "return") == 0) {
        if (node->left) {
            analyze_expression(node->left, loop_depth, switch_depth);
        }
        return;
    }

    if (strcmp(node->type, "print") == 0) {
        analyze_node(node->left, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "scan") == 0) {
        analyze_lvalue(node->left, loop_depth, switch_depth);
        return;
    }

    if (strcmp(node->type, "string") == 0 || strcmp(node->type, "empty") == 0) {
        return;
    }

    if (strcmp(node->type, "pre_inc") == 0 || strcmp(node->type, "pre_dec") == 0 ||
        strcmp(node->type, "post_inc") == 0 || strcmp(node->type, "post_dec") == 0) {
        analyze_expression(node, loop_depth, switch_depth);
        return;
    }

    analyze_expression(node, loop_depth, switch_depth);
}

int semantic_analysis(ASTNode *root_node)
{
    semantic_errors = 0;
    enter_scope();
    analyze_node(root_node, 0, 0);
    exit_scope();
    return semantic_errors == 0;
}

int semantic_error_count(void)
{
    return semantic_errors;
}
