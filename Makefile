# ============================================================
# Makefile — Compiler Project (Weeks 1-9)
# ============================================================
CC     = gcc
CFLAGS = -Wall -Wextra -std=c99

# All source files
SRCS = parser.tab.c lex.yy.c semantic.c ir.c riscv.c
OBJS = $(SRCS:.c=.o)

# Final executable
TARGET = compiler

# ---- Default target ----
all: $(TARGET)

$(TARGET): $(SRCS) ast.h semantic.h ir.h riscv.h
	$(CC) $(CFLAGS) -o $@ $(SRCS) -lfl -lm

# ---- Bison / Flex generation ----
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

# ---- Convenience: run a source file and produce output.s ----
# Usage: make run SRC=test_bubble_sort.cd
run: $(TARGET)
	./$(TARGET) < $(SRC)

# ---- Assemble the generated RISC-V (requires riscv32 toolchain) ----
# Usage: make asm
asm: output.s
	riscv32-unknown-elf-as -march=rv32im -mabi=ilp32 \
	    -o output.o output.s
	riscv32-unknown-elf-ld -o output.elf output.o
	@echo "Assembled: output.elf"

# ---- Simulate on Spike ----
sim: output.elf
	spike --isa=rv32im pk output.elf

# ---- Clean ----
clean:
	rm -f $(TARGET) lex.yy.c parser.tab.c parser.tab.h \
	      output.ir output.s output.o output.elf *.o

.PHONY: all run asm sim clean