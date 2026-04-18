# CD Compiler Project Report

## 1. Abstract
This project implements an end-to-end compiler toolchain for a custom language (CD) with scalar, array, and matrix constructs. The compiler includes lexical analysis, parsing, semantic analysis, intermediate representation (IR) generation, IR optimization, and RISC-V code generation. A key strength of the project is domain-aware matrix optimization in IR generation, including structure-based fast paths (identity/zero/diagonal/symmetric/triangular), expression caching, and low-level control-flow simplification. The system produces intermediate artifacts (`tokens.txt`, `ast.txt`, `symbol.txt`, `output.ir`) and a final assembly output (`output.s`) suitable for simulator execution.

## 2. Problem Statement
General educational compilers often focus on scalar programs and basic optimization only. This project addresses:
- Language support for arrays and matrices in addition to scalar types.
- Compile-time semantic checking for bounds, shape, and type safety.
- Optimization of matrix-heavy workloads via specialized transformations.
- Translation from custom IR to RISC-V assembly.

## 3. Objectives
- Design and implement a custom language front-end (`lexer.l`, `parser.y`).
- Enforce static semantics (`semantic.c`) with categorized error reporting.
- Generate three-address style IR (`ir.c`) with selectable optimization levels (`-O0`, `-O1`, `-O2`).
- Implement a backend (`riscv.c`) to emit RV32IM-compatible assembly.
- Validate functionality with a broad test suite (`tests/*.cd`).

## 4. Language Overview
### 4.1 Supported Data Types
- Scalar: `int`, `decimal`, `char`, `string`
- Collection: fixed-size `array`, fixed-size `matrix`

### 4.2 Supported Statements
- Declaration and initialization
- Assignment to scalar/array/matrix elements
- `if`, `if-else`, `for`, `while`, `switch-case-default`
- `break`, `continue`, `return`
- I/O: `print(...)`, `scan(...)`

### 4.3 Matrix/Array Operators
- Array: `.size`, `.sort`
- Matrix: `.add(...)`, `.sub(...)`, `.mul(...)`, `.T`, `.det`, `.inv`, `.shape`

## 5. System Architecture
The compiler pipeline is:

1. **Lexical Analysis** (`lexer.l`)
2. **Syntax Analysis + AST Construction** (`parser.y`)
3. **Semantic Analysis + Symbol Processing** (`semantic.c`)
4. **IR Generation + Optimization** (`ir.c`, `ir_o0.c`, `ir_o2.c`)
5. **RISC-V Code Generation** (`riscv.c`)

### 5.1 Generated Artifacts
When compilation succeeds, the pipeline emits:
- `tokens.txt` (token stream)
- `ast.txt` (AST dump)
- `symbol.txt` (symbol table and semantic phase output)
- `output.ir` (optimized IR)
- `output.s` (RISC-V assembly, via `ir2riscv`)

## 6. Front-End Design
### 6.1 Lexer
The lexer recognizes keywords, identifiers, constants, operators, and delimiters. It supports:
- Single-line comments (`//`)
- Multi-line comments (`/* ... */`)
- Line-aware lexical diagnostics (`Lexical Error at line ...`)

### 6.2 Parser
The parser is implemented in Bison and builds a structured AST. It defines:
- Precedence and associativity for arithmetic/logical operators
- Grammar productions for declarations, statements, and expressions
- Explicit forms for array/matrix access and matrix postfix operations

### 6.3 Semantic Analysis
Semantic analysis performs:
- Scope checks (undeclared use, redeclaration)
- Type compatibility checks (assignment, operators, control conditions)
- Array/matrix index type checks
- Compile-time bounds checks for constant indices
- Matrix shape validation for assignment and operations
- Context validation for `break` / `continue`

Errors are categorized as:
- `Type Error`
- `Scope Error`
- `Runtime Semantic Error`
- `Logical Error`
- `Semantic Error` (general fallback)

## 7. Intermediate Representation (IR)
The compiler lowers AST to a textual, three-address style IR:
- Temporaries: `t1`, `t2`, ...
- Labels: `L1`, `L2`, ...
- One instruction per line between `IR_BEGIN` and `IR_END`

Instruction families include:
- Declarations (`decl`, `decl_array`, `decl_matrix`)
- Arithmetic/logical ops
- Addressing (`addr`, `load`, `store`)
- Branching (`ifz`, `goto`, `label`)
- I/O (`print`, `print_str`, `scan`)
- Matrix operators (`matadd`, `matsub`, `matmul`, `transpose`, `det`, `inv`, etc.)

