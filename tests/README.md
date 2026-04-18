# Semantic Analyzer Tests

## Valid case
- `valid_semantic.cd`: should parse and pass semantic analysis.

## Invalid cases
- `invalid_bounds.cd`: should report compile-time bounds failures for `a[5]` and `m[2][0]`.
- `invalid_shape.cd`: should report matrix shape mismatch in assignment (`c` is `2x3`, `a.mul(b)` is `2x2`).
- `invalid_types.cd`: should report type mismatch assigning `string` to `int`.

## Error-category cases
- `error_scope.cd`: should emit `Scope Error` (undeclared identifier).
- `error_type.cd`: should emit `Type Error` (incompatible assignment).
- `error_runtime_semantic.cd`: should emit `Runtime Semantic Error` (`10 / 0`, compile-time bounds failure).
- `error_logical.cd`: should emit `Logical Error` (always-true `if`, never-executed `for`).

## Warning case
- `warn_unused.cd`: should emit a semantic warning for `unused_local` and still succeed compilation.

Run all tests:

```bash
make clean && make
for f in semantic_tests/*.cd; do
  echo "===== $f ====="
  ./compiler < "$f"
  echo
 done
```
