%code requires {
    #include "ast.h"
}

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"

    extern int line_no;
    extern int yylex();
    void yyerror(const char *s);

    ASTNode* root;
%}


/* ---------- Token Definitions ---------- */
%token KEYWORD_INT KEYWORD_DECIMAL KEYWORD_CHAR KEYWORD_STRING KEYWORD_MATRIX KEYWORD_ARR
%token KEYWORD_MAIN KEYWORD_IF KEYWORD_ELSE
%token KEYWORD_FOR KEYWORD_WHILE
%token KEYWORD_SWITCH KEYWORD_CASE KEYWORD_DEFAULT
%token KEYWORD_BREAK KEYWORD_CONTINUE
%token KEYWORD_RETURN
%token KEYWORD_PRINT KEYWORD_SCAN
%token OP_EQ OP_NE OP_LE OP_GE OP_LT OP_GT
%token OP_PLUS OP_MINUS OP_MUL OP_DIV OP_MOD OP_INC OP_DEC
%token OP_AND OP_OR OP_NOT
%token OP_ASSIGN
%token OP_SIZE OP_SORT
%token OP_MATADD OP_MATSUB OP_MATMUL 
%token OP_MATTRANSPOSE OP_MATDET OP_MATINV OP_MATSHAPE 
%token DELIM_SEMI DELIM_COMMA DELIM_COLON
%token DELIM_LPAREN DELIM_RPAREN
%token DELIM_LBRACE DELIM_RBRACE
%token DELIM_LBRACK DELIM_RBRACK
%token <str> ID INT_CONST DECIMAL_CONST CHAR_CONST STRING_CONST

%union {
    ASTNode *node;
    char *str;
}

%type <node> program
%type <node> global_declarations
%type <node> global_declaration
%type <node> main_block
%type <node> block
%type <node> statements
%type <node> statement
%type <node> declaration
%type <node> assignment
%type <node> assignment_expr
%type <node> inc_dec_expr
%type <node> if_statement
%type <node> for_loop
%type <node> while_loop
%type <node> switch_statement
%type <node> case_list_opt
%type <node> case_list
%type <node> case_statement
%type <node> default_case
%type <node> break_statement
%type <node> continue_statement
%type <node> return_statement
%type <node> print_statement
%type <node> print_args
%type <node> print_arg
%type <node> scan_statement
%type <node> lvalue
%type <node> expression
%type <node> logical_or_expression
%type <node> logical_and_expression
%type <node> equality_expression
%type <node> relational_expression
%type <node> additive_expression
%type <node> multiplicative_expression
%type <node> unary_expression
%type <node> postfix_expression
%type <node> primary_expression
%type <node> constant
%type <str> type_specifier
%type <node> delim_statement

/* ---------- Operator Precedence ---------- */
%right OP_ASSIGN
%left OP_OR
%left OP_AND
%left OP_EQ OP_NE
%left OP_LT OP_GT OP_LE OP_GE
%left OP_PLUS OP_MINUS
%left OP_MUL OP_DIV OP_MOD 
%right OP_NOT UMINUS
%left DELIM_LBRACK DELIM_RBRACK

/* ---------- Grammar Rules ---------- */
%%

program:
    global_declarations KEYWORD_INT KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN main_block
    { 
        $$ = createNode("program", NULL, $1, $6, NULL); 
        root = $$;  
    }

    | KEYWORD_INT KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block
    { 
        $$ = createNode("program", NULL, NULL, $5, NULL); 
        root = $$;
    }

    ;

global_declarations:
    global_declaration  
    { $$ = $1; }

    | global_declarations global_declaration
    { $$ = createNode("global", NULL, $1, $2, NULL); }
    ;

/*Only allowing matrix and array keywords for declaration*/
global_declaration:
    type_specifier ID DELIM_SEMI
    {
        $$ = createNode("decl", NULL,
                createNode("type", $1, NULL, NULL, NULL),
                createNode("id", $2, NULL, NULL, NULL), NULL);
    }

    | type_specifier ID OP_ASSIGN constant DELIM_SEMI
    {
        $$ = createNode("decl_assign", NULL,
                createNode("type", $1, NULL, NULL, NULL),
                createNode("id", $2, NULL, NULL, NULL), $4);
    }

    | KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    {
        $$ = createNode("matrix_decl", $2,
                createNode("rows", $4, NULL, NULL, NULL),
                createNode("cols", $7, NULL, NULL, NULL), NULL);
    }
    
    | KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    {
        $$ = createNode("array_decl", $2,
                createNode("size", $4, NULL, NULL, NULL), NULL, NULL);
    }

    | ID OP_ASSIGN expression DELIM_SEMI
    { $$ = createNode("assign", $1, $3, NULL, NULL); }
    ;

