/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 5 "parser.y"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"
    #include "semantic.h"
    #include "ir.h"

    extern int line_no;
    extern int yylex();
    void yyerror(const char *s);
    void set_token_output(FILE *out);

    ASTNode* root;

#line 87 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_KEYWORD_INT = 3,                /* KEYWORD_INT  */
  YYSYMBOL_KEYWORD_DECIMAL = 4,            /* KEYWORD_DECIMAL  */
  YYSYMBOL_KEYWORD_CHAR = 5,               /* KEYWORD_CHAR  */
  YYSYMBOL_KEYWORD_STRING = 6,             /* KEYWORD_STRING  */
  YYSYMBOL_KEYWORD_MATRIX = 7,             /* KEYWORD_MATRIX  */
  YYSYMBOL_KEYWORD_ARR = 8,                /* KEYWORD_ARR  */
  YYSYMBOL_KEYWORD_MAIN = 9,               /* KEYWORD_MAIN  */
  YYSYMBOL_KEYWORD_IF = 10,                /* KEYWORD_IF  */
  YYSYMBOL_KEYWORD_ELSE = 11,              /* KEYWORD_ELSE  */
  YYSYMBOL_KEYWORD_FOR = 12,               /* KEYWORD_FOR  */
  YYSYMBOL_KEYWORD_WHILE = 13,             /* KEYWORD_WHILE  */
  YYSYMBOL_KEYWORD_SWITCH = 14,            /* KEYWORD_SWITCH  */
  YYSYMBOL_KEYWORD_CASE = 15,              /* KEYWORD_CASE  */
  YYSYMBOL_KEYWORD_DEFAULT = 16,           /* KEYWORD_DEFAULT  */
  YYSYMBOL_KEYWORD_BREAK = 17,             /* KEYWORD_BREAK  */
  YYSYMBOL_KEYWORD_CONTINUE = 18,          /* KEYWORD_CONTINUE  */
  YYSYMBOL_KEYWORD_RETURN = 19,            /* KEYWORD_RETURN  */
  YYSYMBOL_KEYWORD_PRINT = 20,             /* KEYWORD_PRINT  */
  YYSYMBOL_KEYWORD_SCAN = 21,              /* KEYWORD_SCAN  */
  YYSYMBOL_OP_EQ = 22,                     /* OP_EQ  */
  YYSYMBOL_OP_NE = 23,                     /* OP_NE  */
  YYSYMBOL_OP_LE = 24,                     /* OP_LE  */
  YYSYMBOL_OP_GE = 25,                     /* OP_GE  */
  YYSYMBOL_OP_LT = 26,                     /* OP_LT  */
  YYSYMBOL_OP_GT = 27,                     /* OP_GT  */
  YYSYMBOL_OP_PLUS = 28,                   /* OP_PLUS  */
  YYSYMBOL_OP_MINUS = 29,                  /* OP_MINUS  */
  YYSYMBOL_OP_MUL = 30,                    /* OP_MUL  */
  YYSYMBOL_OP_DIV = 31,                    /* OP_DIV  */
  YYSYMBOL_OP_MOD = 32,                    /* OP_MOD  */
  YYSYMBOL_OP_INC = 33,                    /* OP_INC  */
  YYSYMBOL_OP_DEC = 34,                    /* OP_DEC  */
  YYSYMBOL_OP_AND = 35,                    /* OP_AND  */
  YYSYMBOL_OP_OR = 36,                     /* OP_OR  */
  YYSYMBOL_OP_NOT = 37,                    /* OP_NOT  */
  YYSYMBOL_OP_ASSIGN = 38,                 /* OP_ASSIGN  */
  YYSYMBOL_OP_SIZE = 39,                   /* OP_SIZE  */
  YYSYMBOL_OP_SORT = 40,                   /* OP_SORT  */
  YYSYMBOL_OP_MATADD = 41,                 /* OP_MATADD  */
  YYSYMBOL_OP_MATSUB = 42,                 /* OP_MATSUB  */
  YYSYMBOL_OP_MATMUL = 43,                 /* OP_MATMUL  */
  YYSYMBOL_OP_MATTRANSPOSE = 44,           /* OP_MATTRANSPOSE  */
  YYSYMBOL_OP_MATDET = 45,                 /* OP_MATDET  */
  YYSYMBOL_OP_MATINV = 46,                 /* OP_MATINV  */
  YYSYMBOL_OP_MATSHAPE = 47,               /* OP_MATSHAPE  */
  YYSYMBOL_DELIM_SEMI = 48,                /* DELIM_SEMI  */
  YYSYMBOL_DELIM_COMMA = 49,               /* DELIM_COMMA  */
  YYSYMBOL_DELIM_COLON = 50,               /* DELIM_COLON  */
  YYSYMBOL_DELIM_LPAREN = 51,              /* DELIM_LPAREN  */
  YYSYMBOL_DELIM_RPAREN = 52,              /* DELIM_RPAREN  */
  YYSYMBOL_DELIM_LBRACE = 53,              /* DELIM_LBRACE  */
  YYSYMBOL_DELIM_RBRACE = 54,              /* DELIM_RBRACE  */
  YYSYMBOL_DELIM_LBRACK = 55,              /* DELIM_LBRACK  */
  YYSYMBOL_DELIM_RBRACK = 56,              /* DELIM_RBRACK  */
  YYSYMBOL_ID = 57,                        /* ID  */
  YYSYMBOL_INT_CONST = 58,                 /* INT_CONST  */
  YYSYMBOL_DECIMAL_CONST = 59,             /* DECIMAL_CONST  */
  YYSYMBOL_CHAR_CONST = 60,                /* CHAR_CONST  */
  YYSYMBOL_STRING_CONST = 61,              /* STRING_CONST  */
  YYSYMBOL_UMINUS = 62,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 63,                  /* $accept  */
  YYSYMBOL_program = 64,                   /* program  */
  YYSYMBOL_global_declarations = 65,       /* global_declarations  */
  YYSYMBOL_global_declaration = 66,        /* global_declaration  */
  YYSYMBOL_type_specifier = 67,            /* type_specifier  */
  YYSYMBOL_main_block = 68,                /* main_block  */
  YYSYMBOL_block = 69,                     /* block  */
  YYSYMBOL_statements = 70,                /* statements  */
  YYSYMBOL_statement = 71,                 /* statement  */
  YYSYMBOL_delim_statement = 72,           /* delim_statement  */
  YYSYMBOL_declaration = 73,               /* declaration  */
  YYSYMBOL_assignment = 74,                /* assignment  */
  YYSYMBOL_if_statement = 75,              /* if_statement  */
  YYSYMBOL_for_loop = 76,                  /* for_loop  */
  YYSYMBOL_while_loop = 77,                /* while_loop  */
  YYSYMBOL_switch_statement = 78,          /* switch_statement  */
  YYSYMBOL_case_list_opt = 79,             /* case_list_opt  */
  YYSYMBOL_case_list = 80,                 /* case_list  */
  YYSYMBOL_case_statement = 81,            /* case_statement  */
  YYSYMBOL_default_case = 82,              /* default_case  */
  YYSYMBOL_break_statement = 83,           /* break_statement  */
  YYSYMBOL_continue_statement = 84,        /* continue_statement  */
  YYSYMBOL_return_statement = 85,          /* return_statement  */
  YYSYMBOL_print_statement = 86,           /* print_statement  */
  YYSYMBOL_print_args = 87,                /* print_args  */
  YYSYMBOL_print_arg = 88,                 /* print_arg  */
  YYSYMBOL_scan_statement = 89,            /* scan_statement  */
  YYSYMBOL_lvalue = 90,                    /* lvalue  */
  YYSYMBOL_assignment_expr = 91,           /* assignment_expr  */
  YYSYMBOL_inc_dec_expr = 92,              /* inc_dec_expr  */
  YYSYMBOL_expression = 93,                /* expression  */
  YYSYMBOL_logical_or_expression = 94,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 95,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 96,       /* equality_expression  */
  YYSYMBOL_relational_expression = 97,     /* relational_expression  */
  YYSYMBOL_additive_expression = 98,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 99, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 100,         /* unary_expression  */
  YYSYMBOL_postfix_expression = 101,       /* postfix_expression  */
  YYSYMBOL_primary_expression = 102,       /* primary_expression  */
  YYSYMBOL_constant = 103,                 /* constant  */
  YYSYMBOL_array_initializer = 104,        /* array_initializer  */
  YYSYMBOL_array_init_list_opt = 105,      /* array_init_list_opt  */
  YYSYMBOL_array_init_list = 106,          /* array_init_list  */
  YYSYMBOL_matrix_initializer = 107,       /* matrix_initializer  */
  YYSYMBOL_matrix_row_list_opt = 108,      /* matrix_row_list_opt  */
  YYSYMBOL_matrix_row_list = 109,          /* matrix_row_list  */
  YYSYMBOL_matrix_row = 110                /* matrix_row  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   354

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  146
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  324

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   317


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   111,   111,   117,   126,   129,   134,   141,   148,   155,
     163,   169,   180,   181,   182,   183,   187,   190,   196,   199,
     204,   206,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   228,   233,   240,   247,
     254,   260,   268,   277,   280,   286,   294,   297,   300,   308,
     316,   321,   327,   329,   334,   337,   340,   345,   350,   356,
     361,   366,   369,   374,   379,   382,   387,   390,   393,   396,
     401,   406,   409,   412,   417,   423,   429,   435,   441,   444,
     447,   450,   453,   456,   460,   463,   466,   469,   472,   475,
     482,   487,   490,   495,   497,   502,   505,   508,   513,   516,
     519,   522,   525,   530,   533,   536,   541,   544,   547,   550,
     555,   558,   561,   564,   567,   572,   575,   578,   581,   584,
     587,   590,   593,   596,   599,   602,   605,   610,   611,   614,
     617,   620,   625,   628,   631,   634,   639,   645,   647,   652,
     655,   660,   666,   668,   673,   676,   681
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "KEYWORD_INT",
  "KEYWORD_DECIMAL", "KEYWORD_CHAR", "KEYWORD_STRING", "KEYWORD_MATRIX",
  "KEYWORD_ARR", "KEYWORD_MAIN", "KEYWORD_IF", "KEYWORD_ELSE",
  "KEYWORD_FOR", "KEYWORD_WHILE", "KEYWORD_SWITCH", "KEYWORD_CASE",
  "KEYWORD_DEFAULT", "KEYWORD_BREAK", "KEYWORD_CONTINUE", "KEYWORD_RETURN",
  "KEYWORD_PRINT", "KEYWORD_SCAN", "OP_EQ", "OP_NE", "OP_LE", "OP_GE",
  "OP_LT", "OP_GT", "OP_PLUS", "OP_MINUS", "OP_MUL", "OP_DIV", "OP_MOD",
  "OP_INC", "OP_DEC", "OP_AND", "OP_OR", "OP_NOT", "OP_ASSIGN", "OP_SIZE",
  "OP_SORT", "OP_MATADD", "OP_MATSUB", "OP_MATMUL", "OP_MATTRANSPOSE",
  "OP_MATDET", "OP_MATINV", "OP_MATSHAPE", "DELIM_SEMI", "DELIM_COMMA",
  "DELIM_COLON", "DELIM_LPAREN", "DELIM_RPAREN", "DELIM_LBRACE",
  "DELIM_RBRACE", "DELIM_LBRACK", "DELIM_RBRACK", "ID", "INT_CONST",
  "DECIMAL_CONST", "CHAR_CONST", "STRING_CONST", "UMINUS", "$accept",
  "program", "global_declarations", "global_declaration", "type_specifier",
  "main_block", "block", "statements", "statement", "delim_statement",
  "declaration", "assignment", "if_statement", "for_loop", "while_loop",
  "switch_statement", "case_list_opt", "case_list", "case_statement",
  "default_case", "break_statement", "continue_statement",
  "return_statement", "print_statement", "print_args", "print_arg",
  "scan_statement", "lvalue", "assignment_expr", "inc_dec_expr",
  "expression", "logical_or_expression", "logical_and_expression",
  "equality_expression", "relational_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "postfix_expression",
  "primary_expression", "constant", "array_initializer",
  "array_init_list_opt", "array_init_list", "matrix_initializer",
  "matrix_row_list_opt", "matrix_row_list", "matrix_row", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-79)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     114,     3,   -79,   -79,   -79,   -34,   -25,    35,   141,   -79,
       6,    24,    27,    36,   -79,    88,   -79,   -17,    72,    70,
      99,    83,    92,   -79,    87,   109,   110,   120,   -79,   -79,
     -79,   -79,   126,    82,   -79,   121,   -12,   125,   -79,   -79,
     118,   128,   136,   140,   147,   148,   158,   159,    -4,   157,
     160,   135,   155,   -79,   -79,   -16,   156,   -79,   176,   -79,
     -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,
     -79,   -79,   -79,   161,   162,   -79,   258,   -79,   163,   165,
      47,   -23,    47,    47,   -79,   -79,    47,    47,    47,    47,
     -79,    47,   166,   169,   178,   193,    43,   144,   104,    81,
     -79,   207,   -79,   -79,    66,   174,   179,   182,   -79,   -79,
      47,    47,   -11,   -79,   -79,   183,    47,   190,   258,   188,
     185,   197,   204,   -14,   211,   -79,   208,   217,   -79,   -79,
     -79,   -79,   222,    47,   -79,    47,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,   -79,   -79,
     -79,   -79,   230,   231,   232,   -79,   -79,   -79,   -79,   229,
     -79,   -43,   -79,   233,   234,    47,    47,   237,   238,    47,
     -79,   -10,   -79,   235,   241,   -79,   239,   -79,   240,   242,
      87,    47,    47,    47,    87,   244,   -79,   243,   193,    43,
     144,   144,   104,   104,   104,   104,    81,    81,   -79,   -79,
     -79,    47,    47,    47,    47,    66,   247,    47,   252,   245,
     246,   -79,    12,   255,   251,   -79,   -79,    47,   -79,   250,
      14,   276,   -79,   253,   259,   -79,   293,   257,   261,   262,
     264,   254,   -79,   -79,   263,   -79,   265,   266,   -79,   -79,
      47,    47,   -79,   269,   270,   -79,   267,   162,   -79,    -5,
      76,   -23,    92,   272,   122,   -79,    47,   -79,   -79,   -79,
     268,   273,    47,    47,   279,   274,    47,   275,   282,   -79,
     -79,   277,   284,   -79,   -79,    47,    47,   283,   286,   -79,
     287,   -79,   -79,   278,    47,    47,   285,   288,   -79,    26,
     289,   -79,   269,    20,   -79,   -79,   290,    87,   258,   258,
     -79,   291,   292,   -79,   -79,   -79,   -79,    47,   -79,   -79,
     251,   -79,    45,   -79,   258,   258,   -79,   -79,   294,   297,
      47,   -79,   -79,   -79
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    12,    13,    14,    15,     0,     0,     0,     0,     4,
       0,     0,     0,     0,     1,    12,     5,     0,     0,     0,
       0,     0,     0,     6,     0,     0,     0,     0,   132,   133,
     134,   135,     0,     0,     3,     0,     0,     0,     7,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    19,     0,     0,    35,     0,    20,
      34,    22,    23,    25,    26,    27,    28,    29,    30,    33,
      31,    32,    24,     0,     0,    10,     0,     2,     0,     0,
       0,     0,     0,     0,    59,    60,     0,     0,     0,     0,
      62,     0,   128,     0,    90,    91,    93,    95,    98,   103,
     106,   110,   115,   127,     0,     0,    78,    79,    84,    85,
       0,     0,     0,    18,    21,     0,   137,     0,     0,    33,
       0,     0,     0,     0,     0,    77,     0,     0,   112,   113,
     114,   111,     0,     0,    61,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   125,   126,
     116,   117,     0,     0,     0,   118,   119,   120,   121,    67,
      66,     0,    64,    71,     0,     0,     0,     0,     0,     0,
      37,     0,   139,     0,   138,    11,    33,    17,     0,     0,
       0,     0,     0,     0,     0,     0,   131,     0,    92,    94,
      96,    97,   101,   102,    99,   100,   104,   105,   107,   108,
     109,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,     0,     8,   136,     0,    16,     0,
       0,    46,    74,     0,     0,    50,    52,   129,     0,     0,
       0,     0,    65,    63,     0,    70,    80,    81,    86,    87,
       0,     0,    38,   142,     0,   140,     0,     0,    40,     0,
       0,     0,     0,     0,    53,    54,     0,   122,   123,   124,
      68,    72,     0,     0,     0,     0,   137,     0,   143,   144,
       9,     0,     0,    47,    48,     0,     0,     0,     0,    51,
       0,    55,    56,     0,     0,     0,     0,     0,    44,     0,
       0,   141,     0,     0,    42,    75,     0,     0,     0,     0,
     130,     0,     0,    82,    83,    88,    89,     0,   146,   145,
       0,    39,     0,    49,    57,    58,    69,    73,     0,     0,
       0,    45,    41,    76
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -79,   -79,   -79,   309,    69,   -79,   -24,   -72,   -57,   -79,
     -79,   -79,    75,   -79,   -79,   -79,   -79,   -79,    84,   -79,
     -79,   -79,   -69,   -79,   -79,   134,   -79,   -79,    89,   -78,
     -40,   -79,   214,   215,    17,    63,    15,   -73,   -79,   -79,
     -20,   103,    86,   -79,    44,   -79,   -79,    61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     7,     8,     9,    56,    77,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,   253,   254,   255,   282,
      67,    68,    69,    70,   161,   162,    71,   164,   124,    72,
     172,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   117,   173,   174,   244,   267,   268,   269
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      34,   114,    32,   125,   118,    42,   205,   119,    93,   206,
      51,    52,    11,   128,   129,   130,   131,   108,   109,   108,
     109,    22,   110,    12,   181,    86,    74,   169,   214,    87,
      88,    23,    13,    89,   123,    14,    75,   170,   215,   111,
     122,   182,   126,   127,    90,   238,   239,    91,    33,   176,
     240,   132,   247,    92,    28,    29,    30,    31,   310,   305,
     306,   114,   248,    17,   307,   137,   138,   241,   311,    10,
     167,   168,   198,   199,   200,    18,    86,    10,   305,   306,
      87,    88,    19,   320,    89,    39,     2,     3,     4,    40,
      41,    20,    42,   187,    43,    44,    45,    21,    91,    46,
      47,    48,    49,    50,    92,    28,    29,    30,    31,   238,
     239,   145,   146,   147,   275,    51,    52,     1,     2,     3,
       4,     5,     6,   159,    24,   209,   210,   160,    25,   213,
      53,   276,   143,   144,    27,    33,    54,   252,   280,    55,
      33,   222,   223,   224,    15,     2,     3,     4,     5,     6,
      28,    29,    30,    31,   190,   191,   221,    26,   196,   197,
     225,   228,   229,   230,   231,    35,    36,   234,   139,   140,
     141,   142,    37,   125,    38,    78,    73,   245,    76,    39,
       2,     3,     4,    40,    41,    79,    42,    80,    43,    44,
      45,    81,   106,    46,    47,    48,    49,    50,    82,    83,
     264,   265,   192,   193,   194,   195,    84,    85,   104,    51,
      52,   105,   107,   112,   135,   116,   283,   134,   120,   115,
     121,   133,   286,   287,    53,   273,   314,   315,   136,    33,
     113,   163,   278,    55,   165,   295,   296,   166,   175,   171,
     148,   149,   177,   178,   301,   302,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   179,   180,   114,   114,   183,
     184,    39,     2,     3,     4,    40,    41,   318,    42,   185,
      43,    44,    45,   313,   186,    46,    47,    48,    49,    50,
     323,   201,   202,   203,   204,   211,   208,   249,   207,   216,
     217,    51,    52,   218,   212,   233,   219,   226,   220,   227,
     235,   236,   237,   242,   243,   246,    53,   251,   252,   250,
     260,    33,   256,   257,   258,    55,   259,    16,   270,   261,
     262,   263,   266,   284,   274,   271,   279,   288,   285,   291,
     289,   292,   294,   293,   300,   297,   298,   299,   281,   232,
     277,   303,   321,   308,   304,   322,   312,   316,   317,   188,
     272,   189,   290,   309,   319
};

static const yytype_int16 yycheck[] =
{
      24,    58,    22,    81,    76,    10,    49,    76,    48,    52,
      33,    34,     9,    86,    87,    88,    89,    33,    34,    33,
      34,    38,    38,    57,    38,    29,    38,    38,    38,    33,
      34,    48,    57,    37,    57,     0,    48,    48,    48,    55,
      80,    55,    82,    83,    48,    33,    34,    51,    53,   118,
      38,    91,    38,    57,    58,    59,    60,    61,    38,    33,
      34,   118,    48,    57,    38,    22,    23,    55,    48,     0,
     110,   111,   145,   146,   147,    51,    29,     8,    33,    34,
      33,    34,    55,    38,    37,     3,     4,     5,     6,     7,
       8,    55,    10,   133,    12,    13,    14,     9,    51,    17,
      18,    19,    20,    21,    57,    58,    59,    60,    61,    33,
      34,    30,    31,    32,    38,    33,    34,     3,     4,     5,
       6,     7,     8,    57,    52,   165,   166,    61,    58,   169,
      48,    55,    28,    29,    51,    53,    54,    15,    16,    57,
      53,   181,   182,   183,     3,     4,     5,     6,     7,     8,
      58,    59,    60,    61,   137,   138,   180,    58,   143,   144,
     184,   201,   202,   203,   204,    56,    56,   207,    24,    25,
      26,    27,    52,   251,    48,    57,    55,   217,    53,     3,
       4,     5,     6,     7,     8,    57,    10,    51,    12,    13,
      14,    51,    57,    17,    18,    19,    20,    21,    51,    51,
     240,   241,   139,   140,   141,   142,    48,    48,    51,    33,
      34,    51,    57,    57,    36,    53,   256,    48,    55,    58,
      55,    55,   262,   263,    48,   249,   298,   299,    35,    53,
      54,    57,   252,    57,    55,   275,   276,    55,    48,    56,
      33,    34,    54,    58,   284,   285,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    58,    52,   314,   315,    48,
      52,     3,     4,     5,     6,     7,     8,   307,    10,    52,
      12,    13,    14,   297,    52,    17,    18,    19,    20,    21,
     320,    51,    51,    51,    55,    48,    52,    11,    55,    54,
      49,    33,    34,    54,    56,    48,    56,    53,    56,    56,
      48,    56,    56,    48,    53,    55,    48,    48,    15,    56,
      56,    53,    55,    52,    52,    57,    52,     8,    48,    56,
      55,    55,    53,    55,   249,    58,    54,    48,    55,    54,
      56,    49,    48,    56,    56,    52,    50,    50,   254,   205,
     251,    56,    48,    54,    56,    48,    56,    56,    56,   135,
     247,   136,   266,   292,   310
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    64,    65,    66,
      67,     9,    57,    57,     0,     3,    66,    57,    51,    55,
      55,     9,    38,    48,    52,    58,    58,    51,    58,    59,
      60,    61,   103,    53,    69,    56,    56,    52,    48,     3,
       7,     8,    10,    12,    13,    14,    17,    18,    19,    20,
      21,    33,    34,    48,    54,    57,    67,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    83,    84,    85,
      86,    89,    92,    55,    38,    48,    53,    68,    57,    57,
      51,    51,    51,    51,    48,    48,    29,    33,    34,    37,
      48,    51,    57,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    51,    51,    57,    57,    33,    34,
      38,    55,    57,    54,    71,    58,    53,   104,    70,    85,
      55,    55,    93,    57,    91,    92,    93,    93,   100,   100,
     100,   100,    93,    55,    48,    36,    35,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    57,
      61,    87,    88,    57,    90,    55,    55,    93,    93,    38,
      48,    56,    93,   105,   106,    48,    85,    54,    58,    58,
      52,    38,    55,    48,    52,    52,    52,    93,    95,    96,
      97,    97,    98,    98,    98,    98,    99,    99,   100,   100,
     100,    51,    51,    51,    55,    49,    52,    55,    52,    93,
      93,    48,    56,    93,    38,    48,    54,    49,    54,    56,
      56,    69,    93,    93,    93,    69,    53,    56,    93,    93,
      93,    93,    88,    48,    93,    48,    56,    56,    33,    34,
      38,    55,    48,    53,   107,    93,    55,    38,    48,    11,
      56,    48,    15,    79,    80,    81,    55,    52,    52,    52,
      56,    56,    55,    55,    93,    93,    53,   108,   109,   110,
      48,    58,   104,    69,    75,    38,    55,    91,   103,    54,
      16,    81,    82,    93,    55,    55,    93,    93,    48,    56,
     105,    54,    49,    56,    48,    93,    93,    52,    50,    50,
      56,    93,    93,    56,    56,    33,    34,    38,    54,   110,
      38,    48,    56,    69,    70,    70,    56,    56,    93,   107,
      38,    48,    48,    93
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    63,    64,    64,    65,    65,    66,    66,    66,    66,
      66,    66,    67,    67,    67,    67,    68,    68,    69,    69,
      70,    70,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    72,    73,    73,    73,
      73,    73,    73,    74,    74,    74,    75,    75,    75,    76,
      77,    78,    79,    79,    80,    80,    80,    81,    82,    83,
      84,    85,    85,    86,    87,    87,    88,    88,    88,    88,
      89,    90,    90,    90,    91,    91,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      93,    94,    94,    95,    95,    96,    96,    96,    97,    97,
      97,    97,    97,    98,    98,    98,    99,    99,    99,    99,
     100,   100,   100,   100,   100,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   102,   102,   102,
     102,   102,   103,   103,   103,   103,   104,   105,   105,   106,
     106,   107,   108,   108,   109,   109,   110
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     6,     5,     1,     2,     3,     5,     9,    11,
       6,     8,     1,     1,     1,     1,     4,     3,     3,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     5,     9,
       6,    11,     8,     4,     7,    10,     5,     7,     7,     9,
       5,     7,     0,     1,     1,     2,     2,     4,     3,     2,
       2,     3,     2,     5,     1,     3,     1,     1,     4,     7,
       5,     1,     4,     7,     3,     6,     9,     1,     2,     2,
       5,     5,     8,     8,     2,     2,     5,     5,     8,     8,
       1,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       1,     2,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     5,     5,     5,     2,     2,     1,     1,     4,
       7,     3,     1,     1,     1,     1,     3,     0,     1,     1,
       3,     3,     0,     1,     1,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: global_declarations KEYWORD_INT KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN main_block  */
