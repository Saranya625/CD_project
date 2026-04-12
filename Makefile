CC = gcc
CFLAGS = -Wall

all: compiler ir2riscv

compiler: parser.tab.c lex.yy.c semantic.c semantic.h ir.c ir_o0.c ir_o2.c ir.h ir_internal.h
	$(CC) $(CFLAGS) -o compiler parser.tab.c lex.yy.c semantic.c ir.c ir_o0.c ir_o2.c

ir2riscv: riscv.c
	$(CC) $(CFLAGS) -o ir2riscv riscv.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm -f compiler ir2riscv lex.yy.c parser.tab.c parser.tab.h

.PHONY: all clean
