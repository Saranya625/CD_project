# `ir.c` Optimization Guide

This document explains the optimizations implemented in `ir.c`, what each one does, why it helps, and how the code is designed to apply it.

The important thing to know up front is that `ir.c` optimizes in two different places:

1. During IR generation itself.
   The emitter tries to avoid generating unnecessary IR in the first place.
2. After IR generation.
   `optimize_ir()` rewrites already-generated IR lines with simple cleanup passes.

Because of that, some optimizations happen on the AST while IR is being emitted, and others happen on the textual IR after it has already been captured.

---

## 1. Core Design Approach

The optimizer is built around a few reusable ideas:

### 1.1 Property tracking for matrices

`MatrixInfo` stores matrix dimensions plus structural facts:

- `is_zero`
- `is_identity`
- `is_diagonal`
- `is_symmetric`
- `is_upper_triangular`
- `is_lower_triangular`

These facts let later code replace generic matrix algorithms with cheaper special cases.

### 1.2 Constant environment for scalar values

`ConstBinding` tracks known constant values of scalar variables. This lets the emitter:

- fold constant expressions,
- collapse constant `if`/`switch` conditions,
- keep only constants that remain valid after branching.

### 1.3 Expression caches

There are two caches:

- `ExprCache` for scalar temp expressions
- `MatExprCache` for matrix-valued computations

These caches avoid recomputing identical expressions when the inputs have not changed.

### 1.4 Capture-and-rewrite IR pipeline

IR is first emitted into `ctx->lines`, then `optimize_ir()` runs cleanup passes over the saved lines before writing final output.

That means the design is:

- high-level simplification first,
- low-level IR cleanup second.

---

## 2. Optimization Pipeline

### 2.1 What actually runs today

`optimize_ir()` currently runs these passes:

- `apply_jump_threading`
- `apply_jump_simplify`
- `apply_unreachable_elim`
- `apply_propagation`

### 2.2 What is implemented but not currently called

These helpers exist in `ir.c`, but `optimize_ir()` does not invoke them right now:

- `apply_dead_named_elim`
- `apply_dead_temp_elim`
- `emit_matrix_inverse_diagonal`
- `emit_matrix_inverse_triangular`

So they are real implemented optimizations/helpers, but they are not active in the current main optimization pipeline.

---

## 3. Scalar Expression Optimizations

## 3.1 Full constant folding

### Where

- `emit_expr_internal`
- `eval_const_expr`
- `fold_int_binop`

### What it does

If an expression can be fully evaluated at compile time, the emitter generates a constant temp instead of runtime operations.

### Example

Source idea:

```c
int x;
x = 2 + 3 * 4;
```

Instead of generating:

```text
t1 = const int 3
t2 = const int 4
t3 = * t1, t2
t4 = const int 2
t5 = + t4, t3
store [x], t5
```

it can collapse to:

```text
t1 = const int 14
store [x], t1
```

### Design approach

`emit_expr_internal()` calls `eval_const_expr()` before emitting normal code. If folding succeeds, it immediately emits a constant temp through `emit_const_value()` and stops there.

This is the earliest and cleanest optimization in the file because it prevents extra temps and operations from being emitted at all.

---

## 3.2 Scalar algebraic identity simplification

### Where

- `emit_expr_internal`
- `apply_propagation`

### Rules implemented

- `x + 0 -> x`
- `0 + x -> x`
- `x - 0 -> x`
- `0 - x -> -x`
- `x * 0 -> 0`
- `x * 1 -> x`
- `1 * x -> x`
- `x * -1 -> -x`
- `-1 * x -> -x`
- `x / 1 -> x`
- `x / -1 -> -x`
- `x * 2 -> x + x`

### Example

```c
y = a * 1;
z = b * 2;
```

becomes conceptually:

```text
y = a
z = b + b
```

### Design approach

There are two layers:

1. `emit_expr_internal()` performs AST-level simplification before IR is emitted.
2. `apply_propagation()` performs the same style of cleanup later on textual IR lines when values become known only after earlier rewrites.

That two-stage design is useful because some simplifications are visible early, while others only become visible after propagation has rewritten temps.

