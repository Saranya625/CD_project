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
  YYSYMBOL_KEYWORD_MATRIX = 3,             /* KEYWORD_MATRIX  */
  YYSYMBOL_KEYWORD_ARR = 4,                /* KEYWORD_ARR  */
  YYSYMBOL_KEYWORD_MAIN = 5,               /* KEYWORD_MAIN  */
  YYSYMBOL_KEYWORD_IF = 6,                 /* KEYWORD_IF  */
  YYSYMBOL_KEYWORD_ELSE = 7,               /* KEYWORD_ELSE  */
  YYSYMBOL_KEYWORD_FOR = 8,                /* KEYWORD_FOR  */
  YYSYMBOL_KEYWORD_WHILE = 9,              /* KEYWORD_WHILE  */
  YYSYMBOL_KEYWORD_SWITCH = 10,            /* KEYWORD_SWITCH  */
  YYSYMBOL_KEYWORD_CASE = 11,              /* KEYWORD_CASE  */
  YYSYMBOL_KEYWORD_DEFAULT = 12,           /* KEYWORD_DEFAULT  */
  YYSYMBOL_KEYWORD_BREAK = 13,             /* KEYWORD_BREAK  */
  YYSYMBOL_KEYWORD_CONTINUE = 14,          /* KEYWORD_CONTINUE  */
  YYSYMBOL_KEYWORD_RETURN = 15,            /* KEYWORD_RETURN  */
  YYSYMBOL_KEYWORD_PRINT = 16,             /* KEYWORD_PRINT  */
  YYSYMBOL_KEYWORD_SCAN = 17,              /* KEYWORD_SCAN  */
  YYSYMBOL_OP_EQ = 18,                     /* OP_EQ  */
  YYSYMBOL_OP_NE = 19,                     /* OP_NE  */
  YYSYMBOL_OP_LE = 20,                     /* OP_LE  */
  YYSYMBOL_OP_GE = 21,                     /* OP_GE  */
  YYSYMBOL_OP_LT = 22,                     /* OP_LT  */
  YYSYMBOL_OP_GT = 23,                     /* OP_GT  */
  YYSYMBOL_OP_PLUS = 24,                   /* OP_PLUS  */
  YYSYMBOL_OP_MINUS = 25,                  /* OP_MINUS  */
  YYSYMBOL_OP_MUL = 26,                    /* OP_MUL  */
  YYSYMBOL_OP_DIV = 27,                    /* OP_DIV  */
  YYSYMBOL_OP_MOD = 28,                    /* OP_MOD  */
  YYSYMBOL_OP_INC = 29,                    /* OP_INC  */
  YYSYMBOL_OP_DEC = 30,                    /* OP_DEC  */
  YYSYMBOL_OP_AND = 31,                    /* OP_AND  */
  YYSYMBOL_OP_OR = 32,                     /* OP_OR  */
  YYSYMBOL_OP_NOT = 33,                    /* OP_NOT  */
  YYSYMBOL_OP_ASSIGN = 34,                 /* OP_ASSIGN  */
  YYSYMBOL_OP_SIZE = 35,                   /* OP_SIZE  */
  YYSYMBOL_OP_SORT = 36,                   /* OP_SORT  */
  YYSYMBOL_OP_MATADD = 37,                 /* OP_MATADD  */
  YYSYMBOL_OP_MATSUB = 38,                 /* OP_MATSUB  */
  YYSYMBOL_OP_MATMUL = 39,                 /* OP_MATMUL  */
  YYSYMBOL_OP_MATTRANSPOSE = 40,           /* OP_MATTRANSPOSE  */
  YYSYMBOL_OP_MATDET = 41,                 /* OP_MATDET  */
  YYSYMBOL_OP_MATINV = 42,                 /* OP_MATINV  */
  YYSYMBOL_OP_MATSHAPE = 43,               /* OP_MATSHAPE  */
  YYSYMBOL_DELIM_SEMI = 44,                /* DELIM_SEMI  */
  YYSYMBOL_DELIM_COMMA = 45,               /* DELIM_COMMA  */
  YYSYMBOL_DELIM_COLON = 46,               /* DELIM_COLON  */
  YYSYMBOL_DELIM_LPAREN = 47,              /* DELIM_LPAREN  */
  YYSYMBOL_DELIM_RPAREN = 48,              /* DELIM_RPAREN  */
  YYSYMBOL_DELIM_LBRACE = 49,              /* DELIM_LBRACE  */
  YYSYMBOL_DELIM_RBRACE = 50,              /* DELIM_RBRACE  */
  YYSYMBOL_DELIM_LBRACK = 51,              /* DELIM_LBRACK  */
  YYSYMBOL_DELIM_RBRACK = 52,              /* DELIM_RBRACK  */
  YYSYMBOL_ID = 53,                        /* ID  */
  YYSYMBOL_INT_CONST = 54,                 /* INT_CONST  */
  YYSYMBOL_DECIMAL_CONST = 55,             /* DECIMAL_CONST  */
  YYSYMBOL_CHAR_CONST = 56,                /* CHAR_CONST  */
  YYSYMBOL_STRING = 57,                    /* STRING  */
  YYSYMBOL_UMINUS = 58,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 59,                  /* $accept  */
  YYSYMBOL_program = 60,                   /* program  */
  YYSYMBOL_global_declarations = 61,       /* global_declarations  */
  YYSYMBOL_global_declaration = 62,        /* global_declaration  */
  YYSYMBOL_block = 63,                     /* block  */
  YYSYMBOL_statements = 64,                /* statements  */
  YYSYMBOL_statement = 65,                 /* statement  */
  YYSYMBOL_declaration = 66,               /* declaration  */
  YYSYMBOL_assignment = 67,                /* assignment  */
  YYSYMBOL_if_statement = 68,              /* if_statement  */
  YYSYMBOL_for_loop = 69,                  /* for_loop  */
  YYSYMBOL_while_loop = 70,                /* while_loop  */
  YYSYMBOL_switch_statement = 71,          /* switch_statement  */
  YYSYMBOL_case_list = 72,                 /* case_list  */
  YYSYMBOL_case_statement = 73,            /* case_statement  */
  YYSYMBOL_default_case = 74,              /* default_case  */
  YYSYMBOL_break_statement = 75,           /* break_statement  */
  YYSYMBOL_continue_statement = 76,        /* continue_statement  */
  YYSYMBOL_return_statement = 77,          /* return_statement  */
  YYSYMBOL_print_statement = 78,           /* print_statement  */
  YYSYMBOL_print_args = 79,                /* print_args  */
  YYSYMBOL_print_arg = 80,                 /* print_arg  */
  YYSYMBOL_scan_statement = 81,            /* scan_statement  */
  YYSYMBOL_lvalue = 82,                    /* lvalue  */
  YYSYMBOL_assignment_expr = 83,           /* assignment_expr  */
  YYSYMBOL_expression = 84,                /* expression  */
  YYSYMBOL_logical_or_expression = 85,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 86,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 87,       /* equality_expression  */
  YYSYMBOL_relational_expression = 88,     /* relational_expression  */
  YYSYMBOL_additive_expression = 89,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 90, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 91,          /* unary_expression  */
  YYSYMBOL_postfix_expression = 92,        /* postfix_expression  */
  YYSYMBOL_primary_expression = 93,        /* primary_expression  */
  YYSYMBOL_constant = 94                   /* constant  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   263

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  242

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   313


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
      55,    56,    57,    58
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    85,    85,    88,    93,    96,   102,   109,   115,   120,
     123,   128,   130,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   150,   157,   165,   168,   175,
     183,   186,   189,   197,   205,   210,   211,   215,   216,   220,
     224,   228,   232,   236,   237,   241,   245,   246,   250,   251,
     255,   259,   260,   261,   265,   266,   267,   272,   276,   279,
     284,   286,   291,   294,   297,   302,   305,   308,   311,   314,
     319,   322,   325,   330,   333,   336,   339,   344,   347,   350,
     353,   356,   361,   364,   367,   370,   373,   376,   379,   382,
     385,   388,   391,   394,   399,   400,   403,   406,   409,   414,
     417,   420
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
  "\"end of file\"", "error", "\"invalid token\"", "KEYWORD_MATRIX",
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
  "DECIMAL_CONST", "CHAR_CONST", "STRING", "UMINUS", "$accept", "program",
  "global_declarations", "global_declaration", "block", "statements",
  "statement", "declaration", "assignment", "if_statement", "for_loop",
  "while_loop", "switch_statement", "case_list", "case_statement",
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

#define YYPACT_NINF (-153)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      51,   -51,   -46,   -27,   -12,    35,    54,  -153,    11,    45,
      18,   157,  -153,    53,  -153,    37,    48,    50,   157,   157,
     157,   157,   157,    55,  -153,  -153,  -153,    59,    73,    80,
       8,    65,    17,   145,  -153,    99,  -153,  -153,    69,    91,
      92,     0,  -153,  -153,  -153,  -153,  -153,    72,   157,  -153,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,  -153,  -153,  -153,  -153,   100,   102,   103,
    -153,  -153,  -153,  -153,    50,   104,   110,   105,   109,   116,
     118,   122,   127,   113,   131,   123,   133,   134,  -153,    26,
    -153,    15,  -153,  -153,  -153,  -153,  -153,  -153,  -153,  -153,
    -153,  -153,  -153,  -153,  -153,   136,    80,     8,    65,    65,
      17,    17,    17,    17,   145,   145,  -153,  -153,  -153,   157,
     157,   157,  -153,   129,  -153,   138,   143,   157,   144,   157,
     157,  -153,  -153,  -153,   152,    68,   146,   157,   157,  -153,
    -153,   147,   155,   158,   159,   148,   151,   154,   166,    27,
     171,   168,   169,  -153,  -153,    44,  -153,  -153,   167,   174,
     175,   172,   157,  -153,  -153,  -153,   179,   173,   177,    50,
     157,   157,   157,    50,   178,    68,   182,   157,   186,  -153,
      29,   180,  -153,   183,   187,   226,  -153,   184,   191,  -153,
     227,  -153,  -153,   185,  -153,   157,   157,  -153,   188,  -153,
      -1,    33,   144,   137,    40,  -153,   189,   195,   192,   193,
    -153,  -153,   157,   157,   198,   197,   201,  -153,  -153,   199,
     157,  -153,   207,   204,  -153,   200,    50,    30,    30,  -153,
     202,   157,  -153,   216,  -153,    30,    30,  -153,   209,   157,
    -153,  -153
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     4,     0,     0,
       0,     0,     1,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    99,   100,   101,     0,    57,    58,
      60,    62,    65,    70,    73,    77,    82,    94,     0,     0,
       0,     0,     3,    79,    80,    81,    78,     0,     0,     8,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    92,    93,    83,    84,     0,     0,     0,
      85,    86,    87,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,     0,
      24,     0,    11,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    98,     0,    59,    61,    63,    64,
      68,    69,    66,    67,    71,    72,    74,    75,    76,     0,
       0,     0,     2,     0,     7,     0,     0,     0,     0,     0,
       0,    41,    42,    44,     0,     0,     0,     0,     0,     9,
      12,    96,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    49,     0,    46,    48,    51,     0,
       0,     0,     0,    89,    90,    91,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,     0,     6,     0,     0,    30,    54,     0,     0,    34,
       0,    47,    45,     0,    50,     0,     0,    97,     0,    26,
       0,     0,     0,     0,     0,    37,    52,     0,     0,     0,
      31,    32,     0,     0,     0,     0,     0,    35,    38,     0,
       0,    28,     0,     0,    55,     0,     0,     0,     0,    36,
       0,     0,    25,     0,    33,    39,    40,    53,     0,     0,
      29,    56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -153,  -153,  -153,   245,    -5,  -152,   -90,  -153,  -153,    56,
    -153,  -153,  -153,  -153,    57,  -153,  -153,  -153,  -153,  -153,
    -153,    82,  -153,  -153,    58,   -11,  -153,   205,   208,    42,
      76,    23,    52,  -153,  -153,    60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,     6,     7,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   204,   205,   219,    99,   100,   101,   102,
     155,   156,   103,   159,   150,   157,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      27,   140,     8,    77,    78,    79,    79,     9,    80,    81,
      82,    47,    42,    83,    84,    85,    86,    87,    77,    78,
      10,    79,    11,    80,    81,    82,    52,    53,    83,    84,
      85,    86,    87,    77,    78,    12,    79,   105,    80,    81,
      82,    58,    59,    83,    84,    85,    86,    87,    41,    41,
      88,   203,   216,    89,     1,     2,     3,     1,     2,    13,
     137,   170,    15,   195,    41,   139,    17,   212,    89,   122,
      43,    44,    45,    46,   134,   235,   236,   138,   171,    41,
     196,   114,   115,    89,   213,    54,    55,    56,    57,   175,
     217,    39,   176,    18,   108,   109,    16,    19,    20,    41,
      38,    21,    40,    49,     4,    50,    48,     4,   142,   143,
     144,    51,   116,   117,   118,    22,   148,    74,   151,   152,
     104,    23,    24,    25,    26,   154,   160,   161,    63,    64,
     110,   111,   112,   113,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    75,    76,   140,   140,   119,    18,   120,
     121,   181,    19,    20,   124,   123,    21,   131,   125,   186,
     187,   188,   126,   127,   185,   128,   193,   133,   189,   129,
      22,    60,    61,    62,   130,   132,    23,    24,    25,    26,
     135,   136,    18,   145,   207,   208,    19,    20,   141,   146,
      21,    24,    25,    26,   147,   210,   153,   149,   162,   158,
     166,   224,   225,   163,    22,   167,   164,   165,   168,   230,
      23,    24,    25,    26,   169,   172,   173,   174,   177,   179,
     238,   234,   178,   182,   180,   183,   192,   190,   241,   184,
     194,   199,   197,   200,   198,   202,   201,   206,   203,   221,
     220,   231,   209,   227,   222,   223,   226,   228,   232,   229,
     239,    14,   233,   240,   237,   106,   211,   191,     0,   107,
     214,   218,     0,   215
};

static const yytype_int16 yycheck[] =
{
      11,    91,    53,     3,     4,     6,     6,    53,     8,     9,
      10,    22,    17,    13,    14,    15,    16,    17,     3,     4,
      47,     6,    34,     8,     9,    10,    18,    19,    13,    14,
      15,    16,    17,     3,     4,     0,     6,    48,     8,     9,
      10,    24,    25,    13,    14,    15,    16,    17,    49,    49,
      50,    11,    12,    53,     3,     4,     5,     3,     4,     5,
      34,    34,    51,    34,    49,    50,    48,    34,    53,    74,
      18,    19,    20,    21,    85,   227,   228,    51,    51,    49,
      51,    58,    59,    53,    51,    20,    21,    22,    23,    45,
      50,    54,    48,    25,    52,    53,    51,    29,    30,    49,
      47,    33,    54,    44,    53,    32,    51,    53,   119,   120,
     121,    31,    60,    61,    62,    47,   127,    48,   129,   130,
      48,    53,    54,    55,    56,    57,   137,   138,    29,    30,
      54,    55,    56,    57,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    52,    52,   235,   236,    47,    25,    47,
      47,   162,    29,    30,    44,    51,    33,    44,    53,   170,
     171,   172,    53,    47,   169,    47,   177,    44,   173,    47,
      47,    26,    27,    28,    47,    44,    53,    54,    55,    56,
      47,    47,    25,    54,   195,   196,    29,    30,    52,    51,
      33,    54,    55,    56,    51,   200,    44,    53,    51,    53,
      52,   212,   213,    48,    47,    54,    48,    48,    54,   220,
      53,    54,    55,    56,    48,    44,    48,    48,    51,    44,
     231,   226,    48,    44,    52,    52,    44,    49,   239,    52,
      44,    44,    52,     7,    51,    44,    52,    52,    11,    44,
      51,    34,    54,    46,    52,    52,    48,    46,    44,    50,
      34,     6,    52,    44,    52,    50,   200,   175,    -1,    51,
     202,   204,    -1,   203
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,    53,    60,    61,    62,    53,    53,
      47,    34,     0,     5,    62,    51,    51,    48,    25,    29,
      30,    33,    47,    53,    54,    55,    56,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    47,    54,
      54,    49,    63,    91,    91,    91,    91,    84,    51,    44,
      32,    31,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    48,    52,    52,     3,     4,     6,
       8,     9,    10,    13,    14,    15,    16,    17,    50,    53,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    75,
      76,    77,    78,    81,    48,    84,    86,    87,    88,    88,
      89,    89,    89,    89,    90,    90,    91,    91,    91,    47,
      47,    47,    63,    51,    44,    53,    53,    47,    47,    47,
      47,    44,    44,    44,    84,    47,    47,    34,    51,    50,
      65,    52,    84,    84,    84,    54,    51,    51,    84,    53,
      83,    84,    84,    44,    57,    79,    80,    84,    53,    82,
      84,    84,    51,    48,    48,    48,    52,    54,    54,    48,
      34,    51,    44,    48,    48,    45,    48,    51,    48,    44,
      52,    84,    44,    52,    52,    63,    84,    84,    84,    63,
      49,    80,    44,    84,    44,    34,    51,    52,    51,    44,
       7,    52,    44,    11,    72,    73,    52,    84,    84,    54,
      63,    68,    34,    51,    83,    94,    12,    50,    73,    74,
      51,    44,    52,    52,    84,    84,    48,    46,    46,    50,
      84,    34,    44,    52,    63,    64,    64,    52,    84,    34,
      44,    84
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    59,    60,    60,    61,    61,    62,    62,    62,    63,
      63,    64,    64,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    66,    66,    67,    67,    67,
      68,    68,    68,    69,    70,    71,    71,    72,    72,    73,
      74,    75,    76,    77,    77,    78,    79,    79,    80,    80,
      81,    82,    82,    82,    83,    83,    83,    84,    85,    85,
      86,    86,    87,    87,    87,    88,    88,    88,    88,    88,
      89,    89,    89,    90,    90,    90,    90,    91,    91,    91,
      91,    91,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    93,    93,    93,    93,    93,    94,
      94,    94
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     5,     4,     1,     2,     9,     6,     4,     3,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     9,     6,     4,     7,    10,
       5,     7,     7,     9,     5,     7,     8,     1,     2,     4,
       3,     2,     2,     3,     2,     5,     1,     3,     1,     1,
       5,     1,     4,     7,     3,     6,     9,     1,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     2,     2,
       2,     2,     1,     2,     2,     2,     2,     2,     2,     5,
       5,     5,     2,     2,     1,     1,     4,     7,     3,     1,
       1,     1
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
  case 2: /* program: global_declarations KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block  */
