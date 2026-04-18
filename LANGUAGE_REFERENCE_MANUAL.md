# CD Language Reference Manual

## 1. Language Overview
CD is a statically typed, block-structured language with first-class support for:
- Scalar types: `int`, `decimal`, `char`, `string`
- Fixed-size collections: `array`, `matrix`
- Control flow: `if/else`, `for`, `while`, `switch/case/default`
- Built-ins for arrays/matrices through postfix operators (for example `.size`, `.sort`, `.T`, `.mul(...)`)

Programs are compiled through lexer, parser, semantic analysis, and IR generation.

## 2. Program Structure

### 2.1 Entry Point
A program must define `main` with signature:

```cd
int main() { ... }
```

Global declarations may appear before `main`.

### 2.2 Top-Level Grammar (Simplified)
```ebnf
program              ::= global_declarations? "int" "main" "(" ")" block
global_declarations  ::= global_declaration+
global_declaration   ::= declaration
```

## 3. Lexical Specification

### 3.1 Keywords
`int decimal char string array matrix main if else for while switch case default break continue return print scan`

### 3.2 Operators
- Arithmetic: `+ - * / %`
- Assignment: `=`
- Comparison: `== != < > <= >=`
- Logical: `&& || !`
- Increment/decrement: `++ --`
- Postfix built-ins:
` .size .sort .add(expr) .sub(expr) .mul(expr) .det .inv .shape .T`

### 3.3 Delimiters
`; , : ( ) { } [ ]`

### 3.4 Identifiers and Literals
- Identifier: `[a-zA-Z_][a-zA-Z0-9_]*`
- Integer literal: digits
- Decimal literal: digits`.`digits
- Character literal: single-quoted character or escape
- String literal: double-quoted sequence with escapes

### 3.5 Comments and Whitespace
- Single-line comment: `// ...`
- Multi-line comment: `/* ... */`
- Whitespace is ignored except newline tracking for diagnostics.

## 4. Types

### 4.1 Scalar Types
- `int`
- `decimal`
- `char`
- `string`

### 4.2 Collection Types
- `array name[INT_CONST]`
- `matrix name[INT_CONST][INT_CONST]`

Collection dimensions are compile-time constants and must be positive.

## 5. Declarations and Initialization

### 5.1 Scalar Declarations
```cd
int x;
decimal pi = 3.14;
char c = 'A';
string s = "hello";
```

### 5.2 Array Declarations
```cd
array a[5];
array b[4] = {1, 2, 3, 4};
```

### 5.3 Matrix Declarations
```cd
matrix m[2][3];
matrix n[2][2] = {{1, 2}, {3, 4}};
```

Matrix initializer row count and per-row column count must match declared dimensions.

## 6. Statements

### 6.1 Supported Statements
```ebnf
statement ::= declaration
            | assignment
            | inc_dec_expr
            | if_statement
            | for_loop
            | while_loop
            | switch_statement
            | break_statement
            | continue_statement
            | print_statement
            | scan_statement
            | return_statement
            | ";"
            | block
```

### 6.2 Assignment Forms
```cd
x = expr;
a[i] = expr;
m[i][j] = expr;
```

### 6.3 Input/Output
```cd
print("value:", x, a[i], m[i][j]);
scan(x);
scan(a[i]);
scan(m[i][j]);
```

`print` arguments are restricted to string literals, identifiers, and array/matrix element access.

### 6.4 Control Flow
```cd
if (cond) { ... }
if (cond) { ... } else { ... }

for (init; cond; update) { ... }
while (cond) { ... }

switch (expr) {
  case 1: ...;
  default: ...;
}
```

`break` is valid in loops/switch.  
`continue` is valid only in loops.

## 7. Expressions

### 7.1 Precedence (High to Low)
1. Postfix: indexing, `.size`, `.sort`, `.T`, `.det`, `.inv`, `.shape`, `.add(...)`, `.sub(...)`, `.mul(...)`, postfix `++ --`
2. Unary: `!`, unary `-`, prefix `++ --`
3. Multiplicative: `* / %`
4. Additive: `+ -`
5. Relational: `< > <= >=`
6. Equality: `== !=`
7. Logical AND: `&&`
8. Logical OR: `||`
9. Assignment form is used in assignment contexts (`x = expr`, `for` init/update).

### 7.2 Indexing
```cd
a[i]
m[i][j]
```
Index expressions for arrays/matrices must be `int` or `char`.

## 8. Array and Matrix Built-ins

### 8.1 Arrays
- `a.size` returns `int`
- `a.sort` returns `array` (same size)

### 8.2 Matrices
- `m.T` returns transposed matrix
- `m.det` returns `decimal` (square matrix only)
- `m.inv` returns matrix (square matrix only)
- `m.shape` returns `string`
- `m.add(n)` returns matrix (same shape required)
- `m.sub(n)` returns matrix (same shape required)
- `m.mul(n)` returns matrix (`m.cols == n.rows` required)

## 9. Static Semantic Rules

### 9.1 Name and Scope
- Use-before-declare is an error.
- Redeclaration in the same scope is an error.
- Declared variables that are never referenced produce a semantic warning.

### 9.2 Assignment Compatibility
Allowed:
- same type to same type
- `char -> int`
- `char -> decimal`
- `int -> decimal`

Not allowed:
- incompatible scalar types (for example `string -> int`)
- non-matching collection sizes/shapes on assignment

### 9.3 Operator Constraints
- `+ - * /` require numeric operands (`int/decimal/char`)
- `%` requires `int/char` operands
- relational operators require numeric operands
- logical operators require scalar operands
- increment/decrement require assignable numeric lvalues

### 9.4 Control-Flow Conditions
- `if`, `for`, `while` conditions must be scalar.
- `switch` expression must be `int` or `char`.

### 9.5 Compile-Time Checks
- Constant-index bounds checks for arrays/matrices
- Division/modulo by zero detection for constant zero divisors
- Matrix dimension validation (`det`/`inv` need square matrices)

## 10. Diagnostic Categories
Semantic diagnostics are reported with categories:
- `Type Error`
- `Scope Error`
- `Runtime Semantic Error`
- `Logical Error`
- `Semantic Error` (fallback/general)

Non-fatal diagnostics:
- `Semantic warning` for non-blocking issues such as unused variables.

Example messages include:
- type mismatch in assignment
- use of undeclared identifier
- compile-time bounds check failed
- possible runtime crash: division by zero
- for-loop condition is always false; loop never executes

## 11. Representative Examples

### 11.1 Valid
```cd
int main() {
    int i;
    array a[5];
    matrix m[2][3];
    matrix n[3][4];
    matrix p[2][4];

    i = 2;
    a[1 + 1] = 9;
    p = m.mul(n);
    a = a.sort;
    print("ok", a[2], p[1][3]);

    return 0;
}
```

### 11.2 Invalid (Bounds)
```cd
int main() {
    array a[5];
    matrix m[2][2];
    a[5] = 1;
    m[2][0] = 3;
    return 0;
}
```

### 11.3 Invalid (Type)
```cd
int main() {
    int x;
    string s;
    s = "hello";
    x = s;
    return 0;
}
```

## 12. Notes and Current Limitations
- No user-defined functions beyond `main`.
- Arrays and matrices are fixed-size only.
- Element access is scalar (`int`) in semantic analysis.
- `print` does not accept arbitrary expression arguments directly.