---

## 3.3 Unary constant folding

### Where

- `emit_expr_internal`
- `apply_propagation`

### What it does

If `uminus` or `not` is applied to a compile-time constant, the result is computed immediately.

### Example

```c
x = -(5);
y = !(0);
```

becomes:

```text
x = -5
y = 1
```

### Design approach

The emitter checks whether the operand is constant. If yes, it emits a constant temp instead of a unary instruction.

---

## 3.4 Scalar common subexpression elimination

### Where

- `ExprCache`
- `emit_expr_internal`

### What it does

If the exact same scalar expression appears again and the inputs have not been invalidated, the old temp is reused.

### Example

```c
a = (x + y);
b = (x + y);
```

The second expression can reuse the temp from the first one instead of generating another add.

### Design approach

`expr_key(node)` builds a canonical key for the expression tree. `emit_expr_internal()` checks the cache before generating code. If a match exists, it returns the cached temp immediately.

This is local, lightweight CSE driven by expression structure rather than a full dataflow framework.

---

## 4. Control-Flow Optimizations

## 4.1 Constant-folded `if` and `if-else`

### Where

- `emit_if_else_chain`

### What it does

If the condition is compile-time known, only the reachable branch is emitted.

### Example

```c
if (1) {
    a = 10;
} else {
    a = 20;
}
```

Only the `then` branch is emitted.

### Design approach

`emit_if_else_chain()` evaluates the condition using `eval_const_expr()`. If the result is known, it directly emits just the chosen branch and skips labels and conditional jumps entirely.

When the condition is not constant, it still does something smart: it clones the constant environment for both branches and intersects the surviving known constants at merge points.

That means the optimizer preserves facts that remain true after both branches.

---

## 4.2 Constant-folded `switch`

### Where

- `emit_switch`
- `find_const_case`

### What it does

If the switch expression is constant, only the matching case body is emitted.

### Example

```c
switch (2) {
case 1: x = 10; break;
case 2: x = 20; break;
default: x = 30;
}
```

This becomes only:

```c
x = 20;
```

### Design approach

`emit_switch()` first tries `eval_const_expr()` on the controlling expression. If successful, `find_const_case()` searches the AST for the matching case and emits only that body.

---

## 4.3 Constant-folded loop conditions

### Where

- `emit_for_body`
- `emit_while`

### What it does

- `for (...; false; ...)` emits no loop body.
- `while (0)` emits nothing.
- literal-true loop conditions avoid generating the extra condition check each iteration.

### Example

```c
while (0) {
    x = x + 1;
}
```

No loop IR is emitted.

### Design approach

The code only folds loops when it is safe. In particular, `emit_while()` only folds literal constant conditions and avoids folding variable-based conditions that might accidentally change semantics.

That is a conservative design choice to avoid turning a runtime loop into a wrong compile-time infinite loop.

---

## 4.4 Small-count `for` loop unrolling

### Where

- `emit_for_body_unrolled`

### What it does

If a `for` loop has:

- a recognizable induction variable,
- constant start/bound/step,
- no `break` or `continue`,
- no modification of the loop variable inside the body,
- iteration count `<= 8`,

then the loop is fully unrolled.

### Example

```c
for (i = 0; i < 3; i++) {
    x = x + 1;
}
```

becomes three explicit copies of the body plus updates, with no loop labels or branches.

### Design approach

The implementation is nicely staged:

- `extract_loop_init()`
- `extract_loop_condition()`
- `extract_loop_step()`
- `compute_unroll_count()`

Only if all of those succeed does `emit_for_body_unrolled()` emit repeated bodies directly.

This is a clean pattern-matching based unroller, not a general loop optimizer.

---

## 5. Post-Generation IR Cleanup Passes

## 5.1 Jump threading

### Where

- `apply_jump_threading`

### What it does

If a jump targets a label whose first real instruction is another unconditional jump, the jump is redirected to the final destination.

### Example

```text
goto L1
...
label L1
goto L2
```

becomes:

```text
goto L2
```

### Design approach

The pass first builds a label-to-line index using `build_label_index()`. Then for each `goto`, it follows chains of jump-only labels and rewrites the source jump to point to the ultimate label.

