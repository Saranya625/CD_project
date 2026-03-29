# Matrix Optimizations Added

This file documents the matrix-operation optimizations added in `ir.c`, with
simple examples showing what changes in the generated IR and why it is faster.

## 1) Hoisted Matrix Address Math (Add/Sub/Copy/Transpose)

**Where**
- `emit_matrix_add_sub`
- `emit_matrix_copy`
- `emit_matrix_transpose`

**What changed**
- Compute matrix base addresses once (`addr <matrix>`).
- Compute `cols_bytes = cols * 4` once.
- For each row, compute `row_base = base + row * cols_bytes`.
- For each column, compute `col_off = col * 4`.
- Compute final element address as `row_base + col_off`.

**Simple example code**
```c
int main() {
    matrix a[2][2] = {{1,2},{3,4}};
    matrix b[2][2] = {{5,6},{7,8}};
    matrix c[2][2];
    c = a.add(b);
}
```

**Before (per element)**
```
addr = base + (row * cols + col) * 4
```

**After (per row + per element)**
```
row_base = base + row * (cols * 4)
addr     = row_base + col * 4
```

**Why it helps**
The old address formula recomputed `row*cols`, `*4`, and `base+...` for every
element. Now the row part is computed once per row and reused for each column,
cutting multiplies/adds in the tight loop and reducing temporary values.

---

## 2) Matrix Multiply: Reduced Address Recomputation

**Where**
- `emit_matrix_mul`

**What changed**
- Precompute `inner_bytes = inner * 4` and `cols_bytes = cols * 4`.
- Compute `row_base_a = base_a + row * inner_bytes`.
- Compute `addr_a = row_base_a + k * 4`.
- Compute `addr_b = base_b + k * cols_bytes + col * 4`.
- Precompute `base_a`, `base_b`, `base_dst` once.

**Simple example code**
```c
int main() {
    matrix a[2][3];
    matrix b[3][2];
    matrix c[2][2];
    c = a.mul(b);
}
```

**Before (per k)**
```
addr_a = base_a + (row * inner + k) * 4
addr_b = base_b + (k * cols + col) * 4
```

**After (per k)**
```
row_base_a = base_a + row * (inner * 4)
addr_a     = row_base_a + k * 4
addr_b     = base_b + k * (cols * 4) + col * 4
```

**Why it helps**
Matrix multiply is the hottest path. Hoisting the row base and using byte
strides reduces the number of arithmetic ops needed to compute addresses in
the inner `k` loop. This cuts repeated multiplies/adds for every multiply-add.

---

## 3) Matrix Inverse: Replace Per-Element Division With One Reciprocal

**Where**
- `emit_matrix_inverse`

**What changed**
- Compute `inv_pivot = 1 / pivot` once per pivot row.
- Replace per-element `val / pivot` with `val * inv_pivot`.

**Simple example code**
```c
int main() {
    matrix a[2][2] = {{1,2},{3,5}};
    matrix inva[2][2];
    inva = a.inv;
}
```

**Before (inside inner loop)**
```
new_a   = val_a   / pivot
new_inv = val_inv / pivot
```

**After (inside inner loop)**
```
inv_pivot = 1 / pivot   // once per pivot row
new_a     = val_a * inv_pivot
new_inv   = val_inv * inv_pivot
```

**Why it helps**
Division is more expensive than multiplication. This reduces two divisions per
element into two multiplies, paying only one division per pivot row.

---

## 4) Algebraic Identity Simplifications

**Where**
- `emit_assignment` (matrix assignment fast-paths)
- `emit_expr_internal` (det identity fold)

**What changed**
- `A + 0 -> A` (skip add loop, emit copy)
- `A - 0 -> A` (skip sub loop, emit copy)
- `A * I -> A` and `I * A -> A` (skip mul loop, emit copy)
- `A * 0 -> 0` (emit zero-fill loop)
- `transpose(transpose(A)) -> A` (copy or in-place transpose)
- `det(I) -> 1` (constant fold)

**Simple example code**
```c
int main() {
    matrix I[2][2] = {{1,0},{0,1}};
    matrix Z[2][2] = {{0,0},{0,0}};
    matrix A[2][2];
    matrix B[2][2];
    B = A.add(Z);    // becomes B = A
    B = A.mul(I);    // becomes B = A
}
```

**Why it helps**
These rules avoid generating full nested loops when the result is already
known algebraically. This is a big win for large matrices.

---

## 5) Structural / Shape-Based Optimizations

### 5.1 Inner Dimension = 1 (Outer Product)
**Where**
- `emit_assignment` -> `emit_matrix_mul_inner1`

**Simple example**
```c
matrix a[3][1];
matrix b[1][4];
matrix c[3][4];
c = a.mul(b);
```

**Why it helps**
The `k` loop disappears. Each element is just `a[row][0] * b[0][col]`.

### 5.2 1x1 Matrices -> Scalar Ops
**Where**
- `emit_assignment` -> `emit_matrix_scalar_binop` / `emit_matrix_scalar_copy`

**Example**
```c
matrix a[1][1], b[1][1], c[1][1];
c = a.add(b);
```

**Why it helps**
Replaces the entire matrix loop with one load, one op, one store.