type_specifier:
    KEYWORD_INT         { $$ = "int"; }
    | KEYWORD_DECIMAL   { $$ = "decimal"; }
    | KEYWORD_CHAR      { $$ = "char"; }
    | KEYWORD_STRING    { $$ = "string"; }
    ;

main_block:
    DELIM_LBRACE statements return_statement DELIM_RBRACE
    { $$ = createNode("main_block", NULL, $2, $3, NULL); }

    | DELIM_LBRACE return_statement DELIM_RBRACE
    { $$ = createNode("main_block", NULL, NULL, $2, NULL); }
    ;
    ;

block:
    DELIM_LBRACE statements DELIM_RBRACE
    { $$ = createNode("block", NULL, $2, NULL, NULL); }

    | DELIM_LBRACE DELIM_RBRACE
    { $$ = createNode("block", NULL, NULL, NULL, NULL); }
    ;

statements:
    statement   { $$ = $1; }

    | statements statement 
    { $$ = createNode("statements", NULL, $1, $2, NULL); }
    ;

statement:
    declaration 
    | assignment
    | inc_dec_expr
    | if_statement
    | for_loop
    | while_loop
    | switch_statement
    | break_statement
    | continue_statement
    | print_statement
    | scan_statement
    | return_statement
    | delim_statement
    | block
    ;

delim_statement:
    DELIM_SEMI
    { $$ = createNode("empty", NULL, NULL, NULL, NULL); }
    ;

declaration:
    type_specifier ID DELIM_SEMI
    {
        $$ = createNode("decl", NULL,
                createNode("type", $1, NULL, NULL, NULL),
                createNode("id", $2, NULL, NULL, NULL), NULL);
    }

    | type_specifier ID OP_ASSIGN expression DELIM_SEMI
    {
        $$ = createNode("decl_assign", NULL,
                createNode("type", $1, NULL, NULL, NULL),
                createNode("id", $2, NULL, NULL, NULL), $4);
    }

    | KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    {
        $$ = createNode("matrix_decl", $2,
                createNode("rows", $4, NULL, NULL, NULL),
                createNode("cols", $7, NULL, NULL, NULL), NULL);
    }

    | KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    {
        $$ = createNode("array_decl", $2,
                createNode("size", $4, NULL, NULL, NULL), NULL, NULL);
    }
    ;