This reduces useless control-flow hops.

---

## 5.2 Jump-to-next-label removal

### Where

- `apply_jump_simplify`

### What it does

If a `goto Lx` is immediately followed by `label Lx`, the jump is deleted.

### Example

```text
goto L5
label L5
```

becomes just:

```text
label L5
```

### Design approach

This is a simple peephole pass over neighboring lines. After removing matches, it compacts the line array.

---

## 5.3 Unreachable code elimination after unconditional jumps

### Where

- `apply_unreachable_elim`

### What it does

After a `goto`, any non-label instructions until the next label are deleted.

### Example

```text
goto L3
t9 = const int 5
t10 = + t1, t2
label L3
```

The two temp lines are unreachable and are removed.

### Design approach

The pass scans linearly with a `skip` flag. After it sees a `goto`, it deletes everything until a label resets reachability.

---

## 5.4 Copy propagation, constant propagation, and local store forwarding

### Where

- `apply_propagation`

### What it does

This is the biggest textual IR cleanup pass. It performs several optimizations together:

- resolves chains like `t5 = mov t4`, `t6 = mov t5`
- propagates constants through temps
- turns loads from known constant variables into constants
- forwards the most recent stored temp into a later load
- rewrites uses to refer to the resolved temp directly
- removes repeated identical stores to the same named location

### Example 1: copy propagation

```text
t2 = mov t1
t3 = mov t2
print t3
```

becomes effectively:

```text
print t1
```

### Example 2: constant propagation

```text
t1 = const int 5
t2 = mov t1
t3 = + t2, t2
```

becomes:

```text
t2 = const int 5
t3 = const int 10
```

### Example 3: load forwarding

```text
store [t7], t4
t8 = load x
```

If `t7` is known to be the address of `x`, this can become:

```text
t8 = mov t4
```

### Design approach

`apply_propagation()` keeps:

- `TempInfo` for each temp,
- `NameInfo` for named variables,
- an address-to-name map built by `build_addr_name_map()`.

That lets it reason about:

- which temp is a constant,
- which temp is really just another temp,
- which address temp points to which named variable,
- which variable was most recently stored.

This pass is local and label-sensitive: when a label is encountered, temp facts are cleared to avoid unsound cross-branch assumptions.

That is an important correctness choice.

---

## 6. Matrix Metadata and Property Inference

## 6.1 Matrix declaration collection

### Where

- `collect_matrix_decls`

### What it does

Before emission begins, the code records every declared matrix and array with its dimensions.

### Why it matters

Nearly every matrix optimization needs shape information such as:

- rows,
- cols,
- whether a matrix is square.

Without this pre-pass, later specializations would not know if a transformation is legal.

---

## 6.2 Property inference from matrix initializers

### Where

- `emit_matrix_decl`
- `matrix_props_from_init`
- `matrix_props_visit_rows`
- `matrix_props_visit_list`
- `matrix_collect_init`

### What it does

When a matrix is declared with a constant initializer, the code infers structural properties immediately.

### Example

```c
matrix I[3][3] = {
  {1,0,0},
  {0,1,0},
  {0,0,1}
};
```

The matrix is marked as:

- identity,
- diagonal,
- symmetric,
- upper triangular,
- lower triangular.

### Design approach

The implementation walks initializer rows and elements and checks:

- whether off-diagonal entries are zero,
- whether diagonal entries are one,
- whether upper/lower triangle constraints hold,
- whether mirrored entries are equal for symmetry.

This property engine is what unlocks most later matrix shortcuts.

---

## 6.3 Property propagation through assignments

### Where

- `emit_assignment`
- `matrix_props_copy`
- `matrix_props_set`
- `matrix_props_unknown`

### What it does

After a matrix optimization runs, the destination matrix’s known properties are updated.

### Example

- Copying an identity matrix preserves identity.
- Adding two upper-triangular matrices preserves upper-triangular.
- Multiplying two diagonal matrices preserves diagonal.

### Design approach

Every fast-path in `emit_assignment()` not only emits better IR, but also updates metadata for the destination. This is important because one optimization often enables another later one.

