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

    extern int line_no;
    extern int yylex();
    void yyerror(const char *s);

    ASTNode* root;

#line 84 "parser.tab.c"

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
  YYSYMBOL_expression = 92,                /* expression  */
  YYSYMBOL_logical_or_expression = 93,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 94,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 95,       /* equality_expression  */
  YYSYMBOL_relational_expression = 96,     /* relational_expression  */
  YYSYMBOL_additive_expression = 97,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 98, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 99,          /* unary_expression  */
  YYSYMBOL_postfix_expression = 100,       /* postfix_expression  */
  YYSYMBOL_primary_expression = 101,       /* primary_expression  */
  YYSYMBOL_constant = 102                  /* constant  */
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
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   295

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  268

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
       0,   100,   100,   106,   115,   118,   124,   131,   138,   145,
     151,   156,   157,   158,   159,   163,   166,   172,   175,   180,
     182,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   203,   208,   215,   222,   229,   238,
     241,   247,   255,   258,   261,   269,   277,   282,   288,   290,
     295,   298,   301,   306,   311,   317,   322,   327,   330,   335,
     340,   343,   348,   353,   358,   361,   364,   369,   375,   381,
     390,   395,   398,   403,   405,   410,   413,   416,   421,   424,
     427,   430,   433,   438,   441,   444,   449,   452,   455,   458,
     463,   466,   469,   472,   475,   480,   483,   486,   489,   492,
     495,   498,   501,   504,   507,   510,   513,   518,   519,   522,
     525,   528,   533,   536,   539,   542
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
  "scan_statement", "lvalue", "assignment_expr", "expression",
  "logical_or_expression", "logical_and_expression", "equality_expression",
  "relational_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "postfix_expression",
  "primary_expression", "constant", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-142)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      58,     5,  -142,  -142,  -142,   -38,   -31,     1,    33,    88,
    -142,   -10,     8,    16,    45,    69,  -142,    98,  -142,    20,
      64,    39,    67,    69,    69,    69,    69,    69,    79,  -142,
    -142,  -142,  -142,    93,   106,   118,    32,    59,    76,   116,
    -142,   154,  -142,  -142,   104,    51,  -142,   103,   101,   108,
    -142,  -142,  -142,  -142,   114,    69,  -142,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
    -142,  -142,  -142,  -142,   120,   126,   127,  -142,  -142,  -142,
    -142,   130,   136,     3,  -142,   134,   142,  -142,   135,   118,
      32,    59,    59,    76,    76,    76,    76,   116,   116,  -142,
    -142,  -142,    69,    69,    69,   139,  -142,  -142,   145,   147,
     160,   164,   165,   167,   157,   171,   180,   169,   170,  -142,
    -142,   -20,   166,  -142,    24,  -142,  -142,  -142,  -142,  -142,
    -142,  -142,  -142,  -142,  -142,  -142,  -142,  -142,   168,  -142,
     172,   177,   178,   181,   155,  -142,   179,   188,    69,   187,
      69,    69,  -142,  -142,  -142,   174,    69,   189,    69,    69,
      42,  -142,  -142,   191,    69,  -142,  -142,  -142,   155,   182,
     190,   193,   183,    12,   197,   200,   201,  -142,    30,  -142,
    -142,   194,   202,   207,   203,    69,  -142,   208,   204,   209,
    -142,   205,   206,   103,    69,    69,    69,   103,   211,    69,
     210,    69,   217,  -142,    14,   218,  -142,  -142,  -142,   212,
     220,   246,  -142,   213,   222,  -142,   256,  -142,  -142,   216,
    -142,    69,    69,  -142,   215,  -142,    -5,    15,   187,    51,
     221,   122,  -142,   219,   228,   223,   224,  -142,  -142,    69,
      69,   225,   231,  -142,   232,  -142,  -142,    69,  -142,   240,
     235,  -142,   229,   103,   155,   155,   230,    69,  -142,   249,
    -142,   155,   155,  -142,   236,    69,  -142,  -142
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    11,    12,    13,    14,     0,     0,     0,     0,     0,
       4,     0,     0,     0,     0,     0,     1,    11,     5,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   112,
     113,   114,   115,     0,    70,    71,    73,    75,    78,    83,
      86,    90,    95,   107,     0,     0,     6,     0,     0,     0,
      92,    93,    94,    91,     0,     0,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,   106,    96,    97,     0,     0,     0,    98,    99,   100,
     101,     0,     0,     0,     3,     0,     0,   111,     0,    72,
      74,    76,    77,    81,    82,    79,    80,    84,    85,    87,
      88,    89,     0,     0,     0,     0,     7,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      18,     0,     0,    33,     0,    19,    32,    21,    22,    23,
      24,    25,    26,    27,    28,    31,    29,    30,     0,     9,
     109,     0,     0,     0,     0,     2,     0,     0,     0,     0,
       0,     0,    55,    56,    58,     0,     0,     0,     0,     0,
       0,    17,    20,     0,     0,   102,   103,   104,     0,    31,
       0,     0,     0,     0,     0,     0,     0,    57,     0,    60,
      62,    64,     0,     0,     0,     0,    35,     0,     0,    31,
      16,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    39,     0,     0,     8,   110,    15,     0,
       0,    42,    67,     0,     0,    46,    48,    61,    59,     0,
      63,     0,     0,    36,     0,    38,     0,     0,     0,     0,
       0,    49,    50,    65,     0,     0,     0,    43,    44,     0,
       0,     0,     0,    47,     0,    51,    52,     0,    40,     0,
       0,    68,     0,     0,     0,     0,     0,     0,    37,     0,
      45,    53,    54,    66,     0,     0,    41,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -142,  -142,  -142,   279,    99,  -142,   -43,  -141,  -122,  -142,
    -142,  -142,    63,  -142,  -142,  -142,  -142,  -142,    61,  -142,
    -142,  -142,  -119,  -142,  -142,    91,  -142,  -142,    65,   -15,
    -142,   234,   237,    72,    60,    86,    50,  -142,  -142,   -44
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     8,     9,    10,   122,   145,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   230,   231,   232,   246,
     133,   134,   135,   136,   178,   179,   137,   182,   174,   180,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      33,    82,   162,   168,    84,   110,   107,     2,     3,     4,
     108,   109,    54,   110,    12,   111,   112,   113,   158,    13,
     114,   115,   116,   117,   118,   169,    14,   107,     2,     3,
       4,   108,   109,    16,   110,   159,   111,   112,   113,    15,
      88,   114,   115,   116,   117,   118,   162,    19,    83,   189,
     194,   119,   221,   239,    59,    60,    83,   120,    45,    20,
     121,     1,     2,     3,     4,     5,     6,   195,    46,   222,
     240,    21,   119,    50,    51,    52,    53,    83,   161,   199,
     185,   121,   200,    61,    62,    63,    64,   141,   142,   143,
     186,    17,     2,     3,     4,     5,     6,    48,    23,    11,
      22,   155,    24,    25,    65,    66,    26,    44,    11,    29,
      30,    31,    32,   261,   262,     7,    47,    99,   100,   101,
      27,    93,    94,    95,    96,    49,    28,    29,    30,    31,
      32,    91,    92,   172,    55,   175,   176,   229,   244,   162,
     162,    56,    57,   183,   184,     7,    67,    68,    69,   188,
     211,    97,    98,    58,   215,    81,    83,    85,   107,     2,
       3,     4,   108,   109,    86,   110,    87,   111,   112,   113,
     205,   102,   114,   115,   116,   117,   118,   103,   104,   212,
     213,   214,   105,   237,   106,   242,   219,    70,    71,   138,
     139,   140,   144,    72,    73,    74,    75,    76,    77,    78,
      79,    80,   146,   119,   147,   152,   234,   235,    83,    23,
     260,   148,   121,    24,    25,   149,   150,    26,   151,   153,
     156,   157,   177,   160,   251,   252,   163,   164,   154,   165,
     166,    27,   256,   167,   170,   193,   190,    28,    29,    30,
      31,    32,   264,   171,   173,   196,   181,   187,   191,   201,
     267,   192,   197,   198,   202,   203,   206,   226,   218,   204,
     207,   209,   210,   208,   216,   220,   223,   224,   225,   227,
     228,   229,   233,   236,   247,   243,   248,   253,   257,   249,
     250,   254,   255,   258,   266,   259,   263,   265,    18,   238,
     217,    89,   245,   241,     0,    90
};

