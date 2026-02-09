%{
#include "parser.tab.h"
#include <stdio.h>
int line_no = 1;
%}


DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INTC        {DIGIT}+
DECIMALC    {DIGIT}+\.{DIGIT}+
CHARC       '.'
STRING      \"[^\"]*\"


%%
"int"       { return KEYWORD_INT; }
"decimal"   { return KEYWORD_DECIMAL; }
"char"      { return KEYWORD_CHAR; }
"matrix"    { return KEYWORD_MATRIX; }
"main"      { return KEYWORD_MAIN; }
"if"        { return KEYWORD_IF; }
"else"      { return KEYWORD_ELSE; }
"for"       { return KEYWORD_FOR; }
"while"     { return KEYWORD_WHILE; }
"switch"    { return KEYWORD_SWITCH; }
"case"      { return KEYWORD_CASE; }
"default"   { return KEYWORD_DEFAULT; }
"break"     { return KEYWORD_BREAK; }
"continue"  { return KEYWORD_CONTINUE; }
"return"    { return KEYWORD_RETURN; }
"print"     { return KEYWORD_PRINT; }
"scan"      { return KEYWORD_SCAN; }

"=="        { return OP_EQ; }
"!="        { return OP_NE; }
"<="        { return OP_LE; }
">="        { return OP_GE; }
"<"         { return OP_LT; }
">"         { return OP_GT; }
"&&"        { return OP_AND; }
"||"        { return OP_OR; }
"!"         { return OP_NOT; }
"+"         { return OP_PLUS; }
"-"         { return OP_MINUS; }
"*"         { return OP_MUL; }
"/"         { return OP_DIV; }
"%"         { return OP_MOD; }
"="         { return OP_ASSIGN; }

";"         { return DELIM_SEMI; }
","         { return DELIM_COMMA; }
":"         { return DELIM_COLON; }
"("         { return DELIM_LPAREN; }
")"         { return DELIM_RPAREN; }
"{"         { return DELIM_LBRACE; }
"}"         { return DELIM_RBRACE; }
"["         { return DELIM_LBRACK; }
"]"         { return DELIM_RBRACK; }

{DECIMALC}  { return DECIMAL_CONST; }
{INTC}      { return INT_CONST; }
{CHARC}     { return CHAR_CONST; }
{STRING}    { return STRING; }
{ID}        { return ID; }

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

[ \t]+      { /* Ignore whitespace */ }
\n          { line_no++; }
.           { printf("Lexical Error at line %d: %s\n", line_no, yytext); }
%%

int yywrap() { 
    return 1; 
}
