%{
#include <stdio.h>
int line_no = 1;
%}

/* ---------- Definitions ---------- */
DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*
INTC    {DIGIT}+
STRING  \"[^\"]*\"

/* ---------- Rules ---------- */
%%


"int"       { printf("<KEYWORD, int>\n"); }
"main"      { printf("<KEYWORD, main>\n"); }
"if"        { printf("<KEYWORD, if>\n"); }
"else"      { printf("<KEYWORD, else>\n"); }
"for"       { printf("<KEYWORD, for>\n"); }
"while"     { printf("<KEYWORD, while>\n"); }
"return"    { printf("<KEYWORD, return>\n"); }
"print"     { printf("<KEYWORD, print>\n"); }
"scan"      { printf("<KEYWORD, scan>\n"); }


"=="        { printf("<OP, ==>\n"); }
"!="        { printf("<OP, !=>\n"); }
"<="        { printf("<OP, <=>\n"); }
">="        { printf("<OP, >=>\n"); }
"<"         { printf("<OP, <>\n"); }
">"         { printf("<OP, >>\n"); }
"+"         { printf("<OP, +>\n"); }
"-"         { printf("<OP, ->\n"); }
"*"         { printf("<OP, *>\n"); }
"/"         { printf("<OP, />\n"); }
"="         { printf("<OP, =>\n"); }


";"         { printf("<DELIM, ;>\n"); }
","         { printf("<DELIM, ,>\n"); }
"("         { printf("<DELIM, (>\n"); }
")"         { printf("<DELIM, )>\n"); }
"{"         { printf("<DELIM, {>\n"); }
"}"         { printf("<DELIM, }>\n"); }
"["         { printf("<DELIM, [>\n"); }
"]"         { printf("<DELIM, ]>\n"); }


{INTC}      { printf("<INT_CONST, %s>\n", yytext); }
{STRING}    { printf("<STRING, %s>\n", yytext); }


{ID}        { printf("<ID, %s>\n", yytext); }


[ \t]+      { }
\n          { line_no++; }


.           { printf("Lexical Error at line %d: %s\n", line_no, yytext); }

%%

int yywrap() { return 1; }

int main() {
    yylex();
    return 0;
}