This is a chain-enabling design.

---

## 7. Matrix Operation Optimizations

## 7.1 Hoisted address math for add/sub

### Where

- `emit_matrix_add_sub`

### What it does

Instead of recomputing:

```text
base + (row * cols + col) * 4
```

for every element, it computes:

- `cols_bytes = cols * 4`
- `row_off = row * cols_bytes`
- row bases for `a`, `b`, and `dst`
- `col_off = col * 4`

and then just adds row base plus column offset.

### Example

```c
c = a.add(b);
```

### Why it helps

The row component is reused across the whole inner loop, so fewer multiplies and adds are emitted per element.

### Design approach

The emitter explicitly creates:

- `base_a`, `base_b`, `base_dst`
- `cols_bytes`
- `row_base_*`
- `col_off`

That is classic loop-invariant code motion, but done manually in the emitter.

---

## 7.2 Hoisted address math for copy

### Where

- `emit_matrix_copy`

### What it does

Uses the same row-base strategy as add/sub, but for copying one matrix into another.

### Example

```c
b = a;
```

### Why it helps

Copy is common because many other optimizations reduce expensive operations into a simple copy. Making copy itself cheap matters.

---

## 7.3 Hoisted address math for transpose

### Where

- `emit_matrix_transpose`

### What it does

Precomputes source and destination byte strides so transpose does not recompute full index formulas for every access.

### Example

```c
b = a.T;
```

### Design approach

It keeps separate source and destination row widths because transpose swaps dimensions.

---

## 7.4 Reduced address recomputation in matrix multiply

### Where

- `emit_matrix_mul`

### What it does

The generic matrix multiply still uses three nested loops, but it reduces address arithmetic inside the hot inner loop.

### Key ideas

- `inner_bytes = inner * 4`
- `cols_bytes = cols * 4`
- `row_base_a = base_a + row * inner_bytes`
- `addr_a = row_base_a + k * 4`
- `addr_b = base_b + k * cols_bytes + col * 4`

### Example

```c
c = a.mul(b);
```

### Why it helps

Matrix multiply is the most arithmetic-heavy matrix operation. Saving address operations inside the `k` loop reduces both temp count and runtime work.

---

## 7.5 Inverse normalization using one reciprocal per pivot row

### Where

- `emit_matrix_inverse`

### What it does

Instead of dividing every value in the pivot row by `pivot`, it computes:

```text
inv_pivot = 1 / pivot
```

once, then multiplies each row element by `inv_pivot`.

### Example

If a pivot row has three columns, the naive version would do six divisions while normalizing `A` and `Inv`. This version does one division and then multiplications.

### Why it helps

Division is more expensive than multiplication.

### Design approach

This is a classic strength-reduction optimization embedded directly into the Gauss-Jordan code emitter.

---

## 8. Matrix Algebraic Simplifications

## 8.1 `A + 0 -> A`

### Where

- `emit_assignment`

### Example

```c
B = A.add(Z);
```

if `Z` is known zero, the emitter generates a matrix copy instead of an add loop.

### Design approach

`emit_assignment()` checks `matrix_is_zero(right_info)` for add/sub cases and redirects to `emit_matrix_copy()`.

---

## 8.2 `A - 0 -> A`

### Where

- `emit_assignment`

### Example

```c
B = A.sub(Z);
```

becomes a copy.

---

## 8.3 `A * 0 -> 0`

### Where

- `emit_assignment`
- `emit_matrix_zero_fill`

### Example

```c
B = A.mul(Z);
```

If either side is a known zero matrix, the result is emitted with a zero-fill loop instead of full multiplication.

---

## 8.4 `A * I -> A` and `I * A -> A`

### Where

- `emit_assignment`

### Example

```c
B = A.mul(I);
```

becomes a copy of `A`.

### Design approach

The emitter checks `matrix_is_identity(left_info/right_info)` before falling back to generic multiply.

---

## 8.5 `transpose(transpose(A)) -> A`

### Where

- `emit_assignment`

### Example

```c
B = transpose(transpose(A));
```

becomes a copy of `A`, or an in-place transpose if destination equals source and the shape allows it.

### Design approach