#line 86 "parser.y"
    { (yyval.node) = createNode("program", NULL, (yyvsp[-4].node), (yyvsp[0].node), NULL);    root = (yyval.node); }
#line 1337 "parser.tab.c"
    break;

  case 3: /* program: KEYWORD_MAIN DELIM_LPAREN DELIM_RPAREN block  */
#line 89 "parser.y"
    { (yyval.node) = createNode("program", NULL, NULL, (yyvsp[0].node), NULL);   root = (yyval.node); }
#line 1343 "parser.tab.c"
    break;

  case 4: /* global_declarations: global_declaration  */
#line 94 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1349 "parser.tab.c"
    break;

  case 5: /* global_declarations: global_declarations global_declaration  */
#line 97 "parser.y"
    { (yyval.node) = createNode("global", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1355 "parser.tab.c"
    break;

  case 6: /* global_declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 103 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1365 "parser.tab.c"
    break;

  case 7: /* global_declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 110 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1374 "parser.tab.c"
    break;

  case 8: /* global_declaration: ID OP_ASSIGN expression DELIM_SEMI  */
#line 116 "parser.y"
    { (yyval.node) = createNode("assign", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1380 "parser.tab.c"
    break;

  case 9: /* block: DELIM_LBRACE statements DELIM_RBRACE  */
#line 121 "parser.y"
    { (yyval.node) = createNode("block", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1386 "parser.tab.c"
    break;

  case 10: /* block: DELIM_LBRACE DELIM_RBRACE  */
#line 124 "parser.y"
    { (yyval.node) = createNode("block", NULL, NULL, NULL, NULL); }
#line 1392 "parser.tab.c"
    break;

  case 11: /* statements: statement  */
#line 128 "parser.y"
                { (yyval.node) = (yyvsp[0].node); }
#line 1398 "parser.tab.c"
    break;

  case 12: /* statements: statements statement  */
#line 131 "parser.y"
    { (yyval.node) = createNode("statements", NULL, (yyvsp[-1].node), (yyvsp[0].node), NULL); }
#line 1404 "parser.tab.c"
    break;

  case 25: /* declaration: KEYWORD_MATRIX ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 151 "parser.y"
    {
        (yyval.node) = createNode("matrix_decl", (yyvsp[-7].str),
                createNode("rows", (yyvsp[-5].str), NULL, NULL, NULL),
                createNode("cols", (yyvsp[-2].str), NULL, NULL, NULL), NULL);
    }
#line 1414 "parser.tab.c"
    break;

  case 26: /* declaration: KEYWORD_ARR ID DELIM_LBRACK INT_CONST DELIM_RBRACK DELIM_SEMI  */
#line 158 "parser.y"
    {
        (yyval.node) = createNode("array_decl", (yyvsp[-4].str),
                createNode("size", (yyvsp[-2].str), NULL, NULL, NULL), NULL, NULL);
    }
#line 1423 "parser.tab.c"
    break;

  case 27: /* assignment: ID OP_ASSIGN expression DELIM_SEMI  */
#line 166 "parser.y"
    {(yyval.node) = createNode("assign", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL);}
#line 1429 "parser.tab.c"
    break;

  case 28: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 169 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("array_access", (yyvsp[-6].str), (yyvsp[-4].node), NULL, NULL), (yyvsp[-1].node), NULL);
    }
#line 1438 "parser.tab.c"
    break;

  case 29: /* assignment: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK OP_ASSIGN expression DELIM_SEMI  */
#line 176 "parser.y"
    {
        (yyval.node) = createNode("assign", NULL,
                createNode("matrix_access", (yyvsp[-9].str), (yyvsp[-7].node), (yyvsp[-4].node), NULL), (yyvsp[-1].node), NULL);
    }
#line 1447 "parser.tab.c"
    break;

  case 30: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block  */
#line 184 "parser.y"
    { (yyval.node) = createNode("if", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1453 "parser.tab.c"
    break;

  case 31: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE block  */
#line 187 "parser.y"
    { (yyval.node) = createNode("if_else", NULL, (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 1459 "parser.tab.c"
    break;

  case 32: /* if_statement: KEYWORD_IF DELIM_LPAREN expression DELIM_RPAREN block KEYWORD_ELSE if_statement  */
#line 190 "parser.y"
    {
        (yyval.node) = createNode("if_else", NULL,
                createNode("if", NULL, (yyvsp[-4].node), (yyvsp[-2].node), NULL), (yyvsp[0].node), NULL);
    }
#line 1468 "parser.tab.c"
    break;

  case 33: /* for_loop: KEYWORD_FOR DELIM_LPAREN assignment_expr DELIM_SEMI expression DELIM_SEMI assignment_expr DELIM_RPAREN block  */
#line 198 "parser.y"
    {
        (yyval.node) = createNode("for_body", NULL, 
                createNode("for", NULL, (yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node)), (yyvsp[0].node), NULL); 
    }
#line 1477 "parser.tab.c"
    break;

  case 34: /* while_loop: KEYWORD_WHILE DELIM_LPAREN expression DELIM_RPAREN block  */
#line 206 "parser.y"
    { (yyval.node) = createNode("while", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1483 "parser.tab.c"
    break;

  case 58: /* logical_or_expression: logical_and_expression  */
#line 277 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1489 "parser.tab.c"
    break;

  case 59: /* logical_or_expression: logical_or_expression OP_OR logical_and_expression  */
#line 280 "parser.y"
    { (yyval.node) = createNode("or", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1495 "parser.tab.c"
    break;

  case 60: /* logical_and_expression: equality_expression  */
#line 285 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1501 "parser.tab.c"
    break;

  case 61: /* logical_and_expression: logical_and_expression OP_AND equality_expression  */
#line 287 "parser.y"
    { (yyval.node) = createNode("and", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1507 "parser.tab.c"
    break;

  case 62: /* equality_expression: relational_expression  */
#line 292 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1513 "parser.tab.c"
    break;

  case 63: /* equality_expression: equality_expression OP_EQ relational_expression  */
#line 295 "parser.y"
    { (yyval.node) = createNode("eq", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1519 "parser.tab.c"
    break;

  case 64: /* equality_expression: equality_expression OP_NE relational_expression  */
#line 298 "parser.y"
    { (yyval.node) = createNode("ne", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1525 "parser.tab.c"
    break;

  case 65: /* relational_expression: additive_expression  */
#line 303 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1531 "parser.tab.c"
    break;

  case 66: /* relational_expression: relational_expression OP_LT additive_expression  */
#line 306 "parser.y"
    { (yyval.node) = createNode("lt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1537 "parser.tab.c"
    break;

  case 67: /* relational_expression: relational_expression OP_GT additive_expression  */
#line 309 "parser.y"
    { (yyval.node) = createNode("gt", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1543 "parser.tab.c"
    break;

  case 68: /* relational_expression: relational_expression OP_LE additive_expression  */
#line 312 "parser.y"
    { (yyval.node) = createNode("le", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1549 "parser.tab.c"
    break;

  case 69: /* relational_expression: relational_expression OP_GE additive_expression  */
#line 315 "parser.y"
    { (yyval.node) = createNode("ge", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1555 "parser.tab.c"
    break;

  case 70: /* additive_expression: multiplicative_expression  */
#line 320 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1561 "parser.tab.c"
    break;

  case 71: /* additive_expression: additive_expression OP_PLUS multiplicative_expression  */
#line 323 "parser.y"
    { (yyval.node) = createNode("+", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1567 "parser.tab.c"
    break;

  case 72: /* additive_expression: additive_expression OP_MINUS multiplicative_expression  */
#line 326 "parser.y"
    { (yyval.node) = createNode("-", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1573 "parser.tab.c"
    break;

  case 73: /* multiplicative_expression: unary_expression  */
#line 331 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1579 "parser.tab.c"
    break;

  case 74: /* multiplicative_expression: multiplicative_expression OP_MUL unary_expression  */
#line 334 "parser.y"
    { (yyval.node) = createNode("*", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1585 "parser.tab.c"
    break;

  case 75: /* multiplicative_expression: multiplicative_expression OP_DIV unary_expression  */
#line 337 "parser.y"
    { (yyval.node) = createNode("/", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1591 "parser.tab.c"
    break;

  case 76: /* multiplicative_expression: multiplicative_expression OP_MOD unary_expression  */
#line 340 "parser.y"
    { (yyval.node) = createNode("%", NULL, (yyvsp[-2].node), (yyvsp[0].node), NULL); }
#line 1597 "parser.tab.c"
    break;

  case 77: /* unary_expression: postfix_expression  */
#line 345 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1603 "parser.tab.c"
    break;

  case 78: /* unary_expression: OP_NOT unary_expression  */
#line 348 "parser.y"
    { (yyval.node) = createNode("not", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1609 "parser.tab.c"
    break;

  case 79: /* unary_expression: OP_MINUS unary_expression  */
#line 351 "parser.y"
    { (yyval.node) = createNode("uminus", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1615 "parser.tab.c"
    break;

  case 80: /* unary_expression: OP_INC unary_expression  */
#line 354 "parser.y"
    { (yyval.node) = createNode("pre_inc", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1621 "parser.tab.c"
    break;

  case 81: /* unary_expression: OP_DEC unary_expression  */
#line 357 "parser.y"
    { (yyval.node) = createNode("pre_dec", NULL, (yyvsp[0].node), NULL, NULL); }
#line 1627 "parser.tab.c"
    break;

  case 82: /* postfix_expression: primary_expression  */
#line 362 "parser.y"
    { (yyval.node) = (yyvsp[0].node); }
#line 1633 "parser.tab.c"
    break;

  case 83: /* postfix_expression: postfix_expression OP_SIZE  */
#line 365 "parser.y"
    { (yyval.node) = createNode("size", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1639 "parser.tab.c"
    break;

  case 84: /* postfix_expression: postfix_expression OP_SORT  */
#line 368 "parser.y"
    { (yyval.node) = createNode("sort", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1645 "parser.tab.c"
    break;

  case 85: /* postfix_expression: postfix_expression OP_MATTRANSPOSE  */
#line 371 "parser.y"
    { (yyval.node) = createNode("transpose", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1651 "parser.tab.c"
    break;

  case 86: /* postfix_expression: postfix_expression OP_MATDET  */
#line 374 "parser.y"
    { (yyval.node) = createNode("det", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1657 "parser.tab.c"
    break;

  case 87: /* postfix_expression: postfix_expression OP_MATINV  */
#line 377 "parser.y"
    { (yyval.node) = createNode("inv", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1663 "parser.tab.c"
    break;

  case 88: /* postfix_expression: postfix_expression OP_MATSHAPE  */
#line 380 "parser.y"
    { (yyval.node) = createNode("shape", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1669 "parser.tab.c"
    break;

  case 89: /* postfix_expression: postfix_expression OP_MATADD DELIM_LPAREN expression DELIM_RPAREN  */
#line 383 "parser.y"
    { (yyval.node) = createNode("matadd", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1675 "parser.tab.c"
    break;

  case 90: /* postfix_expression: postfix_expression OP_MATSUB DELIM_LPAREN expression DELIM_RPAREN  */
#line 386 "parser.y"
    { (yyval.node) = createNode("matmul", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1681 "parser.tab.c"
    break;

  case 91: /* postfix_expression: postfix_expression OP_MATMUL DELIM_LPAREN expression DELIM_RPAREN  */
#line 389 "parser.y"
    { (yyval.node) = createNode("matmul", NULL, (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1687 "parser.tab.c"
    break;

  case 92: /* postfix_expression: postfix_expression OP_INC  */
#line 392 "parser.y"
    { (yyval.node) = createNode("post_inc", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1693 "parser.tab.c"
    break;

  case 93: /* postfix_expression: postfix_expression OP_DEC  */
#line 395 "parser.y"
    { (yyval.node) = createNode("post_dec", NULL, (yyvsp[-1].node), NULL, NULL); }
#line 1699 "parser.tab.c"
    break;

  case 95: /* primary_expression: ID  */
#line 401 "parser.y"
    { (yyval.node) = createNode("id", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1705 "parser.tab.c"
    break;

  case 96: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK  */
#line 404 "parser.y"
    { (yyval.node) = createNode("array_access", (yyvsp[-3].str), (yyvsp[-1].node), NULL, NULL); }
#line 1711 "parser.tab.c"
    break;

  case 97: /* primary_expression: ID DELIM_LBRACK expression DELIM_RBRACK DELIM_LBRACK expression DELIM_RBRACK  */
#line 407 "parser.y"
    { (yyval.node) = createNode("matrix_access", (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node), NULL); }
#line 1717 "parser.tab.c"
    break;

  case 98: /* primary_expression: DELIM_LPAREN expression DELIM_RPAREN  */
#line 410 "parser.y"
    { (yyval.node) = (yyvsp[-1].node); }
#line 1723 "parser.tab.c"
    break;

  case 99: /* constant: INT_CONST  */
#line 415 "parser.y"
    { (yyval.node) = createNode("int", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1729 "parser.tab.c"
    break;

  case 100: /* constant: DECIMAL_CONST  */
#line 418 "parser.y"
    { (yyval.node) = createNode("decimal", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1735 "parser.tab.c"
    break;

  case 101: /* constant: CHAR_CONST  */
#line 421 "parser.y"
    { (yyval.node) = createNode("char", (yyvsp[0].str), NULL, NULL, NULL); }
#line 1741 "parser.tab.c"
    break;


#line 1745 "parser.tab.c"

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

#line 424 "parser.y"


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