#line 112 "parser.y"
    { 
        (yyval.node) = createNode("program", NULL, (yyvsp[-5].node), (yyvsp[0].node), NULL); 
        root = (yyval.node);  
    }
#line 1419 "parser.tab.c"
    break;

  case 3: /* program: KEYWORD_INT KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block  */
#line 118 "parser.y"
    { 
        (yyval.node) = createNode("program", NULL, NULL, (yyvsp[0].node), NULL); 
        root = (yyval.node);
    }
#line 1428 "parser.tab.c"
    break;

  case 4: /* global_declarations: global_declaration  */
#line 127 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1434 "parser.tab.c"
    break;

  case 5: /* global_declarations: global_declarations global_declaration  */
#line 130 "parser.y"
    { (yyval.node) = createNode("global", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1440 "parser.tab.c"
    break;

  case 6: /* global_declaration: type_specifier ID DELIM_SEMI  */
#line 135 "parser.y"
    {
        (yyval.node) = createNode("decl", NULL,
                createNode("type", (yyvsp[-2].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL);
    }
#line 1450 "parser.tab.c"
    break;

  case 7: /* global_declaration: type_specifier ID OP_ASSIGN constant DELIM_SEMI  */
#line 142 "parser.y"
    {
        (yyval.node) = createNode("decl_assign", NULL,
                createNode("type", (yyvsp[-4].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-3].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1460 "parser.tab.c"
    break;

  case 8: /* global_declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 149 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1470 "parser.tab.c"
    break;

  case 9: /* global_declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK OP_ASSIGN matrix_initializer DELIM_SEMI  */
#line 157 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-9].str),
                createNode("rows", (yyvsp[-7].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-4].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1480 "parser.tab.c"
    break;

  case 10: /* global_declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 164 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1489 "parser.tab.c"
    break;

  case 11: /* global_declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK OP_ASSIGN array_initializer DELIM_SEMI  */
#line 170 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-6].str),
                createNode("size", (yyvsp[-4].str), NULL, NULL, NULL), NULL, (yyvsp[-1].node));
    }
#line 1498 "parser.tab.c"
    break;

  case 12: /* type_specifier: KEYWORD_INT  */
#line 180 "parser.y"
                        { (yyval.str) = "int"; }
#line 1504 "parser.tab.c"
    break;

  case 13: /* type_specifier: KEYWORD_DECIMAL  */
#line 181 "parser.y"
                        { (yyval.str) = "decimal"; }
#line 1510 "parser.tab.c"
    break;

  case 14: /* type_specifier: KEYWORD_CHAR  */
#line 182 "parser.y"
                        { (yyval.str) = "char"; }
#line 1516 "parser.tab.c"
    break;

  case 15: /* type_specifier: KEYWORD_STRING  */
#line 183 "parser.y"
                        { (yyval.str) = "string"; }
#line 1522 "parser.tab.c"
    break;

  case 16: /* main_block: DELIM_LBRACE statements return_statement DELIM_RBRACE  */
#line 188 "parser.y"
    { (yyval.node) = createNode("main_block", NULL, (yyvsp[-2].node), (yyvsp[-1].node), NULL); }
#line 1528 "parser.tab.c"
    break;

  case 17: /* main_block: DELIM_LBRACE return_statement DELIM_RBRACE  */
#line 191 "parser.y"
    { (yyval.node) = createNode("main_block", NULL, NULL, (yyvsp[-1].node), NULL); }
#line 1534 "parser.tab.c"
    break;

  case 18: /* block: DELIM_LBRACE statements DELIM_RBRACE  */
#line 197 "parser.y"
    { (yyval.node) = createNode("block", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1540 "parser.tab.c"
    break;

  case 19: /* block: DELIM_LBRACE DELIM_RBRACE  */
#line 200 "parser.y"
    { (yyval.node) = createNode("block", NULL, NULL, NULL, NULL); }
#line 1546 "parser.tab.c"
    break;

  case 20: /* statements: statement  */
#line 204 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1552 "parser.tab.c"
    break;

  case 21: /* statements: statements statement  */
#line 207 "parser.y"
    { (yyval.node) = createNode("statements", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1558 "parser.tab.c"
    break;

  case 36: /* delim_statement: DELIM_SEMI  */
#line 229 "parser.y"
    { (yyval.node) = createNode("empty", NULL, NULL, NULL, NULL); }
#line 1564 "parser.tab.c"
    break;

  case 37: /* declaration: type_specifier ID DELIM_SEMI  */
#line 234 "parser.y"
    {
        (yyval.node) = createNode("decl", NULL,
                createNode("type", (yyvsp[-2].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL);
    }
#line 1574 "parser.tab.c"
    break;

  case 38: /* declaration: type_specifier ID OP_ASSIGN expression DELIM_SEMI  */
#line 241 "parser.y"
    {
        (yyval.node) = createNode("decl_assign", NULL,
                createNode("type", (yyvsp[-4].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-3].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1584 "parser.tab.c"
    break;

  case 39: /* declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 248 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1594 "parser.tab.c"
    break;

  case 40: /* declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 255 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1603 "parser.tab.c"
    break;

  case 41: /* declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK OP_ASSIGN matrix_initializer DELIM_SEMI  */
#line 262 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-9].str),
                createNode("rows", (yyvsp[-7].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-4].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1613 "parser.tab.c"
    break;

  case 42: /* declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK OP_ASSIGN array_initializer DELIM_SEMI  */
#line 269 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-6].str),
                createNode("size", (yyvsp[-4].str), NULL, NULL, NULL), NULL, (yyvsp[-1].node));
    }
#line 1622 "parser.tab.c"
    break;

  case 43: /* assignment: ID OP_ASSIGN expression DELIM_SEMI  */
#line 278 "parser.y"
    { (yyval.node) = createNode("assign", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1628 "parser.tab.c"
    break;

  case 44: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 281 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("array_access", (yyvsp[-6].str), (yyvsp[-4].node), NULL, NULL), (yyvsp[-1].node), NULL);
    }
#line 1637 "parser.tab.c"
    break;

  case 45: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 287 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("matrix_access", (yyvsp[-9].str), (yyvsp[-7].node), (yyvsp[-4].node), NULL), (yyvsp[-1].node), NULL);
    }
#line 1646 "parser.tab.c"
    break;

  case 46: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block  */
#line 295 "parser.y"
    { (yyval.node) = createNode("if", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1652 "parser.tab.c"
    break;

  case 47: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE block  */
#line 298 "parser.y"
    { (yyval.node) = createNode("if_else", NULL, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1658 "parser.tab.c"
    break;

  case 48: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE if_statement  */
#line 301 "parser.y"
    {
        (yyval.node) = createNode("if_else", NULL,
                createNode("if", NULL, (yyvsp[-4].node), (yyvsp[-2].node), NULL), (yyvsp[0].node), NULL);
    }
#line 1667 "parser.tab.c"
    break;

  case 49: /* for_loop: KEYWORD_FOR DELIM_LPAREN assignment_expr DELIM_SEMI expression DELIM_SEMI assignment_expr DELIM_RPAREN block  */
#line 309 "parser.y"
    {
        (yyval.node) = createNode("for_body", NULL, 
                createNode("for", NULL, (yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node)), (yyvsp[0].node), NULL); 
    }
#line 1676 "parser.tab.c"
    break;

  case 50: /* while_loop: KEYWORD_WHILE DELIM_LPAREN expression DELIM_RPAREN block  */
#line 317 "parser.y"
    { (yyval.node) = createNode("while", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1682 "parser.tab.c"
    break;

  case 51: /* switch_statement: KEYWORD_SWITCH DELIM_LPAREN expression DELIM_RPAREN DELIM_LBRACE case_list_opt DELIM_RBRACE  */
#line 322 "parser.y"
    { (yyval.node) = createNode("switch", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1688 "parser.tab.c"
    break;

  case 52: /* case_list_opt: %empty  */
#line 327 "parser.y"
    { (yyval.node) = NULL; }
#line 1694 "parser.tab.c"
    break;

  case 53: /* case_list_opt: case_list  */
#line 330 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1700 "parser.tab.c"
    break;

  case 54: /* case_list: case_statement  */
#line 335 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1706 "parser.tab.c"
    break;

  case 55: /* case_list: case_list case_statement  */
#line 338 "parser.y"
    { (yyval.node) = createNode("cases", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1712 "parser.tab.c"
    break;

  case 56: /* case_list: case_list default_case  */
#line 341 "parser.y"
    { (yyval.node) = createNode("cases", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1718 "parser.tab.c"
    break;

  case 57: /* case_statement: KEYWORD_CASE constant DELIM_COLON statements  */
#line 346 "parser.y"
    { (yyval.node) = createNode("case", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1724 "parser.tab.c"
    break;

  case 58: /* default_case: KEYWORD_DEFAULT DELIM_COLON statements  */
#line 351 "parser.y"
    { (yyval.node) = createNode("default", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1730 "parser.tab.c"
    break;

  case 59: /* break_statement: KEYWORD_BREAK DELIM_SEMI  */
#line 357 "parser.y"
    { (yyval.node) = createNode("break", NULL, NULL, NULL, NULL); }
#line 1736 "parser.tab.c"
    break;

  case 60: /* continue_statement: KEYWORD_CONTINUE DELIM_SEMI  */
#line 362 "parser.y"
    { (yyval.node) = createNode("continue", NULL, NULL, NULL, NULL); }
#line 1742 "parser.tab.c"
    break;

  case 61: /* return_statement: KEYWORD_RETURN expression DELIM_SEMI  */
#line 367 "parser.y"
    { (yyval.node) = createNode("return", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1748 "parser.tab.c"
    break;

  case 62: /* return_statement: KEYWORD_RETURN DELIM_SEMI  */
#line 370 "parser.y"
    { (yyval.node) = createNode("return", NULL, NULL, NULL, NULL); }
#line 1754 "parser.tab.c"
    break;

  case 63: /* print_statement: KEYWORD_PRINT DELIM_LPAREN print_args DELIM_RPAREN DELIM_SEMI  */
#line 375 "parser.y"
    { (yyval.node) = createNode("print", NULL, (yyvsp[-2].node), NULL, NULL); }
#line 1760 "parser.tab.c"
    break;

  case 64: /* print_args: print_arg  */
#line 380 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1766 "parser.tab.c"
    break;

  case 65: /* print_args: print_args DELIM_COMMA print_arg  */
#line 383 "parser.y"
    { (yyval.node) = createNode("print_args", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1772 "parser.tab.c"
    break;

  case 66: /* print_arg: STRING_CONST  */
#line 388 "parser.y"
    { (yyval.node) = createNode("string", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1778 "parser.tab.c"
    break;

  case 67: /* print_arg: ID  */
#line 391 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1784 "parser.tab.c"
    break;

  case 68: /* print_arg: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 394 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1790 "parser.tab.c"
    break;

  case 69: /* print_arg: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 397 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1796 "parser.tab.c"
    break;

  case 70: /* scan_statement: KEYWORD_SCAN DELIM_LPAREN lvalue DELIM_RPAREN DELIM_SEMI  */
#line 402 "parser.y"
    { (yyval.node) = createNode("scan", NULL, (yyvsp[-2].node), NULL, NULL); }
#line 1802 "parser.tab.c"
    break;

  case 71: /* lvalue: ID  */
#line 407 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1808 "parser.tab.c"
    break;

  case 72: /* lvalue: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 410 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1814 "parser.tab.c"
    break;

  case 73: /* lvalue: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 413 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1820 "parser.tab.c"
    break;

  case 74: /* assignment_expr: ID OP_ASSIGN expression  */
#line 418 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("id", (yyvsp[-2].str), NULL, NULL, NULL), (yyvsp[0].node), NULL);
    }
#line 1829 "parser.tab.c"
    break;

  case 75: /* assignment_expr: ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression  */
#line 424 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("array_access", (yyvsp[-5].str), (yyvsp[-3].node), NULL, NULL), (yyvsp[0].node), NULL);
    }
#line 1838 "parser.tab.c"
    break;

  case 76: /* assignment_expr: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression  */
#line 430 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("matrix_access", (yyvsp[-8].str), (yyvsp[-6].node), (yyvsp[-3].node), NULL), (yyvsp[0].node), NULL);
    }
#line 1847 "parser.tab.c"
    break;

  case 77: /* assignment_expr: inc_dec_expr  */
#line 436 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1853 "parser.tab.c"
    break;

  case 78: /* inc_dec_expr: OP_INC ID  */
#line 442 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, createNode("id", (yyvsp[0].str), NULL, NULL, NULL), NULL, NULL); }
#line 1859 "parser.tab.c"
    break;

  case 79: /* inc_dec_expr: OP_DEC ID  */
#line 445 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, createNode("id", (yyvsp[0].str), NULL, NULL, NULL), NULL, NULL); }
#line 1865 "parser.tab.c"
    break;

  case 80: /* inc_dec_expr: OP_INC ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 448 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL), NULL, NULL); }
#line 1871 "parser.tab.c"
    break;

  case 81: /* inc_dec_expr: OP_DEC ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 451 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL), NULL, NULL); }
#line 1877 "parser.tab.c"
    break;

  case 82: /* inc_dec_expr: OP_INC ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 454 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL), NULL, NULL); }
#line 1883 "parser.tab.c"
    break;

  case 83: /* inc_dec_expr: OP_DEC ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 457 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL), NULL, NULL); }
#line 1889 "parser.tab.c"
    break;

  case 84: /* inc_dec_expr: ID OP_INC  */
#line 461 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL, NULL); }
#line 1895 "parser.tab.c"
    break;

  case 85: /* inc_dec_expr: ID OP_DEC  */
#line 464 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL, NULL); }
#line 1901 "parser.tab.c"
    break;

  case 86: /* inc_dec_expr: ID DELIM_LBRACK expression DELIM_RBRACK OP_INC  */
#line 467 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, createNode("array_access", (yyvsp[-4].str), (yyvsp[-2].node), NULL, NULL), NULL, NULL); }
#line 1907 "parser.tab.c"
    break;

  case 87: /* inc_dec_expr: ID DELIM_LBRACK expression DELIM_RBRACK OP_DEC  */
#line 470 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, createNode("array_access", (yyvsp[-4].str), (yyvsp[-2].node), NULL, NULL), NULL, NULL); }
#line 1913 "parser.tab.c"
    break;

  case 88: /* inc_dec_expr: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_INC  */
#line 473 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, createNode("matrix_access", (yyvsp[-7].str), (yyvsp[-5].node), (yyvsp[-2].node), NULL), NULL, NULL); }
#line 1919 "parser.tab.c"
    break;

  case 89: /* inc_dec_expr: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_DEC  */
#line 476 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, createNode("matrix_access", (yyvsp[-7].str), (yyvsp[-5].node), (yyvsp[-2].node), NULL), NULL, NULL); }
#line 1925 "parser.tab.c"
    break;

  case 90: /* expression: logical_or_expression  */
#line 483 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1931 "parser.tab.c"
    break;

  case 91: /* logical_or_expression: logical_and_expression  */
#line 488 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1937 "parser.tab.c"
    break;

  case 92: /* logical_or_expression: logical_or_expression OP_OR logical_and_expression  */
#line 491 "parser.y"
    { (yyval.node) = createNode("or", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1943 "parser.tab.c"
    break;

  case 93: /* logical_and_expression: equality_expression  */
#line 496 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1949 "parser.tab.c"
    break;

  case 94: /* logical_and_expression: logical_and_expression OP_AND equality_expression  */
#line 498 "parser.y"
    { (yyval.node) = createNode("and", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1955 "parser.tab.c"
    break;

  case 95: /* equality_expression: relational_expression  */
#line 503 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1961 "parser.tab.c"
    break;

  case 96: /* equality_expression: equality_expression OP_EQ relational_expression  */
#line 506 "parser.y"
    { (yyval.node) = createNode("eq", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1967 "parser.tab.c"
    break;

  case 97: /* equality_expression: equality_expression OP_NE relational_expression  */
#line 509 "parser.y"
    { (yyval.node) = createNode("ne", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1973 "parser.tab.c"
    break;

  case 98: /* relational_expression: additive_expression  */
#line 514 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1979 "parser.tab.c"
    break;

  case 99: /* relational_expression: relational_expression OP_LT additive_expression  */
#line 517 "parser.y"
    { (yyval.node) = createNode("lt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1985 "parser.tab.c"
    break;

  case 100: /* relational_expression: relational_expression OP_GT additive_expression  */
#line 520 "parser.y"
    { (yyval.node) = createNode("gt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1991 "parser.tab.c"
    break;

  case 101: /* relational_expression: relational_expression OP_LE additive_expression  */
#line 523 "parser.y"
    { (yyval.node) = createNode("le", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1997 "parser.tab.c"
    break;

  case 102: /* relational_expression: relational_expression OP_GE additive_expression  */
#line 526 "parser.y"
    { (yyval.node) = createNode("ge", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2003 "parser.tab.c"
    break;

  case 103: /* additive_expression: multiplicative_expression  */
#line 531 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2009 "parser.tab.c"
    break;

  case 104: /* additive_expression: additive_expression OP_PLUS multiplicative_expression  */
#line 534 "parser.y"
    { (yyval.node) = createNode("+", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2015 "parser.tab.c"
    break;

  case 105: /* additive_expression: additive_expression OP_MINUS multiplicative_expression  */
#line 537 "parser.y"
    { (yyval.node) = createNode("-", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2021 "parser.tab.c"
    break;

  case 106: /* multiplicative_expression: unary_expression  */
#line 542 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2027 "parser.tab.c"
    break;

  case 107: /* multiplicative_expression: multiplicative_expression OP_MUL unary_expression  */
#line 545 "parser.y"
    { (yyval.node) = createNode("*", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2033 "parser.tab.c"
    break;

  case 108: /* multiplicative_expression: multiplicative_expression OP_DIV unary_expression  */
#line 548 "parser.y"
    { (yyval.node) = createNode("/", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2039 "parser.tab.c"
    break;

  case 109: /* multiplicative_expression: multiplicative_expression OP_MOD unary_expression  */
#line 551 "parser.y"
    { (yyval.node) = createNode("%", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2045 "parser.tab.c"
    break;

  case 110: /* unary_expression: postfix_expression  */
#line 556 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2051 "parser.tab.c"
    break;

  case 111: /* unary_expression: OP_NOT unary_expression  */
#line 559 "parser.y"
    { (yyval.node) = createNode("not", NULL, (yyvsp[0].node), NULL, NULL); }
#line 2057 "parser.tab.c"
    break;

  case 112: /* unary_expression: OP_MINUS unary_expression  */
#line 562 "parser.y"
    { (yyval.node) = createNode("uminus", NULL, (yyvsp[0].node), NULL, NULL); }
#line 2063 "parser.tab.c"
    break;

  case 113: /* unary_expression: OP_INC unary_expression  */
#line 565 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, (yyvsp[0].node), NULL, NULL); }
#line 2069 "parser.tab.c"
    break;

  case 114: /* unary_expression: OP_DEC unary_expression  */
#line 568 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, (yyvsp[0].node), NULL, NULL); }
#line 2075 "parser.tab.c"
    break;

  case 115: /* postfix_expression: primary_expression  */
#line 573 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2081 "parser.tab.c"
    break;

  case 116: /* postfix_expression: postfix_expression OP_SIZE  */
#line 576 "parser.y"
    { (yyval.node) = createNode("size", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2087 "parser.tab.c"
    break;

  case 117: /* postfix_expression: postfix_expression OP_SORT  */
#line 579 "parser.y"
    { (yyval.node) = createNode("sort", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2093 "parser.tab.c"
    break;

  case 118: /* postfix_expression: postfix_expression OP_MATTRANSPOSE  */
#line 582 "parser.y"
    { (yyval.node) = createNode("transpose", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2099 "parser.tab.c"
    break;

  case 119: /* postfix_expression: postfix_expression OP_MATDET  */
#line 585 "parser.y"
    { (yyval.node) = createNode("det", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2105 "parser.tab.c"
    break;

  case 120: /* postfix_expression: postfix_expression OP_MATINV  */
#line 588 "parser.y"
    { (yyval.node) = createNode("inv", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2111 "parser.tab.c"
    break;

  case 121: /* postfix_expression: postfix_expression OP_MATSHAPE  */
#line 591 "parser.y"
    { (yyval.node) = createNode("shape", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2117 "parser.tab.c"
    break;

  case 122: /* postfix_expression: postfix_expression OP_MATADD DELIM_LPAREN expression DELIM_RPAREN  */
#line 594 "parser.y"
    { (yyval.node) = createNode("matadd", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 2123 "parser.tab.c"
    break;

  case 123: /* postfix_expression: postfix_expression OP_MATSUB DELIM_LPAREN expression DELIM_RPAREN  */
#line 597 "parser.y"
    { (yyval.node) = createNode("matsub", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 2129 "parser.tab.c"
    break;

  case 124: /* postfix_expression: postfix_expression OP_MATMUL DELIM_LPAREN expression DELIM_RPAREN  */
#line 600 "parser.y"
    { (yyval.node) = createNode("matmul", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 2135 "parser.tab.c"
    break;

  case 125: /* postfix_expression: postfix_expression OP_INC  */
#line 603 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2141 "parser.tab.c"
    break;

  case 126: /* postfix_expression: postfix_expression OP_DEC  */
#line 606 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2147 "parser.tab.c"
    break;

  case 128: /* primary_expression: ID  */
#line 612 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2153 "parser.tab.c"
    break;

  case 129: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 615 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 2159 "parser.tab.c"
    break;

  case 130: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 618 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 2165 "parser.tab.c"
    break;

  case 131: /* primary_expression: DELIM_LPAREN expression DELIM_RPAREN  */
#line 621 "parser.y"
    { (yyval.node) = (yyvsp[-1].node); }
#line 2171 "parser.tab.c"
    break;

  case 132: /* constant: INT_CONST  */
#line 626 "parser.y"
    { (yyval.node) = createNode("int", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2177 "parser.tab.c"
    break;

  case 133: /* constant: DECIMAL_CONST  */
#line 629 "parser.y"
    { (yyval.node) = createNode("decimal", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2183 "parser.tab.c"
    break;

  case 134: /* constant: CHAR_CONST  */
#line 632 "parser.y"
    { (yyval.node) = createNode("char", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2189 "parser.tab.c"
    break;

  case 135: /* constant: STRING_CONST  */
#line 635 "parser.y"
    { (yyval.node) = createNode("string", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2195 "parser.tab.c"
    break;

  case 136: /* array_initializer: DELIM_LBRACE array_init_list_opt DELIM_RBRACE  */
#line 640 "parser.y"
    { (yyval.node) = (yyvsp[-1].node); }
#line 2201 "parser.tab.c"
    break;

  case 137: /* array_init_list_opt: %empty  */
#line 645 "parser.y"
    { (yyval.node) = NULL; }
#line 2207 "parser.tab.c"
    break;

  case 138: /* array_init_list_opt: array_init_list  */
#line 648 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2213 "parser.tab.c"
    break;

  case 139: /* array_init_list: expression  */
#line 653 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2219 "parser.tab.c"
    break;

  case 140: /* array_init_list: array_init_list DELIM_COMMA expression  */
#line 656 "parser.y"
    { (yyval.node) = createNode("init_list", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2225 "parser.tab.c"
    break;

  case 141: /* matrix_initializer: DELIM_LBRACE matrix_row_list_opt DELIM_RBRACE  */
#line 661 "parser.y"
    { (yyval.node) = (yyvsp[-1].node); }
#line 2231 "parser.tab.c"
    break;

  case 142: /* matrix_row_list_opt: %empty  */
#line 666 "parser.y"
    { (yyval.node) = NULL; }
#line 2237 "parser.tab.c"
    break;

  case 143: /* matrix_row_list_opt: matrix_row_list  */
#line 669 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2243 "parser.tab.c"
    break;

  case 144: /* matrix_row_list: matrix_row  */
#line 674 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 2249 "parser.tab.c"
    break;

  case 145: /* matrix_row_list: matrix_row_list DELIM_COMMA matrix_row  */
#line 677 "parser.y"
    { (yyval.node) = createNode("init_rows", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 2255 "parser.tab.c"
    break;

  case 146: /* matrix_row: DELIM_LBRACE array_init_list_opt DELIM_RBRACE  */
#line 682 "parser.y"
    { (yyval.node) = createNode("init_row", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 2261 "parser.tab.c"
    break;


#line 2265 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 685 "parser.y"


ASTNode* createNode(char *type, char *value, ASTNode *left, ASTNode *right, ASTNode *third)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value;
    node->line = line_no;
    node->left = left;
    node->right = right;
    node->third = third;
    return node;
}

static void printIndentToFile(FILE *out, int level)
{
    for (int i = 0; i < level; i++)
        fprintf(out, "|   ");
}

static void printASTToFile(FILE *out, ASTNode* node, int level)
{
    if (node == NULL)
        return;

    printIndentToFile(out, level);

    if (node->value)
        fprintf(out, "|-- %s (%s)\n", node->type, node->value);
    else
        fprintf(out, "|-- %s\n", node->type);

    printASTToFile(out, node->left, level + 1);
    printASTToFile(out, node->right, level + 1);
    printASTToFile(out, node->third, level + 1);
}

void printAST(ASTNode* node, int level)
{
    printASTToFile(stdout, node, level);
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_no, s);
}

int main(int argc, char **argv) {
    int parse_ok;
    int semantic_ok = 0;
    IROptLevel opt_level = IR_OPT_O1;
    FILE *tokens_out = NULL;
    FILE *ast_out = NULL;
    FILE *symbol_out = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-O0") == 0) {
            opt_level = IR_OPT_O0;
        } else if (strcmp(argv[i], "-O1") == 0) {
            opt_level = IR_OPT_O1;
        } else if (strcmp(argv[i], "-O2") == 0) {
            opt_level = IR_OPT_O2;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            fprintf(stderr, "Supported options: -O0, -O1, -O2\n");
            return 1;
        }
    }

    tokens_out = fopen("tokens.txt", "w");
    if (!tokens_out) {
        perror("fopen tokens.txt");
        return 1;
    }
    set_token_output(tokens_out);

    parse_ok = (yyparse() == 0);
    fclose(tokens_out);
    set_token_output(NULL);

    if (parse_ok) {
        ast_out = fopen("ast.txt", "w");
        if (!ast_out) {
            perror("fopen ast.txt");
            return 1;
        }
        printASTToFile(ast_out, root, 0);
        fclose(ast_out);

        symbol_out = fopen("symbol.txt", "w");
        if (!symbol_out) {
            perror("fopen symbol.txt");
            return 1;
        }
        fprintf(symbol_out, "Symbol Table\n");
        fprintf(symbol_out, "------------\n");
        semantic_set_symbol_output(symbol_out);
        semantic_ok = semantic_analysis(root);
        semantic_set_symbol_output(NULL);
        fclose(symbol_out);

        if (semantic_ok) {
            FILE *ir_out = fopen("output.ir", "w");
            if (!ir_out) {
                perror("fopen output.ir");
                return 1; /* or YYABORT / appropriate error path */
            }

            generate_ir_level(root, ir_out, opt_level);
            fclose(ir_out);
        }
    }
    return (parse_ok && semantic_ok) ? 0 : 1;
}