## 8. Optimization Strategy
Optimizations are applied in two layers:
- **During emission**: avoid generating unnecessary IR.
- **Post-generation**: rewrite and simplify generated IR.

### 8.1 General Scalar/Control Optimizations
- Constant folding and unary fold
- Algebraic identities (`x+0`, `x*1`, etc.)
- Constant condition simplification for `if`/`switch`/loops
- Small-count loop unrolling (pattern-based)
- Jump threading and jump simplification
- Unreachable code elimination
- Copy/constant propagation and local load/store forwarding

### 8.2 Matrix-Centric Optimizations
- Hoisted address arithmetic in add/sub/copy/transpose
- Reduced address recomputation in matrix multiplication
- Reciprocal-based pivot normalization in inverse
- Algebraic identities (`A+0`, `A*I`, `A*0`, `T(T(A))`)
- Structural fast paths:
  - 1x1 scalar treatment
  - inner-dimension=1 outer-product specialization
  - diagonal-left/right multiplication shortcuts
  - symmetric self-multiplication shortcut
  - triangular/diagonal determinant shortcuts
- Matrix common subexpression elimination:
  - repeated `add/sub/mul`
  - repeated `det` and `inv`
- In-place transpose optimization

### 8.3 Optimization Levels
- `-O0`: minimal/no advanced optimization
- `-O1`: default optimization level
- `-O2`: more aggressive optimization paths enabled

## 9. RISC-V Backend
The `ir2riscv` backend translates IR to RV32IM-style assembly with:
- Symbol scan and temporary type inference
- Peephole/propagation simplification pass before emission
- Register reuse cache (LRU over scratch registers)
- Spill support for temporaries into `.data`
- Support for integer and float-like paths based on IR typing
- Syscall-based I/O (`print int/string/char`, `scan int`, `exit`)

Assembly layout:
- `.data`: globals, arrays/matrices, spilled temps, string literals
- `.text`: `main`, translated IR control flow and operations

## 10. Build and Execution
### 10.1 Build
```bash
make
```

This generates:
- `compiler` (front-end + semantic + IR generation)
- `ir2riscv` (IR to RISC-V backend)

### 10.2 Run Pipeline
```bash
./compiler -O1 < tests/test_opt_matrix_unroll2.cd
./ir2riscv output.ir output.s
```

### 10.3 Convenience Target
```bash
make run SRC=tests/test_opt_matrix_unroll2.cd OPT=-O2
```

## 11. Testing and Validation
The project contains **48** `.cd` test programs in `tests/`, covering:
- Valid semantic behavior
- Type mismatches
- Scope violations
- Compile-time bounds failures
- Matrix shape mismatch
- Logical-warning style diagnostics
- Matrix optimization scenarios (unrolling, diagonal, symmetric, triangular, CSE, identities)

Representative tests:
- `tests/valid_semantic.cd`
- `tests/invalid_bounds.cd`
- `tests/invalid_shape.cd`
- `tests/invalid_types.cd`
- `tests/test_opt_matrix_*.cd`

## 12. Key Achievements
- Complete front-end to backend educational compiler flow.
- Strong semantic checking with meaningful categorized diagnostics.
- Rich matrix feature set uncommon in basic compiler assignments.
- Practical IR optimizations with both generic and domain-specific strategies.
- Working RISC-V code emission pipeline for generated IR.

## 13. Limitations
- Language currently supports only `main` (no user-defined functions/procedures).
- Arrays/matrices are fixed-size only (no dynamic allocation).
- Some implemented optimization helpers are present but not always wired into the active optimization pipeline.
- Performance reporting is qualitative in the current repository; no benchmark harness with numeric speedup tables is included.

## 14. Future Work
- Add user-defined functions and parameter passing.
- Extend type system with booleans and richer numeric promotion rules.
- Add benchmark suite with quantitative optimization comparisons (`-O0/-O1/-O2`).
- Integrate currently inactive dead-code/dead-store helpers into default optimization flow.
- Add additional backend targets and automated simulator-driven validation.

## 15. Conclusion
This project successfully demonstrates a full compiler implementation for a custom language with matrix-aware semantics and optimization. The architecture is modular, the semantic stage is robust, and the optimization design combines classic compiler techniques with domain-specific matrix transformations. The resulting system is both educationally strong and practically extensible for future compiler enhancements.