assignment:
    ID OP_ASSIGN expression DELIM_SEMI 
    { $$ = createNode("assign", $1, $3, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI
    {
        $$ = createNode("assign", NULL,
                createNode("array_access", $1, $3, NULL, NULL), $6, NULL);
    }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI
    {
        $$ = createNode("assign", NULL,
                createNode("matrix_access", $1, $3, $6, NULL), $9, NULL);
    }
    ;

if_statement:
    KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block
    { $$ = createNode("if", NULL, $3, $5, NULL); }

    | KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE block
    { $$ = createNode("if_else", NULL, $3, $5, $7); }

    | KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE if_statement
    {
        $$ = createNode("if_else", NULL,
                createNode("if", NULL, $3, $5, NULL), $7, NULL);
    }
    ;

for_loop:
    KEYWORD_FOR DELIM_LPAREN assignment_expr DELIM_SEMI expression DELIM_SEMI assignment_expr DELIM_RPAREN block
    {
        $$ = createNode("for_body", NULL, 
                createNode("for", NULL, $3, $5, $7), $9, NULL); 
    }
    ;

while_loop:
    KEYWORD_WHILE DELIM_LPAREN expression DELIM_RPAREN block
    { $$ = createNode("while", NULL, $3, $5, NULL); }
    ;

switch_statement:
    KEYWORD_SWITCH DELIM_LPAREN expression DELIM_RPAREN DELIM_LBRACE case_list_opt DELIM_RBRACE
    { $$ = createNode("switch", NULL, $3, $6, NULL); }
    ;

case_list_opt:
    /* empty */
    { $$ = NULL; }

    | case_list
    { $$ = $1; }
    ;

case_list:
    case_statement
    { $$ = $1; }

    | case_list case_statement
    { $$ = createNode("cases", NULL, $1, $2, NULL); }

    | case_list default_case
    { $$ = createNode("cases", NULL, $1, $2, NULL); }
    ;

case_statement:
    KEYWORD_CASE constant DELIM_COLON statements
    { $$ = createNode("case", NULL, $2, $4, NULL); }
    ;

default_case:
    KEYWORD_DEFAULT DELIM_COLON statements
    { $$ = createNode("default", NULL, $3, NULL, NULL); }
    ;


break_statement:
    KEYWORD_BREAK DELIM_SEMI
    { $$ = createNode("break", NULL, NULL, NULL, NULL); }
    ;

continue_statement:
    KEYWORD_CONTINUE DELIM_SEMI
    { $$ = createNode("continue", NULL, NULL, NULL, NULL); }
    ;

return_statement:
    KEYWORD_RETURN expression DELIM_SEMI
    { $$ = createNode("return", NULL, $2, NULL, NULL); }

    | KEYWORD_RETURN DELIM_SEMI
    { $$ = createNode("return", NULL, NULL, NULL, NULL); }
    ;

print_statement:
    KEYWORD_PRINT DELIM_LPAREN print_args DELIM_RPAREN DELIM_SEMI
    { $$ = createNode("print", NULL, $3, NULL, NULL); }
    ;

print_args:
    print_arg
    { $$ = $1; }

    | print_args DELIM_COMMA print_arg
    { $$ = createNode("print_args", NULL, $1, $3, NULL); }
    ;

print_arg:
    STRING_CONST
    { $$ = createNode("string", $1, NULL, NULL, NULL); }

    | ID
    { $$ = createNode("id", $1, NULL, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("array_access", $1, $3, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("matrix_access", $1, $3, $6, NULL); }
    ;

scan_statement:
    KEYWORD_SCAN DELIM_LPAREN lvalue DELIM_RPAREN DELIM_SEMI
    { $$ = createNode("scan", NULL, $3, NULL, NULL); }
    ;

lvalue:
    ID
    { $$ = createNode("id", $1, NULL, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("array_access", $1, $3, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("matrix_access", $1, $3, $6, NULL); }
    ;

assignment_expr:
    ID OP_ASSIGN expression
    {
        $$ = createNode("assign", NULL,
                createNode("id", $1, NULL, NULL, NULL), $3, NULL);
    }

    | ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression
    {
        $$ = createNode("assign", NULL,
                createNode("array_access", $1, $3, NULL, NULL), $6, NULL);
    }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression
    {
        $$ = createNode("assign", NULL,
                createNode("matrix_access", $1, $3, $6, NULL), $9, NULL);
    }

    | inc_dec_expr
    { $$ = $1; }
    ;

inc_dec_expr:
    /* Pre-increment / pre-decrement */
      OP_INC ID
    { $$ = createNode("pre_inc", NULL, createNode("id", $2, NULL, NULL, NULL), NULL, NULL); }

    | OP_DEC ID
    { $$ = createNode("pre_dec", NULL, createNode("id", $2, NULL, NULL, NULL), NULL, NULL); }

    | OP_INC ID DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("pre_inc", NULL, createNode("array_access", $2, $4, NULL, NULL), NULL, NULL); }

    | OP_DEC ID DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("pre_dec", NULL, createNode("array_access", $2, $4, NULL, NULL), NULL, NULL); }

    | OP_INC ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("pre_inc", NULL, createNode("matrix_access", $2, $4, $7, NULL), NULL, NULL); }

    | OP_DEC ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("pre_dec", NULL, createNode("matrix_access", $2, $4, $7, NULL), NULL, NULL); }

    /* Post-increment / post-decrement */
    | ID OP_INC
    { $$ = createNode("post_inc", NULL, createNode("id", $1, NULL, NULL, NULL), NULL, NULL); }

    | ID OP_DEC
    { $$ = createNode("post_dec", NULL, createNode("id", $1, NULL, NULL, NULL), NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK OP_INC
    { $$ = createNode("post_inc", NULL, createNode("array_access", $1, $3, NULL, NULL), NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK OP_DEC
    { $$ = createNode("post_dec", NULL, createNode("array_access", $1, $3, NULL, NULL), NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_INC
    { $$ = createNode("post_inc", NULL, createNode("matrix_access", $1, $3, $6, NULL), NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_DEC
    { $$ = createNode("post_dec", NULL, createNode("matrix_access", $1, $3, $6, NULL), NULL, NULL); }
    ;


/* Divided the expression into sub expressions */
expression:
    logical_or_expression
    { $$ = $1; }
    ;

logical_or_expression:
    logical_and_expression
    { $$ = $1; }

    | logical_or_expression OP_OR logical_and_expression
    { $$ = createNode("or", NULL, $1, $3, NULL); }
    ;

logical_and_expression:
    equality_expression
    { $$ = $1; }
    | logical_and_expression OP_AND equality_expression
    { $$ = createNode("and", NULL, $1, $3, NULL); }
    ;

equality_expression:
    relational_expression
    { $$ = $1; }

    | equality_expression OP_EQ relational_expression
    { $$ = createNode("eq", NULL, $1, $3, NULL); }

    | equality_expression OP_NE relational_expression
    { $$ = createNode("ne", NULL, $1, $3, NULL); }
    ;

relational_expression:
    additive_expression
    { $$ = $1; }

    | relational_expression OP_LT additive_expression
    { $$ = createNode("lt", NULL, $1, $3, NULL); }

    | relational_expression OP_GT additive_expression
    { $$ = createNode("gt", NULL, $1, $3, NULL); }
    
    | relational_expression OP_LE additive_expression
    { $$ = createNode("le", NULL, $1, $3, NULL); }

    | relational_expression OP_GE additive_expression
    { $$ = createNode("ge", NULL, $1, $3, NULL); }
    ;

additive_expression:
    multiplicative_expression 
    { $$ = $1; }

    | additive_expression OP_PLUS multiplicative_expression
    { $$ = createNode("+", NULL, $1, $3, NULL); }

    | additive_expression OP_MINUS multiplicative_expression
    { $$ = createNode("-", NULL, $1, $3, NULL); }
    ;

multiplicative_expression:
    unary_expression
    { $$ = $1; }

    | multiplicative_expression OP_MUL unary_expression
    { $$ = createNode("*", NULL, $1, $3, NULL); }

    | multiplicative_expression OP_DIV unary_expression
    { $$ = createNode("/", NULL, $1, $3, NULL); }

    | multiplicative_expression OP_MOD unary_expression
    { $$ = createNode("%", NULL, $1, $3, NULL); }
    ;

unary_expression:
    postfix_expression   
    { $$ = $1; }

    | OP_NOT unary_expression
    { $$ = createNode("not", NULL, $2, NULL, NULL); }

    | OP_MINUS unary_expression %prec UMINUS
    { $$ = createNode("uminus", NULL, $2, NULL, NULL); }

    | OP_INC unary_expression
    { $$ = createNode("pre_inc", NULL, $2, NULL, NULL); }

    | OP_DEC unary_expression
    { $$ = createNode("pre_dec", NULL, $2, NULL, NULL); }
    ;

postfix_expression:
    primary_expression
    { $$ = $1; }

    | postfix_expression OP_SIZE
    { $$ = createNode("size", NULL, $1, NULL, NULL); }

    | postfix_expression OP_SORT
    { $$ = createNode("sort", NULL, $1, NULL, NULL); }

    | postfix_expression OP_MATTRANSPOSE
    { $$ = createNode("transpose", NULL, $1, NULL, NULL); }

    | postfix_expression OP_MATDET
    { $$ = createNode("det", NULL, $1, NULL, NULL); }

    | postfix_expression OP_MATINV
    { $$ = createNode("inv", NULL, $1, NULL, NULL); }

    | postfix_expression OP_MATSHAPE
    { $$ = createNode("shape", NULL, $1, NULL, NULL); }

    | postfix_expression OP_MATADD DELIM_LPAREN expression DELIM_RPAREN
    { $$ = createNode("matadd", NULL, $1, $4, NULL); }

    | postfix_expression OP_MATSUB DELIM_LPAREN expression DELIM_RPAREN
    { $$ = createNode("matsub", NULL, $1, $4, NULL); }

    | postfix_expression OP_MATMUL DELIM_LPAREN expression DELIM_RPAREN
    { $$ = createNode("matmul", NULL, $1, $4, NULL); }

    | postfix_expression OP_INC
    { $$ = createNode("post_inc", NULL, $1, NULL, NULL); }

    | postfix_expression OP_DEC
    { $$ = createNode("post_dec", NULL, $1, NULL, NULL); }
    ;

primary_expression:
    constant
    | ID
    { $$ = createNode("id", $1, NULL, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("array_access", $1, $3, NULL, NULL); }

    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    { $$ = createNode("matrix_access", $1, $3, $6, NULL); }

    | DELIM_LPAREN expression DELIM_RPAREN
    { $$ = $2; }
    ;

constant:
    INT_CONST
    { $$ = createNode("int", $1, NULL, NULL, NULL); }

    | DECIMAL_CONST
    { $$ = createNode("decimal", $1, NULL, NULL, NULL); }

    | CHAR_CONST
    { $$ = createNode("char", $1, NULL, NULL, NULL); }

    | STRING_CONST
    { $$ = createNode("string", $1, NULL, NULL, NULL); }
    ;

%%

ASTNode* createNode(char *type, char *value, ASTNode *left, ASTNode *right, ASTNode *third)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;
    node->third = third;
    return node;
}

void printIndent(int level)
{
    for (int i = 0; i < level; i++)
        printf("|   ");
}

void printAST(ASTNode* node, int level)
{
    if (node == NULL)
        return;

    printIndent(level);

    if (node->value)
        printf("|-- %s (%s)\n", node->type, node->value);
    else
        printf("|-- %s\n", node->type);

    printAST(node->left, level + 1);
    printAST(node->right, level + 1);
    printAST(node->third, level + 1);
}


void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_no, s);
}

int main() {

   printf("Tokens Generated:\n");
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");
    } else {
        printf("Parsing failed.\n");
    }
    printAST(root, 0);
    return 0;
}
