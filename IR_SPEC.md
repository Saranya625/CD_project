# Custom IR Specification

This project lowers the AST into a simple, linear three-address style IR. The IR is textual, one instruction per line, and uses explicit temporaries (`tN`) and labels (`LN`).

## Format
- **Temporaries:** `t1`, `t2`, ... are SSA-like scratch values.
- **Labels:** `L1`, `L2`, ... are jump targets.
- **Program markers:** `IR_BEGIN` and `IR_END` delimit output.
- **Arrays/matrices:** address calculations are explicit. Element size is normalized to `1` for readability.
- **Matrix layout:** row-major. Address uses `cols(name)` to denote the number of columns in the declared matrix.

## Instruction Set

### Declarations
- `decl <type> <name>`
- `decl_array <name>, <size>`
- `decl_matrix <name>, <rows>, <cols>`

### Constants and Moves
- `tN = const <type> <value>`
- `tN = mov tM`

### Addressing and Memory
- `tN = addr <name>`  
  Address of scalar or base of array/matrix.
- `tN = load <name>`  
  Load scalar variable value by name.
- `tN = load [tM]`  
  Load from computed address.
- `store [tM], tN`  
  Store to computed address.

### Arithmetic and Logical Ops
- `tN = + tA, tB`
- `tN = - tA, tB`
- `tN = * tA, tB`
- `tN = / tA, tB`
- `tN = % tA, tB`
- `tN = eq tA, tB`
- `tN = ne tA, tB`
- `tN = lt tA, tB`
- `tN = gt tA, tB`
- `tN = le tA, tB`
- `tN = ge tA, tB`
- `tN = and tA, tB`
- `tN = or tA, tB`
- `tN = not tA`
- `tN = uminus tA`

### Array and Matrix Operations
- `tN = size tA`
- `tN = sort tA`
- `tN = transpose tA`
- `tN = det tA`
- `tN = inv tA`
- `tN = shape tA`
- `tN = matadd tA, tB`
- `tN = matsub tA, tB`
- `tN = matmul tA, tB`

### Control Flow
- `label LN`
- `goto LN`
- `ifz tA goto LN`  
  Jump if zero/false.

### I/O and Return
- `print tN`
- `print_str <string>`
- `scan [tN]`
- `ret`
- `ret tN`

## Address Calculation Examples

### Array Access `a[i]`
```
t1 = addr a
t2 = <emit i>
t3 = mul t2, 1
t4 = add t1, t3
t5 = load [t4]
```

### Matrix Access `m[r][c]` (row-major)
```
t1 = addr m
t2 = <emit r>
t3 = <emit c>
t4 = mul t2, cols(m)
t5 = add t4, t3
t6 = mul t5, 1
t7 = add t1, t6
t8 = load [t7]
```
