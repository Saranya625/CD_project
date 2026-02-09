/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KEYWORD_INT = 258,             /* KEYWORD_INT  */
    KEYWORD_DECIMAL = 259,         /* KEYWORD_DECIMAL  */
    KEYWORD_CHAR = 260,            /* KEYWORD_CHAR  */
    KEYWORD_MATRIX = 261,          /* KEYWORD_MATRIX  */
    KEYWORD_MAIN = 262,            /* KEYWORD_MAIN  */
    KEYWORD_IF = 263,              /* KEYWORD_IF  */
    KEYWORD_ELSE = 264,            /* KEYWORD_ELSE  */
    KEYWORD_FOR = 265,             /* KEYWORD_FOR  */
    KEYWORD_WHILE = 266,           /* KEYWORD_WHILE  */
    KEYWORD_SWITCH = 267,          /* KEYWORD_SWITCH  */
    KEYWORD_CASE = 268,            /* KEYWORD_CASE  */
    KEYWORD_DEFAULT = 269,         /* KEYWORD_DEFAULT  */
    KEYWORD_BREAK = 270,           /* KEYWORD_BREAK  */
    KEYWORD_CONTINUE = 271,        /* KEYWORD_CONTINUE  */
    KEYWORD_RETURN = 272,          /* KEYWORD_RETURN  */
    KEYWORD_PRINT = 273,           /* KEYWORD_PRINT  */
    KEYWORD_SCAN = 274,            /* KEYWORD_SCAN  */
    OP_EQ = 275,                   /* OP_EQ  */
    OP_NE = 276,                   /* OP_NE  */
    OP_LE = 277,                   /* OP_LE  */
    OP_GE = 278,                   /* OP_GE  */
    OP_LT = 279,                   /* OP_LT  */
    OP_GT = 280,                   /* OP_GT  */
    OP_PLUS = 281,                 /* OP_PLUS  */
    OP_MINUS = 282,                /* OP_MINUS  */
    OP_MUL = 283,                  /* OP_MUL  */
    OP_DIV = 284,                  /* OP_DIV  */
    OP_MOD = 285,                  /* OP_MOD  */
    OP_AND = 286,                  /* OP_AND  */
    OP_OR = 287,                   /* OP_OR  */
    OP_NOT = 288,                  /* OP_NOT  */
    OP_ASSIGN = 289,               /* OP_ASSIGN  */
    DELIM_SEMI = 290,              /* DELIM_SEMI  */
    DELIM_COMMA = 291,             /* DELIM_COMMA  */
    DELIM_COLON = 292,             /* DELIM_COLON  */
    DELIM_LPAREN = 293,            /* DELIM_LPAREN  */
    DELIM_RPAREN = 294,            /* DELIM_RPAREN  */
    DELIM_LBRACE = 295,            /* DELIM_LBRACE  */
    DELIM_RBRACE = 296,            /* DELIM_RBRACE  */
    DELIM_LBRACK = 297,            /* DELIM_LBRACK  */
    DELIM_RBRACK = 298,            /* DELIM_RBRACK  */
    INT_CONST = 299,               /* INT_CONST  */
    DECIMAL_CONST = 300,           /* DECIMAL_CONST  */
    CHAR_CONST = 301,              /* CHAR_CONST  */
    STRING = 302,                  /* STRING  */
    ID = 303,                      /* ID  */
    UMINUS = 304                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