static const yytype_int16 yycheck[] =
{
      15,    45,   124,   144,    47,    10,     3,     4,     5,     6,
       7,     8,    27,    10,     9,    12,    13,    14,    38,    57,
      17,    18,    19,    20,    21,   144,    57,     3,     4,     5,
       6,     7,     8,     0,    10,    55,    12,    13,    14,    38,
      55,    17,    18,    19,    20,    21,   168,    57,    53,   168,
      38,    48,    38,    38,    22,    23,    53,    54,    38,    51,
      57,     3,     4,     5,     6,     7,     8,    55,    48,    55,
      55,    55,    48,    23,    24,    25,    26,    53,    54,    49,
      38,    57,    52,    24,    25,    26,    27,   102,   103,   104,
      48,     3,     4,     5,     6,     7,     8,    58,    29,     0,
      55,   116,    33,    34,    28,    29,    37,     9,     9,    58,
      59,    60,    61,   254,   255,    57,    52,    67,    68,    69,
      51,    61,    62,    63,    64,    58,    57,    58,    59,    60,
      61,    59,    60,   148,    55,   150,   151,    15,    16,   261,
     262,    48,    36,   158,   159,    57,    30,    31,    32,   164,
     193,    65,    66,    35,   197,    51,    53,    56,     3,     4,
       5,     6,     7,     8,    56,    10,    52,    12,    13,    14,
     185,    51,    17,    18,    19,    20,    21,    51,    51,   194,
     195,   196,    52,   226,    48,   229,   201,    33,    34,    55,
      48,    56,    53,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    57,    48,    57,    48,   221,   222,    53,    29,
     253,    51,    57,    33,    34,    51,    51,    37,    51,    48,
      51,    51,    48,    57,   239,   240,    58,    55,    48,    52,
      52,    51,   247,    52,    55,    52,    54,    57,    58,    59,
      60,    61,   257,    55,    57,    48,    57,    56,    58,    55,
     265,    58,    52,    52,    52,    48,    48,    11,    48,    56,
      56,    56,    56,    54,    53,    48,    48,    55,    48,    56,
      48,    15,    56,    58,    55,    54,    48,    52,    38,    56,
      56,    50,    50,    48,    48,    56,    56,    38,     9,   226,
     199,    57,   231,   228,    -1,    58
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    57,    64,    65,
      66,    67,     9,    57,    57,    38,     0,     3,    66,    57,
      51,    55,    55,    29,    33,    34,    37,    51,    57,    58,
      59,    60,    61,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,     9,    38,    48,    52,    58,    58,
      99,    99,    99,    99,    92,    55,    48,    36,    35,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    51,   102,    53,    69,    56,    56,    52,    92,    94,
      95,    96,    96,    97,    97,    97,    97,    98,    98,    99,
      99,    99,    51,    51,    51,    52,    48,     3,     7,     8,
      10,    12,    13,    14,    17,    18,    19,    20,    21,    48,
      54,    57,    67,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    83,    84,    85,    86,    89,    55,    48,
      56,    92,    92,    92,    53,    68,    57,    57,    51,    51,
      51,    51,    48,    48,    48,    92,    51,    51,    38,    55,
      57,    54,    71,    58,    55,    52,    52,    52,    70,    85,
      55,    55,    92,    57,    91,    92,    92,    48,    87,    88,
      92,    57,    90,    92,    92,    38,    48,    56,    92,    85,
      54,    58,    58,    52,    38,    55,    48,    52,    52,    49,
      52,    55,    52,    48,    56,    92,    48,    56,    54,    56,
      56,    69,    92,    92,    92,    69,    53,    88,    48,    92,
      48,    38,    55,    48,    55,    48,    11,    56,    48,    15,
      79,    80,    81,    56,    92,    92,    58,    69,    75,    38,
      55,    91,   102,    54,    16,    81,    82,    55,    48,    56,
      56,    92,    92,    52,    50,    50,    92,    38,    48,    56,
      69,    70,    70,    56,    92,    38,    48,    92
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    63,    64,    64,    65,    65,    66,    66,    66,    66,
      66,    67,    67,    67,    67,    68,    68,    69,    69,    70,
      70,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      71,    71,    71,    71,    72,    73,    73,    73,    73,    74,
      74,    74,    75,    75,    75,    76,    77,    78,    79,    79,
      80,    80,    80,    81,    82,    83,    84,    85,    85,    86,
      87,    87,    88,    89,    90,    90,    90,    91,    91,    91,
      92,    93,    93,    94,    94,    95,    95,    95,    96,    96,
      96,    96,    96,    97,    97,    97,    98,    98,    98,    98,
      99,    99,    99,    99,    99,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   101,   101,   101,
     101,   101,   102,   102,   102,   102
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     6,     5,     1,     2,     3,     5,     9,     6,
       4,     1,     1,     1,     1,     4,     3,     3,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     5,     9,     6,     4,
       7,    10,     5,     7,     7,     9,     5,     7,     0,     1,
       1,     2,     2,     4,     3,     2,     2,     3,     2,     5,
       1,     3,     1,     5,     1,     4,     7,     3,     6,     9,
       1,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       1,     2,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     5,     5,     5,     2,     2,     1,     1,     4,
       7,     3,     1,     1,     1,     1
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
#line 101 "parser.y"
    { 
        (yyval.node) = createNode("program", NULL, (yyvsp[-5].node), (yyvsp[0].node), NULL); 
        root = (yyval.node);  
    }
#line 1365 "parser.tab.c"
    break;

  case 3: /* program: KEYWORD_INT KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block  */
#line 107 "parser.y"
    { 
        (yyval.node) = createNode("program", NULL, NULL, (yyvsp[0].node), NULL); 
        root = (yyval.node);
    }
#line 1374 "parser.tab.c"
    break;

  case 4: /* global_declarations: global_declaration  */
#line 116 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1380 "parser.tab.c"
    break;

  case 5: /* global_declarations: global_declarations global_declaration  */
#line 119 "parser.y"
    { (yyval.node) = createNode("global", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1386 "parser.tab.c"
    break;

  case 6: /* global_declaration: type_specifier ID DELIM_SEMI  */
#line 125 "parser.y"
    {
        (yyval.node) = createNode("decl", NULL,
                createNode("type", (yyvsp[-2].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL);
    }
#line 1396 "parser.tab.c"
    break;

  case 7: /* global_declaration: type_specifier ID OP_ASSIGN constant DELIM_SEMI  */
#line 132 "parser.y"
    {
        (yyval.node) = createNode("decl_assign", NULL,
                createNode("type", (yyvsp[-4].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-3].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1406 "parser.tab.c"
    break;

  case 8: /* global_declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 139 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1416 "parser.tab.c"
    break;

  case 9: /* global_declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 146 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1425 "parser.tab.c"
    break;

  case 10: /* global_declaration: ID OP_ASSIGN expression DELIM_SEMI  */
#line 152 "parser.y"
    { (yyval.node) = createNode("assign", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1431 "parser.tab.c"
    break;

  case 11: /* type_specifier: KEYWORD_INT  */
#line 156 "parser.y"
                        { (yyval.str) = "int"; }
#line 1437 "parser.tab.c"
    break;

  case 12: /* type_specifier: KEYWORD_DECIMAL  */
#line 157 "parser.y"
                        { (yyval.str) = "decimal"; }
#line 1443 "parser.tab.c"
    break;

  case 13: /* type_specifier: KEYWORD_CHAR  */
#line 158 "parser.y"
                        { (yyval.str) = "char"; }
#line 1449 "parser.tab.c"
    break;

  case 14: /* type_specifier: KEYWORD_STRING  */
#line 159 "parser.y"
                        { (yyval.str) = "string"; }
#line 1455 "parser.tab.c"
    break;

  case 15: /* main_block: DELIM_LBRACE statements return_statement DELIM_RBRACE  */
#line 164 "parser.y"
    { (yyval.node) = createNode("main_block", NULL, (yyvsp[-2].node), (yyvsp[-1].node), NULL); }
#line 1461 "parser.tab.c"
    break;

  case 16: /* main_block: DELIM_LBRACE return_statement DELIM_RBRACE  */
#line 167 "parser.y"
    { (yyval.node) = createNode("main_block", NULL, NULL, (yyvsp[-1].node), NULL); }
#line 1467 "parser.tab.c"
    break;

  case 17: /* block: DELIM_LBRACE statements DELIM_RBRACE  */
#line 173 "parser.y"
    { (yyval.node) = createNode("block", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1473 "parser.tab.c"
    break;

  case 18: /* block: DELIM_LBRACE DELIM_RBRACE  */
#line 176 "parser.y"
    { (yyval.node) = createNode("block", NULL, NULL, NULL, NULL); }
#line 1479 "parser.tab.c"
    break;

  case 19: /* statements: statement  */
#line 180 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1485 "parser.tab.c"
    break;

  case 20: /* statements: statements statement  */
#line 183 "parser.y"
    { (yyval.node) = createNode("statements", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1491 "parser.tab.c"
    break;

  case 34: /* delim_statement: DELIM_SEMI  */
#line 204 "parser.y"
    { (yyval.node) = createNode("empty", NULL, NULL, NULL, NULL); }
#line 1497 "parser.tab.c"
    break;

  case 35: /* declaration: type_specifier ID DELIM_SEMI  */
#line 209 "parser.y"
    {
        (yyval.node) = createNode("decl", NULL,
                createNode("type", (yyvsp[-2].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-1].str), NULL, NULL, NULL), NULL);
    }
#line 1507 "parser.tab.c"
    break;

  case 36: /* declaration: type_specifier ID OP_ASSIGN expression DELIM_SEMI  */
#line 216 "parser.y"
    {
        (yyval.node) = createNode("decl_assign", NULL,
                createNode("type", (yyvsp[-4].str), NULL, NULL, NULL),
                createNode("id", (yyvsp[-3].str), NULL, NULL, NULL), (yyvsp[-1].node));
    }
#line 1517 "parser.tab.c"
    break;

  case 37: /* declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 223 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1527 "parser.tab.c"
    break;

  case 38: /* declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 230 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1536 "parser.tab.c"
    break;

  case 39: /* assignment: ID OP_ASSIGN expression DELIM_SEMI  */
#line 239 "parser.y"
    { (yyval.node) = createNode("assign", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1542 "parser.tab.c"
    break;

  case 40: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 242 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("array_access", (yyvsp[-6].str), (yyvsp[-4].node), NULL, NULL), (yyvsp[-1].node), NULL);
    }
#line 1551 "parser.tab.c"
    break;

  case 41: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 248 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("matrix_access", (yyvsp[-9].str), (yyvsp[-7].node), (yyvsp[-4].node), NULL), (yyvsp[-1].node), NULL);
    }
#line 1560 "parser.tab.c"
    break;

  case 42: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block  */
#line 256 "parser.y"
    { (yyval.node) = createNode("if", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1566 "parser.tab.c"
    break;

  case 43: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE block  */
#line 259 "parser.y"
    { (yyval.node) = createNode("if_else", NULL, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1572 "parser.tab.c"
    break;

  case 44: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE if_statement  */
#line 262 "parser.y"
    {
        (yyval.node) = createNode("if_else", NULL,
                createNode("if", NULL, (yyvsp[-4].node), (yyvsp[-2].node), NULL), (yyvsp[0].node), NULL);
    }
#line 1581 "parser.tab.c"
    break;

  case 45: /* for_loop: KEYWORD_FOR DELIM_LPAREN assignment_expr DELIM_SEMI expression DELIM_SEMI assignment_expr DELIM_RPAREN block  */
#line 270 "parser.y"
    {
        (yyval.node) = createNode("for_body", NULL, 
                createNode("for", NULL, (yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node)), (yyvsp[0].node), NULL); 
    }
#line 1590 "parser.tab.c"
    break;

  case 46: /* while_loop: KEYWORD_WHILE DELIM_LPAREN expression DELIM_RPAREN block  */
#line 278 "parser.y"
    { (yyval.node) = createNode("while", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1596 "parser.tab.c"
    break;

  case 47: /* switch_statement: KEYWORD_SWITCH DELIM_LPAREN expression DELIM_RPAREN DELIM_LBRACE case_list_opt DELIM_RBRACE  */
#line 283 "parser.y"
    { (yyval.node) = createNode("switch", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1602 "parser.tab.c"
    break;

  case 48: /* case_list_opt: %empty  */
#line 288 "parser.y"
    { (yyval.node) = NULL; }
#line 1608 "parser.tab.c"
    break;

  case 49: /* case_list_opt: case_list  */
#line 291 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1614 "parser.tab.c"
    break;

  case 50: /* case_list: case_statement  */
#line 296 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1620 "parser.tab.c"
    break;

  case 51: /* case_list: case_list case_statement  */
#line 299 "parser.y"
    { (yyval.node) = createNode("cases", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1626 "parser.tab.c"
    break;

  case 52: /* case_list: case_list default_case  */
#line 302 "parser.y"
    { (yyval.node) = createNode("cases", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1632 "parser.tab.c"
    break;

  case 53: /* case_statement: KEYWORD_CASE constant DELIM_COLON statements  */
#line 307 "parser.y"
    { (yyval.node) = createNode("case", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1638 "parser.tab.c"
    break;

  case 54: /* default_case: KEYWORD_DEFAULT DELIM_COLON statements  */
#line 312 "parser.y"
    { (yyval.node) = createNode("default", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1644 "parser.tab.c"
    break;

  case 55: /* break_statement: KEYWORD_BREAK DELIM_SEMI  */
#line 318 "parser.y"
    { (yyval.node) = createNode("break", NULL, NULL, NULL, NULL); }
#line 1650 "parser.tab.c"
    break;

  case 56: /* continue_statement: KEYWORD_CONTINUE DELIM_SEMI  */
#line 323 "parser.y"
    { (yyval.node) = createNode("continue", NULL, NULL, NULL, NULL); }
#line 1656 "parser.tab.c"
    break;

  case 57: /* return_statement: KEYWORD_RETURN expression DELIM_SEMI  */
#line 328 "parser.y"
    { (yyval.node) = createNode("return", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1662 "parser.tab.c"
    break;

  case 58: /* return_statement: KEYWORD_RETURN DELIM_SEMI  */
#line 331 "parser.y"
    { (yyval.node) = createNode("return", NULL, NULL, NULL, NULL); }
#line 1668 "parser.tab.c"
    break;

  case 59: /* print_statement: KEYWORD_PRINT DELIM_LPAREN print_args DELIM_RPAREN DELIM_SEMI  */
#line 336 "parser.y"
    { (yyval.node) = createNode("print", NULL, (yyvsp[-2].node), NULL, NULL); }
#line 1674 "parser.tab.c"
    break;

  case 60: /* print_args: print_arg  */
#line 341 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1680 "parser.tab.c"
    break;

  case 61: /* print_args: print_args DELIM_COMMA print_arg  */
#line 344 "parser.y"
    { (yyval.node) = createNode("print_args", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1686 "parser.tab.c"
    break;

  case 62: /* print_arg: expression  */
#line 349 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1692 "parser.tab.c"
    break;

  case 63: /* scan_statement: KEYWORD_SCAN DELIM_LPAREN lvalue DELIM_RPAREN DELIM_SEMI  */
#line 354 "parser.y"
    { (yyval.node) = createNode("scan", NULL, (yyvsp[-2].node), NULL, NULL); }
#line 1698 "parser.tab.c"
    break;

  case 64: /* lvalue: ID  */
#line 359 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1704 "parser.tab.c"
    break;

  case 65: /* lvalue: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 362 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1710 "parser.tab.c"
    break;

  case 66: /* lvalue: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 365 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1716 "parser.tab.c"
    break;

  case 67: /* assignment_expr: ID OP_ASSIGN expression  */
#line 370 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("id", (yyvsp[-2].str), NULL, NULL, NULL), (yyvsp[0].node), NULL);
    }
#line 1725 "parser.tab.c"
    break;

  case 68: /* assignment_expr: ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression  */
#line 376 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("array_access", (yyvsp[-5].str), (yyvsp[-3].node), NULL, NULL), (yyvsp[0].node), NULL);
    }
#line 1734 "parser.tab.c"
    break;

  case 69: /* assignment_expr: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression  */
#line 382 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("matrix_access", (yyvsp[-8].str), (yyvsp[-6].node), (yyvsp[-3].node), NULL), (yyvsp[0].node), NULL);
    }
#line 1743 "parser.tab.c"
    break;

  case 70: /* expression: logical_or_expression  */
#line 391 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1749 "parser.tab.c"
    break;

  case 71: /* logical_or_expression: logical_and_expression  */
#line 396 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1755 "parser.tab.c"
    break;

  case 72: /* logical_or_expression: logical_or_expression OP_OR logical_and_expression  */
#line 399 "parser.y"
    { (yyval.node) = createNode("or", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1761 "parser.tab.c"
    break;

  case 73: /* logical_and_expression: equality_expression  */
#line 404 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1767 "parser.tab.c"
    break;

  case 74: /* logical_and_expression: logical_and_expression OP_AND equality_expression  */
#line 406 "parser.y"
    { (yyval.node) = createNode("and", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1773 "parser.tab.c"
    break;

  case 75: /* equality_expression: relational_expression  */
#line 411 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1779 "parser.tab.c"
    break;

  case 76: /* equality_expression: equality_expression OP_EQ relational_expression  */
#line 414 "parser.y"
    { (yyval.node) = createNode("eq", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1785 "parser.tab.c"
    break;

  case 77: /* equality_expression: equality_expression OP_NE relational_expression  */
#line 417 "parser.y"
    { (yyval.node) = createNode("ne", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1791 "parser.tab.c"
    break;

  case 78: /* relational_expression: additive_expression  */
#line 422 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1797 "parser.tab.c"
    break;

  case 79: /* relational_expression: relational_expression OP_LT additive_expression  */
#line 425 "parser.y"
    { (yyval.node) = createNode("lt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1803 "parser.tab.c"
    break;

  case 80: /* relational_expression: relational_expression OP_GT additive_expression  */
#line 428 "parser.y"
    { (yyval.node) = createNode("gt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1809 "parser.tab.c"
    break;

  case 81: /* relational_expression: relational_expression OP_LE additive_expression  */
#line 431 "parser.y"
    { (yyval.node) = createNode("le", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1815 "parser.tab.c"
    break;

  case 82: /* relational_expression: relational_expression OP_GE additive_expression  */
#line 434 "parser.y"
    { (yyval.node) = createNode("ge", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1821 "parser.tab.c"
    break;

  case 83: /* additive_expression: multiplicative_expression  */
#line 439 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1827 "parser.tab.c"
    break;

  case 84: /* additive_expression: additive_expression OP_PLUS multiplicative_expression  */
#line 442 "parser.y"
    { (yyval.node) = createNode("+", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1833 "parser.tab.c"
    break;

  case 85: /* additive_expression: additive_expression OP_MINUS multiplicative_expression  */
#line 445 "parser.y"
    { (yyval.node) = createNode("-", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1839 "parser.tab.c"
    break;

  case 86: /* multiplicative_expression: unary_expression  */
#line 450 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1845 "parser.tab.c"
    break;

  case 87: /* multiplicative_expression: multiplicative_expression OP_MUL unary_expression  */
#line 453 "parser.y"
    { (yyval.node) = createNode("*", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1851 "parser.tab.c"
    break;

  case 88: /* multiplicative_expression: multiplicative_expression OP_DIV unary_expression  */
#line 456 "parser.y"
    { (yyval.node) = createNode("/", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1857 "parser.tab.c"
    break;

  case 89: /* multiplicative_expression: multiplicative_expression OP_MOD unary_expression  */
#line 459 "parser.y"
    { (yyval.node) = createNode("%", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1863 "parser.tab.c"
    break;

  case 90: /* unary_expression: postfix_expression  */
#line 464 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1869 "parser.tab.c"
    break;

  case 91: /* unary_expression: OP_NOT unary_expression  */
#line 467 "parser.y"
    { (yyval.node) = createNode("not", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1875 "parser.tab.c"
    break;

  case 92: /* unary_expression: OP_MINUS unary_expression  */
#line 470 "parser.y"
    { (yyval.node) = createNode("uminus", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1881 "parser.tab.c"
    break;

  case 93: /* unary_expression: OP_INC unary_expression  */
#line 473 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1887 "parser.tab.c"
    break;

  case 94: /* unary_expression: OP_DEC unary_expression  */
#line 476 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1893 "parser.tab.c"
    break;

  case 95: /* postfix_expression: primary_expression  */
#line 481 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1899 "parser.tab.c"
    break;

  case 96: /* postfix_expression: postfix_expression OP_SIZE  */
#line 484 "parser.y"
    { (yyval.node) = createNode("size", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1905 "parser.tab.c"
    break;

  case 97: /* postfix_expression: postfix_expression OP_SORT  */
#line 487 "parser.y"
    { (yyval.node) = createNode("sort", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1911 "parser.tab.c"
    break;

  case 98: /* postfix_expression: postfix_expression OP_MATTRANSPOSE  */
#line 490 "parser.y"
    { (yyval.node) = createNode("transpose", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1917 "parser.tab.c"
    break;

  case 99: /* postfix_expression: postfix_expression OP_MATDET  */
#line 493 "parser.y"
    { (yyval.node) = createNode("det", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1923 "parser.tab.c"
    break;

  case 100: /* postfix_expression: postfix_expression OP_MATINV  */
#line 496 "parser.y"
    { (yyval.node) = createNode("inv", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1929 "parser.tab.c"
    break;

  case 101: /* postfix_expression: postfix_expression OP_MATSHAPE  */
#line 499 "parser.y"
    { (yyval.node) = createNode("shape", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1935 "parser.tab.c"
    break;

  case 102: /* postfix_expression: postfix_expression OP_MATADD DELIM_LPAREN expression DELIM_RPAREN  */
#line 502 "parser.y"
    { (yyval.node) = createNode("matadd", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1941 "parser.tab.c"
    break;

  case 103: /* postfix_expression: postfix_expression OP_MATSUB DELIM_LPAREN expression DELIM_RPAREN  */
#line 505 "parser.y"
    { (yyval.node) = createNode("matsub", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1947 "parser.tab.c"
    break;

  case 104: /* postfix_expression: postfix_expression OP_MATMUL DELIM_LPAREN expression DELIM_RPAREN  */
#line 508 "parser.y"
    { (yyval.node) = createNode("matmul", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1953 "parser.tab.c"
    break;

  case 105: /* postfix_expression: postfix_expression OP_INC  */
#line 511 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1959 "parser.tab.c"
    break;

  case 106: /* postfix_expression: postfix_expression OP_DEC  */
#line 514 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1965 "parser.tab.c"
    break;

  case 108: /* primary_expression: ID  */
#line 520 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1971 "parser.tab.c"
    break;

  case 109: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 523 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1977 "parser.tab.c"
    break;

  case 110: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 526 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1983 "parser.tab.c"
    break;

  case 111: /* primary_expression: DELIM_LPAREN expression DELIM_RPAREN  */
#line 529 "parser.y"
    { (yyval.node) = (yyvsp[-1].node); }
#line 1989 "parser.tab.c"
    break;

  case 112: /* constant: INT_CONST  */
#line 534 "parser.y"
    { (yyval.node) = createNode("int", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1995 "parser.tab.c"
    break;

  case 113: /* constant: DECIMAL_CONST  */
#line 537 "parser.y"
    { (yyval.node) = createNode("decimal", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2001 "parser.tab.c"
    break;

  case 114: /* constant: CHAR_CONST  */
#line 540 "parser.y"
    { (yyval.node) = createNode("char", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2007 "parser.tab.c"
    break;

  case 115: /* constant: STRING_CONST  */
#line 543 "parser.y"
    { (yyval.node) = createNode("string", (yyvsp[0].str), NULL, NULL, NULL); }
#line 2013 "parser.tab.c"
    break;


#line 2017 "parser.tab.c"

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

#line 546 "parser.y"


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
