#ifndef IR_INTERNAL_H
#define IR_INTERNAL_H

#include <stdio.h>

#include "ir.h"

typedef struct IRContext IRContext;

void ir_run_o0_pipeline(IRContext *ctx);
int ir_should_use_cache_local_matmul(IROptLevel opt_level, int rows, int inner, int cols);
int emit_matrix_mul_cache_locality(IRContext *ctx, const char *dst, const char *a, const char *b,
                                   int rows, int inner, int cols, FILE *out);

void ir_printf(IRContext *ctx, FILE *out, const char *fmt, ...);
int new_temp(IRContext *ctx);
int new_label(IRContext *ctx);
const char *safe_str(const char *s);

#endif
