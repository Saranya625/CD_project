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
    KEYWORD_INT = 258,             /* KEYWORD_INT  */
    KEYWORD_DECIMAL = 259,         /* KEYWORD_DECIMAL  */
    KEYWORD_CHAR = 260,            /* KEYWORD_CHAR  */
    KEYWORD_MATRIX = 261,          /* KEYWORD_MATRIX  */
    KEYWORD_ARR = 262,             /* KEYWORD_ARR  */
    KEYWORD_MAIN = 263,            /* KEYWORD_MAIN  */
    KEYWORD_IF = 264,              /* KEYWORD_IF  */
    KEYWORD_ELSE = 265,            /* KEYWORD_ELSE  */
    KEYWORD_FOR = 266,             /* KEYWORD_FOR  */
    KEYWORD_WHILE = 267,           /* KEYWORD_WHILE  */
    KEYWORD_SWITCH = 268,          /* KEYWORD_SWITCH  */
    KEYWORD_CASE = 269,            /* KEYWORD_CASE  */
    KEYWORD_DEFAULT = 270,         /* KEYWORD_DEFAULT  */
    KEYWORD_BREAK = 271,           /* KEYWORD_BREAK  */
    KEYWORD_CONTINUE = 272,        /* KEYWORD_CONTINUE  */
    KEYWORD_RETURN = 273,          /* KEYWORD_RETURN  */
    KEYWORD_PRINT = 274,           /* KEYWORD_PRINT  */
    KEYWORD_SCAN = 275,            /* KEYWORD_SCAN  */
    OP_EQ = 276,                   /* OP_EQ  */
    OP_NE = 277,                   /* OP_NE  */
    OP_LE = 278,                   /* OP_LE  */
    OP_GE = 279,                   /* OP_GE  */
    OP_LT = 280,                   /* OP_LT  */
    OP_GT = 281,                   /* OP_GT  */
    OP_PLUS = 282,                 /* OP_PLUS  */
    OP_MINUS = 283,                /* OP_MINUS  */
    OP_MUL = 284,                  /* OP_MUL  */
    OP_DIV = 285,                  /* OP_DIV  */
    OP_MOD = 286,                  /* OP_MOD  */
    OP_INC = 287,                  /* OP_INC  */
    OP_DEC = 288,                  /* OP_DEC  */
    OP_AND = 289,                  /* OP_AND  */
    OP_OR = 290,                   /* OP_OR  */
    OP_NOT = 291,                  /* OP_NOT  */
    OP_ASSIGN = 292,               /* OP_ASSIGN  */
    OP_SIZE = 293,                 /* OP_SIZE  */
    OP_SORT = 294,                 /* OP_SORT  */
    OP_MATADD = 295,               /* OP_MATADD  */
    OP_MATSUB = 296,               /* OP_MATSUB  */
    OP_MATMUL = 297,               /* OP_MATMUL  */
    OP_MATTRANSPOSE = 298,         /* OP_MATTRANSPOSE  */
    OP_MATDET = 299,               /* OP_MATDET  */
    OP_MATINV = 300,               /* OP_MATINV  */
    OP_MATSHAPE = 301,             /* OP_MATSHAPE  */
    DELIM_SEMI = 302,              /* DELIM_SEMI  */
    DELIM_COMMA = 303,             /* DELIM_COMMA  */
    DELIM_COLON = 304,             /* DELIM_COLON  */
    DELIM_LPAREN = 305,            /* DELIM_LPAREN  */
    DELIM_RPAREN = 306,            /* DELIM_RPAREN  */
    DELIM_LBRACE = 307,            /* DELIM_LBRACE  */
    DELIM_RBRACE = 308,            /* DELIM_RBRACE  */
    DELIM_LBRACK = 309,            /* DELIM_LBRACK  */
    DELIM_RBRACK = 310,            /* DELIM_RBRACK  */
    ID = 311,                      /* ID  */
    INT_CONST = 312,               /* INT_CONST  */
    DECIMAL_CONST = 313,           /* DECIMAL_CONST  */
    CHAR_CONST = 314,              /* CHAR_CONST  */
    STRING = 315,                  /* STRING  */
    UMINUS = 316                   /* UMINUS  */
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

#line 136 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
