%{
#include "parser.tab.h"
#include <stdio.h>
int line_no = 1;

void print_token(const char* token_type, const char* lexeme) {
    printf("%-20s: %s\n", token_type, lexeme);
}
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INTC        {DIGIT}+
DECIMALC    {DIGIT}+\.{DIGIT}+
CHARC       '([^'\\]|\\.)'
STRING      \"([^\"\\]|\\.)*\"

%%
"array"     { print_token("KEYWORD_ARR", yytext); return KEYWORD_ARR; }
"matrix"    { print_token("KEYWORD_MATRIX", yytext); return KEYWORD_MATRIX; }
"main"      { print_token("KEYWORD_MAIN", yytext); return KEYWORD_MAIN; }
"if"        { print_token("KEYWORD_IF", yytext); return KEYWORD_IF; }
"else"      { print_token("KEYWORD_ELSE", yytext); return KEYWORD_ELSE; }
"for"       { print_token("KEYWORD_FOR", yytext); return KEYWORD_FOR; }
"while"     { print_token("KEYWORD_WHILE", yytext); return KEYWORD_WHILE; }
"switch"    { print_token("KEYWORD_SWITCH", yytext); return KEYWORD_SWITCH; }
"case"      { print_token("KEYWORD_CASE", yytext); return KEYWORD_CASE; }
"default"   { print_token("KEYWORD_DEFAULT", yytext); return KEYWORD_DEFAULT; }
"break"     { print_token("KEYWORD_BREAK", yytext); return KEYWORD_BREAK; }
"continue"  { print_token("KEYWORD_CONTINUE", yytext); return KEYWORD_CONTINUE; }
"return"    { print_token("KEYWORD_RETURN", yytext); return KEYWORD_RETURN; }
"print"     { print_token("KEYWORD_PRINT", yytext); return KEYWORD_PRINT; }
"scan"      { print_token("KEYWORD_SCAN", yytext); return KEYWORD_SCAN; }

"=="        { print_token("OP_EQ", yytext); return OP_EQ; }
"!="        { print_token("OP_NE", yytext); return OP_NE; }
"<="        { print_token("OP_LE", yytext); return OP_LE; }
">="        { print_token("OP_GE", yytext); return OP_GE; }
"<"         { print_token("OP_LT", yytext); return OP_LT; }
">"         { print_token("OP_GT", yytext); return OP_GT; }
"&&"        { print_token("OP_AND", yytext); return OP_AND; }
"||"        { print_token("OP_OR", yytext); return OP_OR; }
"!"         { print_token("OP_NOT", yytext); return OP_NOT; }
"++"        { print_token("OP_INC", yytext); return OP_INC; }
"--"        { print_token("OP_DEC", yytext); return OP_DEC; }
"+"         { print_token("OP_PLUS", yytext); return OP_PLUS; }
"-"         { print_token("OP_MINUS", yytext); return OP_MINUS; }
"*"         { print_token("OP_MUL", yytext); return OP_MUL; }
"/"         { print_token("OP_DIV", yytext); return OP_DIV; }
"%"         { print_token("OP_MOD", yytext); return OP_MOD; }
"="         { print_token("OP_ASSIGN", yytext); return OP_ASSIGN; }
".size"     { print_token("OP_SIZE", yytext); return OP_SIZE; }
".sort"     { print_token("OP_SORT", yytext); return OP_SORT; }
".add"      { print_token("OP_MATADD", yytext); return OP_MATADD; }
".sub"      { print_token("OP_MATSUB", yytext); return OP_MATSUB; }
".mul"      { print_token("OP_MATMUL", yytext); return OP_MATMUL; }
".det"      { print_token("OP_MATDET", yytext); return OP_MATDET; }
".inv"      { print_token("OP_MATINV", yytext); return OP_MATINV; }
".shape"    { print_token("OP_MATSHAPE", yytext); return OP_MATSHAPE; }
".T"        { print_token("OP_MATTRANSPOSE", yytext); return OP_MATTRANSPOSE; }

";"         { print_token("DELIM_SEMI", yytext); return DELIM_SEMI; }
","         { print_token("DELIM_COMMA", yytext); return DELIM_COMMA; }
":"         { print_token("DELIM_COLON", yytext); return DELIM_COLON; }
"("         { print_token("DELIM_LPAREN", yytext); return DELIM_LPAREN; }
")"         { print_token("DELIM_RPAREN", yytext); return DELIM_RPAREN; }
"{"         { print_token("DELIM_LBRACE", yytext); return DELIM_LBRACE; }
"}"         { print_token("DELIM_RBRACE", yytext); return DELIM_RBRACE; }
"["         { print_token("DELIM_LBRACK", yytext); return DELIM_LBRACK; }
"]"         { print_token("DELIM_RBRACK", yytext); return DELIM_RBRACK; }

{DECIMALC}  { print_token("DECIMAL_CONST", yytext); return DECIMAL_CONST; }
{INTC}      { print_token("INT_CONST", yytext); return INT_CONST; }
{CHARC}     { print_token("CHAR_CONST", yytext); return CHAR_CONST; }
{STRING}    { print_token("STRING", yytext); return STRING; }
{ID}        { print_token("ID", yytext); return ID; }

"//".*      { /* Single-line comment - ignore */ }
"/*"        { /* Multi-line comment start */
                int c;
                while ((c = input()) != 0) {
                    if (c == '\n') line_no++;
                    else if (c == '*') {
                        if ((c = input()) == '/') break;
                        else unput(c);
                    }
                }
            }

[ \t\r]+      { /* Ignore whitespace */ }
\n          { line_no++; }
.           { printf("Lexical Error at line %d: %s\n", line_no, yytext); }
%%

int yywrap() { 
    return 1; 
}

#ifdef LEXER_STANDALONE
int main() {
    int token;
    printf("=== LEXICAL ANALYSIS ===\n");
    while ((token = yylex())) {
        // Tokens already printed by print_token()
    }
    return 0;
}
#endif