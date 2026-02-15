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
/* "%code requires" blocks.  */
#line 1 "parser.y"

    #include "ast.h"

#line 53 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KEYWORD_MATRIX = 258,          /* KEYWORD_MATRIX  */
    KEYWORD_ARR = 259,             /* KEYWORD_ARR  */
    KEYWORD_MAIN = 260,            /* KEYWORD_MAIN  */
    KEYWORD_IF = 261,              /* KEYWORD_IF  */
    KEYWORD_ELSE = 262,            /* KEYWORD_ELSE  */
    KEYWORD_FOR = 263,             /* KEYWORD_FOR  */
    KEYWORD_WHILE = 264,           /* KEYWORD_WHILE  */
    KEYWORD_SWITCH = 265,          /* KEYWORD_SWITCH  */
    KEYWORD_CASE = 266,            /* KEYWORD_CASE  */
    KEYWORD_DEFAULT = 267,         /* KEYWORD_DEFAULT  */
    KEYWORD_BREAK = 268,           /* KEYWORD_BREAK  */
    KEYWORD_CONTINUE = 269,        /* KEYWORD_CONTINUE  */
    KEYWORD_RETURN = 270,          /* KEYWORD_RETURN  */
    KEYWORD_PRINT = 271,           /* KEYWORD_PRINT  */
    KEYWORD_SCAN = 272,            /* KEYWORD_SCAN  */
    OP_EQ = 273,                   /* OP_EQ  */
    OP_NE = 274,                   /* OP_NE  */
    OP_LE = 275,                   /* OP_LE  */
    OP_GE = 276,                   /* OP_GE  */
    OP_LT = 277,                   /* OP_LT  */
    OP_GT = 278,                   /* OP_GT  */
    OP_PLUS = 279,                 /* OP_PLUS  */
    OP_MINUS = 280,                /* OP_MINUS  */
    OP_MUL = 281,                  /* OP_MUL  */
    OP_DIV = 282,                  /* OP_DIV  */
    OP_MOD = 283,                  /* OP_MOD  */
    OP_INC = 284,                  /* OP_INC  */
    OP_DEC = 285,                  /* OP_DEC  */
    OP_AND = 286,                  /* OP_AND  */
    OP_OR = 287,                   /* OP_OR  */
    OP_NOT = 288,                  /* OP_NOT  */
    OP_ASSIGN = 289,               /* OP_ASSIGN  */
    OP_SIZE = 290,                 /* OP_SIZE  */
    OP_SORT = 291,                 /* OP_SORT  */
    OP_MATADD = 292,               /* OP_MATADD  */
    OP_MATSUB = 293,               /* OP_MATSUB  */
    OP_MATMUL = 294,               /* OP_MATMUL  */
    OP_MATTRANSPOSE = 295,         /* OP_MATTRANSPOSE  */
    OP_MATDET = 296,               /* OP_MATDET  */
    OP_MATINV = 297,               /* OP_MATINV  */
    OP_MATSHAPE = 298,             /* OP_MATSHAPE  */
    DELIM_SEMI = 299,              /* DELIM_SEMI  */
    DELIM_COMMA = 300,             /* DELIM_COMMA  */
    DELIM_COLON = 301,             /* DELIM_COLON  */
    DELIM_LPAREN = 302,            /* DELIM_LPAREN  */
    DELIM_RPAREN = 303,            /* DELIM_RPAREN  */
    DELIM_LBRACE = 304,            /* DELIM_LBRACE  */
    DELIM_RBRACE = 305,            /* DELIM_RBRACE  */
    DELIM_LBRACK = 306,            /* DELIM_LBRACK  */
    DELIM_RBRACK = 307,            /* DELIM_RBRACK  */
    ID = 308,                      /* ID  */
    INT_CONST = 309,               /* INT_CONST  */
    DECIMAL_CONST = 310,           /* DECIMAL_CONST  */
    CHAR_CONST = 311,              /* CHAR_CONST  */
    STRING = 312,                  /* STRING  */
    UMINUS = 313                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 40 "parser.y"

    ASTNode *node;
    char *str;

#line 133 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
