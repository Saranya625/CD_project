%{
#include <stdio.h>
#include <stdlib.h>

extern int line_no;
extern int yylex();
void yyerror(const char *s);
%}

/* ---------- Token Definitions ---------- */
%token KEYWORD_MATRIX KEYWORD_ARR
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
%token INT_CONST DECIMAL_CONST CHAR_CONST STRING ID

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
    global_declarations KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block
    { printf("Valid program parsed successfully!\n"); }
    | KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block
    { printf("Valid program parsed successfully!\n"); }
    ;

global_declarations:
    global_declaration
    | global_declarations global_declaration
    ;

/*Only allowing matrix and array keywords for declaration*/
global_declaration:
    KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    | KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    | ID OP_ASSIGN expression DELIM_SEMI
    ;

block:
    DELIM_LBRACE statements DELIM_RBRACE
    | DELIM_LBRACE DELIM_RBRACE
    ;

statements:
    statement
    | statements statement
    ;

statement:
    declaration
    | assignment
    | if_statement
    | for_loop
    | while_loop
    | switch_statement
    | break_statement
    | continue_statement
    | return_statement
    | print_statement
    | scan_statement
    | block
    ;

declaration:
    KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    | KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI
    ;

assignment:
    ID OP_ASSIGN expression DELIM_SEMI
    | ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI
    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI
    ;

if_statement:
    KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block
    | KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE block
    | KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE if_statement
    ;

for_loop:
    KEYWORD_FOR DELIM_LPAREN assignment_expr DELIM_SEMI expression DELIM_SEMI assignment_expr DELIM_RPAREN block
    ;

while_loop:
    KEYWORD_WHILE DELIM_LPAREN expression DELIM_RPAREN block
    ;

switch_statement:
    KEYWORD_SWITCH DELIM_LPAREN expression DELIM_RPAREN DELIM_LBRACE case_list DELIM_RBRACE
    | KEYWORD_SWITCH DELIM_LPAREN expression DELIM_RPAREN DELIM_LBRACE case_list default_case DELIM_RBRACE
    ;

case_list:
    case_statement
    | case_list case_statement
    ;

case_statement:
    KEYWORD_CASE constant DELIM_COLON statements
    ;

default_case:
    KEYWORD_DEFAULT DELIM_COLON statements
    ;

break_statement:
    KEYWORD_BREAK DELIM_SEMI
    ;

continue_statement:
    KEYWORD_CONTINUE DELIM_SEMI
    ;

return_statement:
    KEYWORD_RETURN expression DELIM_SEMI
    | KEYWORD_RETURN DELIM_SEMI
    ;

print_statement:
    KEYWORD_PRINT DELIM_LPAREN print_args DELIM_RPAREN DELIM_SEMI
    ;

print_args:
    print_arg
    | print_args DELIM_COMMA print_arg
    ;

print_arg:
    expression
    | STRING
    ;

scan_statement:
    KEYWORD_SCAN DELIM_LPAREN lvalue DELIM_RPAREN DELIM_SEMI
    ;

lvalue:
    ID
    | ID DELIM_LBRACK expression DELIM_RBRACK
    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    ;

assignment_expr:
    ID OP_ASSIGN expression
    | ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression
    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression
    ;

/* Divided the expression into sub expressions */
expression:
    logical_or_expression
    ;

logical_or_expression:
      logical_and_expression
    | logical_or_expression OP_OR logical_and_expression
    ;

logical_and_expression:
      equality_expression
    | logical_and_expression OP_AND equality_expression
    ;

equality_expression:
      relational_expression
    | equality_expression OP_EQ relational_expression
    | equality_expression OP_NE relational_expression
    ;

relational_expression:
      additive_expression
    | relational_expression OP_LT additive_expression
    | relational_expression OP_GT additive_expression
    | relational_expression OP_LE additive_expression
    | relational_expression OP_GE additive_expression
    ;

additive_expression:
      multiplicative_expression
    | additive_expression OP_PLUS multiplicative_expression
    | additive_expression OP_MINUS multiplicative_expression
    ;

multiplicative_expression:
      unary_expression
    | multiplicative_expression OP_MUL unary_expression
    | multiplicative_expression OP_DIV unary_expression
    | multiplicative_expression OP_MOD unary_expression
    ;

unary_expression:
      postfix_expression
    | OP_NOT unary_expression
    | OP_MINUS unary_expression %prec UMINUS
    | OP_INC unary_expression
    | OP_DEC unary_expression
    ;

postfix_expression:
      primary_expression
    | postfix_expression OP_SIZE
    | postfix_expression OP_SORT
    | postfix_expression OP_MATTRANSPOSE
    | postfix_expression OP_MATDET
    | postfix_expression OP_MATINV
    | postfix_expression OP_MATSHAPE
    | postfix_expression OP_MATADD DELIM_LPAREN expression DELIM_RPAREN
    | postfix_expression OP_MATSUB DELIM_LPAREN expression DELIM_RPAREN
    | postfix_expression OP_MATMUL DELIM_LPAREN expression DELIM_RPAREN
    | postfix_expression OP_INC
    | postfix_expression OP_DEC
    ;

primary_expression:
      constant
    | ID
    | ID DELIM_LBRACK expression DELIM_RBRACK
    | ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK
    | DELIM_LPAREN expression DELIM_RPAREN
    ;

constant:
    INT_CONST
    | DECIMAL_CONST
    | CHAR_CONST
    ;

%%

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
    return 0;
}