### 5.3 Diagonal Matrices: det/inv shortcuts
**Where**
- `emit_expr_internal` and `emit_assignment`
- `emit_matrix_det_diag`
- `emit_matrix_inverse_diagonal`

**Example**
```c
matrix d[3][3] = {{2,0,0},{0,3,0},{0,0,4}};
decimal x = d.det;   // becomes 2*3*4
matrix inv[3][3];
inv = d.inv;         // only diagonal entries inverted
```

**Why it helps**
Determinant becomes a simple product, and inverse becomes `O(n^2)` instead of
`O(n^3)`.

---

## 6) Loop-Level Optimizations

### 6.1 Fused Elementwise Add (Nested)
**Where**
- `emit_assignment` -> `emit_matrix_add3`

**Example**
```c
matrix d[2][2];
d = a.add(b).add(c);
```

**Why it helps**
Instead of two separate add loops, one loop computes `a + b + c`.

### 6.2 Unrolled Matmul for 2x2, 3x3, and 4x4
**Where**
- `emit_assignment` -> `emit_matrix_mul_unrolled`

**Example**
```c
matrix a[2][2], b[2][2], c[2][2];
c = a.mul(b);
```

**Why it helps**
Generates straight-line arithmetic (no loop overhead) for tiny matrices.

---

## 7) Redundancy Elimination

### 7.1 In-Place Transpose
**Where**
- `emit_assignment` -> `emit_matrix_transpose_inplace`

**Example**
```c
matrix a[3][3];
a = a.T;
```

**Why it helps**
Avoids allocating a temporary matrix and only swaps upper-triangle elements.

### 7.2 Matrix CSE (Common Subexpression Elimination)
**Where**
- `mat_cse_cache` in `IRContext`
- `emit_assignment` (matadd/matsub/matmul)

**Example**
```c
matrix c[2][2], d[2][2];
c = a.add(b);
d = a.add(b);  // reuses c via copy
```

**Why it helps**
If the same matrix expression is computed again without its operands changing,
the result is copied instead of recomputed.

---

## 8) New Matrix Structural Fast Paths

### 8.1 Diagonal x General and General x Diagonal
**Where**
- `emit_assignment` -> `emit_matrix_mul_diag_left` / `emit_matrix_mul_diag_right`

**What changed**
- `D * A` becomes row-scaling: `dst[i][j] = D[i][i] * A[i][j]`
- `A * D` becomes column-scaling: `dst[i][j] = A[i][j] * D[j][j]`
- Removes the `k` loop entirely (no inner multiply-accumulate).

**Example**
```c
matrix D[3][3] = {{2,0,0},{0,3,0},{0,0,4}};
matrix A[3][3];
matrix X[3][3];
matrix Y[3][3];
X = D.mul(A);   // row-scaled
Y = A.mul(D);   // col-scaled
```

### 8.2 Symmetric Matrix Detection & Exploitation
**Where**
- `matrix_props_from_init`
- `emit_assignment` (transpose + matmul)

**What changed**
- Matrices initialized with constant symmetric values are tracked as symmetric.
- `transpose(symmetric)` is a no-op (copy/in-place).
- `S * S` uses only the upper triangle and mirrors the result.

**Example**
```c
matrix S[3][3] = {{1,2,3},{2,4,5},{3,5,6}};
matrix T[3][3];
matrix P[3][3];
T = S.T;        // no-op
P = S.mul(S);   // upper triangle only, mirrored
```

### 8.3 Triangular Determinant
**Where**
- `emit_expr_internal` and `emit_assignment` (det)

**What changed**
- For upper/lower triangular matrices, determinant is the product of the diagonal.

### 8.4 Triangular Inverse
**Where**
- `emit_assignment` -> `emit_matrix_inverse_triangular`

**What changed**
- Upper/lower triangular matrices are inverted using back/forward substitution.
- Reduces complexity from `O(n^3)` to `O(n^2)`.

---

## 9) Dead Matrix Store Elimination

**Where**
- `apply_dead_named_elim`
- `build_addr_name_map`

**What changed**
- Tracks matrix element stores via derived addresses (`addr` + `add`/`mov`).
- If a matrix is never read after, its element stores are removed.

---

## 10) CSE for det and inv

**Where**
- `emit_assignment` (det and inv)

**What changed**
- Reuses the same `det(M)` temp if recomputed.
- Reuses the same `inv(M)` matrix if recomputed.

---

## Summary of Matrix Optimizations in `ir.c`

1. Hoisted address math for add/sub/copy/transpose.
2. Reduced address recomputation in matrix multiply.
3. Replaced per-element division with reciprocal in matrix inverse.
4. Algebraic identity simplifications (zero/identity/double transpose/det identity).
5. Structural optimizations (inner=1 outer product, 1x1 scalar ops, diagonal det/inv).
6. Loop-level optimizations (nested add fusion, 2x2/3x3 matmul unroll).
7. Redundancy elimination (in-place transpose, matrix CSE).
8. Diagonal x general and general x diagonal fast paths.
9. Symmetric detection + symmetric self-mul shortcut.
10. Triangular det and triangular inverse.
11. Dead matrix store elimination.
12. CSE for det and inv.

---

Not yet implemented:
- Full loop tiling/blocking for matmul.
- Scalar broadcast matmul (not expressible in current type system).
- Symmetric determinant via Cholesky (needs sqrt IR op).
