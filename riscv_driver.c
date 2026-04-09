/*
 * riscv_driver.c
 * --------------
 * Standalone driver for the RISC-V code generator.
 *
 * Usage:
 *   ./riscv_driver [input.ir] [output.s]
 *
 * Defaults:
 *   input  = output.ir
 *   output = output.s
 *
 * Build:
 *   gcc -Wall -o riscv_driver riscv_driver.c riscv.c
 *
 * Full pipeline (no changes to parser.y needed):
 *   ./compiler < test.cd           # produces output.ir
 *   ./riscv_driver                 # produces output.s
 */

#include <stdio.h>
#include "riscv.h"

int main(int argc, char *argv[])
{
    const char *ir_file  = (argc > 1) ? argv[1] : "output.ir";
    const char *asm_file = (argc > 2) ? argv[2] : "output.s";

    printf("RISC-V Code Generator\n");
    printf("  Input  IR  : %s\n", ir_file);
    printf("  Output ASM : %s\n", asm_file);

    if (generate_riscv(ir_file, asm_file) != 0) {
        fprintf(stderr, "Code generation failed.\n");
        return 1;
    }

    printf("Done. Assemble with:\n");
    printf("  riscv32-unknown-elf-as -march=rv32im -mabi=ilp32 "
           "-o output.o output.s\n");
    printf("  riscv32-unknown-elf-ld -o output.elf output.o\n");
    printf("Simulate with:\n");
    printf("  spike --isa=rv32im pk output.elf\n");
    return 0;
}
