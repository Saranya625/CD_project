# Lexer and Parser for Custom Programming Language

This project contains a lexical analyzer (using Flex) and a syntax parser (using Bison) for a custom programming language with Python-inspired readability and C-style block delimiters.

## Features

The language supports:
- **Data types**: `int`, `decimal`, `char`, `matrix`
- **Keywords**: `main`, `if`, `else`, `for`, `while`, `switch`, `case`, `default`, `break`, `continue`, `return`, `print`, `scan`
- **Arithmetic Operators**: `+`, `-`, `*`, `/`, `%`
- **Relational Operators**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical Operators**: `&&`, `||`, `!`
- **Assignment**: `=`
- **Delimiters**: `;`, `,`, `:`, `(`, `)`, `{`, `}`, `[`, `]`
- **Literals**: Integer constants, Decimal constants, Character constants, String literals
- **Identifiers**: Variable names
- **Arrays**: Single-dimensional array support (fixed size, zero-indexed)
- **Matrices**: Two-dimensional array support with row-major layout
- **Global Variables**: Variables declared outside main function
- **Comments**: Single-line (`//`) and multi-line (`/* */`)

## Files

- `lexer.l` - Flex lexical analyzer specification
- `parser.y` - Bison parser specification
- `Makefile` - Build configuration
- `test_bubble_sort.c` - Bubble sort sample program from specification
- `test_features.c` - Comprehensive feature demonstration
- `test_globals.c` - Global variables test program

## Prerequisites

You need to have installed:
- `flex` (Fast Lexical Analyzer)
- `bison` (GNU Parser Generator)
- `gcc` (GNU C Compiler)

On Ubuntu/Debian:
```bash
sudo apt-get install flex bison gcc
```

## Building

Simply run:
```bash
make
```

This will:
1. Generate `parser.tab.c` and `parser.tab.h` from `parser.y`
2. Generate `lex.yy.c` from `lexer.l`
3. Compile everything into an executable called `compiler`

## Running

To parse a program:
```bash
./compiler < test_bubble_sort.c
./compiler < test_features.c
./compiler < test_globals.c
```

Or with any other input file:
```bash
./compiler < your_program.c
```

## Cleaning

To remove generated files:
```bash
make clean
```

## Grammar Overview

The parser recognizes programs with the following structure:

```
program:
    [global_declarations]
    int main() { statements }

global_declarations:
    - Type declarations: int, decimal, char
    - Array declarations: type id[size];
    - Matrix declarations: matrix id[rows][cols];
    - With optional initialization

statements:
    - Variable declarations (int, decimal, char, matrix)
    - Array declarations (single and two-dimensional)
    - Assignments (variables, arrays, matrices)
    - If-else statements (with else-if support)
    - For loops (with initialization, condition, increment)
    - While loops
    - Switch-case statements (with default)
    - Break and continue statements
    - Return statements
    - Print statements (supports multiple arguments)
    - Scan statements (for input)

expressions:
    - Constants (integer, decimal, character)
    - Variables
    - Array accesses (single and two-dimensional)
    - String literals
    - Arithmetic operations (+, -, *, /, %)
    - Relational operations (==, !=, <, >, <=, >=)
    - Logical operations (&&, ||, !)
    - Unary minus
    - Parenthesized expressions
```

## Example Program

```c
// Global variables
int max_size = 100;
decimal pi = 3.14159;

int main() {
    // Local declarations
    int x;
    int y = 10;
    int arr[5];
    matrix m[3][3];
    char grade;
    
    // Assignments
    x = 5 + 3;
    arr[0] = x;
    m[0][0] = 1;
    grade = 'A';
    
    // Control structures
    if (x < y && y > 0) {
        print("Condition met");
    } else {
        print("Condition not met");
    }
    
    // Switch statement
    switch (x) {
        case 1:
            print("One");
            break;
        case 2:
            print("Two");
            break;
        default:
            print("Other");
            break;
    }
    
    // For loop
    for (x = 0; x < 10; x = x + 1) {
        if (x == 5) {
            continue;
        }
        print("Value: ", x);
    }
    
    // While loop
    while (y > 0) {
        y = y - 1;
        if (y == 3) {
            break;
        }
    }
    
    // Input/Output
    print("Enter a number:");
    scan(x);
    print("You entered: ", x);
    
    return 0;
}
```

## Output

If the input program is syntactically correct:
```
Starting parser...
Valid program parsed successfully!
Parsing completed successfully.
```

If there are syntax errors, the parser will report the line number and error message.

## Notes

- The program must start with optional global declarations followed by `int main() { ... }`
- All statements must end with a semicolon
- Variable declarations support types: `int`, `decimal`, `char`
- Matrix declarations use the `matrix` keyword
- Arrays are zero-indexed and fixed-size (size must be compile-time constant)
- Braces `{}` are mandatory for all control structures
- Single-line statements without braces are not permitted
- Comments are supported: `//` for single-line, `/* */` for multi-line
- The parser performs syntax checking but does not perform semantic analysis
- Assignment statements do not return values (no chained assignments)
- Print statements can accept multiple comma-separated arguments
- Logical operators: `&&` (AND), `||` (OR), `!` (NOT)
- Modulo operator: `%`
