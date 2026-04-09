/*
 * riscv.h
 * -------
 * RISC-V (RV32IM) code generation backend.
 *
 * Input  : output.ir   – textual TAC IR produced by ir.c / generate_ir()
 * Output : output.s    – RV32IM assembly (bare-metal / Spike / QEMU)
 *
 * Register conventions (temporary-only allocation):
 *   t0-t5  (x5-x7, x28-x30)  general scratch
 *   a0     (x10)              return value / ecall arg
 *   a7     (x17)              ecall number
 *   sp     (x2)               stack pointer (never modified in body)
 *
 * Calling-convention note:
 *   The entire program is emitted as a single `main` function.
 *   All variables live in the .data section (global allocation) so
 *   load/store patterns always work regardless of scope depth.
 */

#ifndef RISCV_H
#define RISCV_H

#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Top-level entry point                                               */
/* ------------------------------------------------------------------ */

/*
 * generate_riscv(ir_file, asm_file)
 *
 * Reads the textual IR from `ir_file`, performs a two-pass translation,
 * and writes RV32IM assembly to `asm_file`.
 *
 * Returns 0 on success, -1 on error.
 */
int generate_riscv(const char *ir_file, const char *asm_file);

/* ------------------------------------------------------------------ */
/*  Instruction-level generators (used internally; exposed for testing) */
/* ------------------------------------------------------------------ */

/* Emit .data section declarations for all collected symbols */
void generate_data_section(FILE *out);

/* Emit .text header and main: prologue */
void generate_text_header(FILE *out);

/* Emit program exit (li a7,10 / ecall) */
void generate_exit(FILE *out);

/* Translate a single IR line to RISC-V assembly */
void generate_instruction(const char *ir_line, FILE *out);

/* Individual instruction families */
void generate_assignment(const char *dst, const char *src, FILE *out);
void generate_arithmetic(const char *dst, const char *op,
                         const char *s1,  const char *s2, FILE *out);
void generate_compare(const char *dst, const char *op,
                      const char *s1,  const char *s2, FILE *out);
void generate_if(const char *cond_temp, const char *label, FILE *out);
void generate_goto(const char *label, FILE *out);
void generate_label(const char *label, FILE *out);
void generate_array_load(const char *dst, const char *addr_temp, FILE *out);
void generate_array_store(const char *addr_temp, const char *src, FILE *out);
void generate_print(const char *src, FILE *out);
void generate_scan(const char *addr_temp, FILE *out);
void generate_return(const char *src, FILE *out);   /* src may be NULL */

#endif /* RISCV_H */