`emit_assignment()` pattern-matches nested transpose nodes and bypasses full transpose generation.

---

## 8.6 `transpose(symmetric_matrix) -> same matrix`

### Where

- `emit_assignment`

### Example

```c
T = S.T;
```

If `S` is known symmetric, the transpose is just a copy of `S`.

If destination and source are the same square matrix, nothing needs to change logically.

### Why it helps

It avoids a complete transpose loop based on a structural fact already known.

---

## 8.7 `det(I) -> 1`

### Where

- `emit_expr_internal`

### Example

```c
x = I.det;
```

becomes a constant `1`.

---

## 9. Matrix Shape-Based Specializations

## 9.1 1x1 matrices treated as scalars

### Where

- `emit_matrix_scalar_copy`
- `emit_matrix_scalar_binop`
- used from `emit_assignment`

### Example

```c
matrix a[1][1], b[1][1], c[1][1];
c = a.add(b);
```

Instead of nested loops, the emitter does:

- load one element,
- compute one scalar result,
- store one element.

### Design approach

This is a very cheap structural optimization triggered entirely by matrix dimensions.

---

## 9.2 Outer-product specialization when inner dimension is 1

### Where

- `emit_matrix_mul_inner1`

### Example

```c
matrix a[3][1];
matrix b[1][4];
matrix c[3][4];
c = a.mul(b);
```

Each output cell is just:

```text
c[r][c] = a[r][0] * b[0][c]
```

There is no `k` loop.

### Why it helps

It removes an entire loop level.

---

## 9.3 Diagonal matrix multiply specialization

### Where

- `emit_matrix_mul_diag_left`
- `emit_matrix_mul_diag_right`

### Example

```c
X = D.mul(A);
Y = A.mul(D);
```

where `D` is diagonal.

### What it means

- left diagonal multiply scales rows,
- right diagonal multiply scales columns.

### Why it helps

The usual inner accumulation disappears because only one diagonal term contributes to each output element.

### Design approach

`emit_assignment()` checks diagonal properties before generic matmul. This is a structural shortcut derived from sparse matrix shape, not from numeric values.

---

## 9.4 Symmetric self-multiplication specialization

### Where

- `emit_matrix_mul_sym_self`

### Example

```c
P = S.mul(S);
```

when `S` is known symmetric and destination is not the same as source.

### Why it helps

The result of `S * S` is also symmetric, so only one triangle needs to be computed and mirrored.

### Design approach

The optimization is enabled only when:

- left and right names are the same,
- the matrix is known symmetric,
- destination is distinct.

That avoids aliasing issues.

---

## 9.5 Diagonal determinant shortcut

### Where

- `emit_matrix_det_diag`
- selected from `emit_expr_internal`

### Example

```c
decimal d = D.det;
```

for diagonal, upper-triangular, or lower-triangular `D`.

### What it does

Computes determinant as the product of diagonal entries only.

### Why it helps

This replaces recursive/cofactor-style determinant logic with a simple loop.

---

## 9.6 Specialized inverse helpers for diagonal and triangular matrices

### Where

- `emit_matrix_inverse_diagonal`
- `emit_matrix_inverse_triangular`

### Important note

These helper implementations exist, but they are not currently called from `emit_assignment()` or `emit_matrix_inverse()`.

### What they are meant to do

- diagonal inverse: invert only diagonal entries and zero the rest
- triangular inverse: compute inverse with forward/back substitution instead of full Gauss-Jordan

### Why they matter

They show the design direction of the optimizer: use structural matrix facts to replace general algorithms with specialized ones.

---

## 10. Matrix Loop-Level Optimizations

## 10.1 Fused triple add

### Where

- `emit_matrix_add3`

### Example

```c
D = A.add(B).add(C);
```

### What it does

Instead of emitting one full loop for `A + B` and then another full loop for `(A + B) + C`, it emits one fused loop that computes:

```text
D[i][j] = A[i][j] + B[i][j] + C[i][j]
```

### Why it helps

- fewer loops,
- fewer temporaries,
- fewer memory passes.

---

## 10.2 Small square matmul unrolling

### Where

- `emit_matrix_mul_unrolled`

