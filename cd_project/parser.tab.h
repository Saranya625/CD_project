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
    KEYWORD_STRING = 261,          /* KEYWORD_STRING  */
    KEYWORD_MATRIX = 262,          /* KEYWORD_MATRIX  */
    KEYWORD_ARR = 263,             /* KEYWORD_ARR  */
    KEYWORD_MAIN = 264,            /* KEYWORD_MAIN  */
    KEYWORD_IF = 265,              /* KEYWORD_IF  */
    KEYWORD_ELSE = 266,            /* KEYWORD_ELSE  */
    KEYWORD_FOR = 267,             /* KEYWORD_FOR  */
    KEYWORD_WHILE = 268,           /* KEYWORD_WHILE  */
    KEYWORD_SWITCH = 269,          /* KEYWORD_SWITCH  */
    KEYWORD_CASE = 270,            /* KEYWORD_CASE  */
    KEYWORD_DEFAULT = 271,         /* KEYWORD_DEFAULT  */
    KEYWORD_BREAK = 272,           /* KEYWORD_BREAK  */
    KEYWORD_CONTINUE = 273,        /* KEYWORD_CONTINUE  */
    KEYWORD_RETURN = 274,          /* KEYWORD_RETURN  */
    KEYWORD_PRINT = 275,           /* KEYWORD_PRINT  */
    KEYWORD_SCAN = 276,            /* KEYWORD_SCAN  */
    OP_EQ = 277,                   /* OP_EQ  */
    OP_NE = 278,                   /* OP_NE  */
    OP_LE = 279,                   /* OP_LE  */
    OP_GE = 280,                   /* OP_GE  */
    OP_LT = 281,                   /* OP_LT  */
    OP_GT = 282,                   /* OP_GT  */
    OP_PLUS = 283,                 /* OP_PLUS  */
    OP_MINUS = 284,                /* OP_MINUS  */
    OP_MUL = 285,                  /* OP_MUL  */
    OP_DIV = 286,                  /* OP_DIV  */
    OP_MOD = 287,                  /* OP_MOD  */
    OP_INC = 288,                  /* OP_INC  */
    OP_DEC = 289,                  /* OP_DEC  */
    OP_AND = 290,                  /* OP_AND  */
    OP_OR = 291,                   /* OP_OR  */
    OP_NOT = 292,                  /* OP_NOT  */
    OP_ASSIGN = 293,               /* OP_ASSIGN  */
    OP_SIZE = 294,                 /* OP_SIZE  */
    OP_SORT = 295,                 /* OP_SORT  */
    OP_MATADD = 296,               /* OP_MATADD  */
    OP_MATSUB = 297,               /* OP_MATSUB  */
    OP_MATMUL = 298,               /* OP_MATMUL  */
    OP_MATTRANSPOSE = 299,         /* OP_MATTRANSPOSE  */
    OP_MATDET = 300,               /* OP_MATDET  */
    OP_MATINV = 301,               /* OP_MATINV  */
    OP_MATSHAPE = 302,             /* OP_MATSHAPE  */
    DELIM_SEMI = 303,              /* DELIM_SEMI  */
    DELIM_COMMA = 304,             /* DELIM_COMMA  */
    DELIM_COLON = 305,             /* DELIM_COLON  */
    DELIM_LPAREN = 306,            /* DELIM_LPAREN  */
    DELIM_RPAREN = 307,            /* DELIM_RPAREN  */
    DELIM_LBRACE = 308,            /* DELIM_LBRACE  */
    DELIM_RBRACE = 309,            /* DELIM_RBRACE  */
    DELIM_LBRACK = 310,            /* DELIM_LBRACK  */
    DELIM_RBRACK = 311,            /* DELIM_RBRACK  */
    ID = 312,                      /* ID  */
    INT_CONST = 313,               /* INT_CONST  */
    DECIMAL_CONST = 314,           /* DECIMAL_CONST  */
    CHAR_CONST = 315,              /* CHAR_CONST  */
    STRING_CONST = 316,            /* STRING_CONST  */
    UMINUS = 317                   /* UMINUS  */
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

#line 137 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