### Trigger

- square matrices,
- same size on both sides,
- size is `2`, `3`, or `4`.

### Example

```c
matrix a[2][2], b[2][2], c[2][2];
c = a.mul(b);
```

### What it does

Generates straight-line multiply-add code for each output element instead of loop control code.

### Why it helps

For tiny matrices, loop overhead can be a big fraction of total work. Unrolling trades code size for speed.

---

## 10.3 In-place transpose

### Where

- `emit_matrix_transpose_inplace`

### Example

```c
A = A.T;
```

for square matrices.

### What it does

Only swaps mirrored off-diagonal elements instead of allocating a new matrix and copying everything.

### Why it helps

- no temporary destination matrix,
- only half the matrix is touched,
- diagonal stays untouched.

---

## 11. Matrix Common Subexpression Elimination

## 11.1 Reusing repeated matrix expressions

### Where

- `MatExprCache`
- `emit_assignment`

### What it does

If the same matrix expression such as `A+B`, `A-B`, or `A*B` is requested again and operands have not been invalidated, the previous result is reused by copy instead of recomputing it.

### Example

```c
C = A.add(B);
D = A.add(B);
```

The second assignment can become a copy from `C`.

### Design approach

`mat_expr_key()` builds a key for matrix binary operations. `emit_assignment()` checks the cache before generating a new loop.

---

## 11.2 Reusing repeated matrix inverse

### Where

- `MatExprCache`
- `emit_assignment`

### Example

```c
X = A.inv;
Y = A.inv;
```

The second inverse can become a copy from `X`.

### Design approach

The code uses `mat_unary_key("inv", src_name)` and checks `mat_cse_cache` before calling the expensive inverse generator.

---

## 12. Cleanup Passes Implemented But Not Active

## 12.1 Dead named store elimination

### Where

- `apply_dead_named_elim`

### What it would do

Walk backwards through IR and remove stores to named scalars or matrices that are never read later.

### Example

```text
store [x], t1
store [x], t2
print x
```

The first store is dead if no read occurs between the two stores.

### Design approach

It builds liveness over named objects using:

- declarations,
- load/store detection,
- address-to-name mapping.

### Current status

Implemented, but not called by `optimize_ir()`.

---

## 12.2 Dead temp elimination

### Where

- `apply_dead_temp_elim`

### What it would do

Remove temp definitions whose results are never used, and then recursively remove temps that only fed dead temps.

### Example

```text
t1 = const int 3
t2 = + t1, t1
```

If `t2` is never used, both `t2` and possibly `t1` can be removed depending on whether `t1` has any other uses.

### Design approach

This pass:

- counts temp uses,
- records each temp’s defining line,
- seeds a queue with zero-use temps,
- recursively deletes dead defining lines.

### Current status

Implemented, but not called by `optimize_ir()`.

---

## 13. Summary By Category

Here is the easiest way to remember the design of `ir.c`:

### 13.1 Before IR is emitted

The file tries to:

- fold constants,
- simplify algebra,
- collapse constant branches,
- unroll tiny loops,
- detect matrix structure,
- choose specialized matrix emitters,
- reuse earlier scalar and matrix expressions.

### 13.2 After IR is emitted

The file then tries to:

- shorten jump chains,
- remove useless gotos,
- delete unreachable instructions,
- propagate constants and copies through temps and loads.

### 13.3 Main philosophy

The optimizer is mostly pattern-driven and structure-aware rather than global and heavy-weight.

That means:

- it is easier to reason about,
- it is safer to extend,
- it gets strong wins on common compiler-project patterns,
- especially for matrix code, where shape information enables very effective special cases.

---

## 14. Most Important Takeaway

The biggest design idea in `ir.c` is not one single optimization. It is this combination:

- infer matrix structure early,
- preserve that structure through assignments,
- use structure to choose cheaper emitters,
- then clean up leftover scalar IR afterward.

That layering is why the optimizer can handle both:

- ordinary scalar simplifications like constant folding and copy propagation,
- and domain-specific matrix optimizations like identity elimination, diagonal shortcuts, symmetric shortcuts, and unrolled small matmul.
