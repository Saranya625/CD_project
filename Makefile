CC = gcc
CFLAGS = -Wall
SRC ?= tests/test_opt_matrix_unroll2.cd
OPT ?= -O0
CREATOR_URL ?= https://creatorsim.github.io/creator/

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

run: all
	./compiler $(OPT) < $(SRC)
	./ir2riscv output.ir output.s

creator: run
	@if command -v powershell >/dev/null 2>&1; then \
		powershell -NoProfile -ExecutionPolicy Bypass -File scripts/open_creator.ps1 -AsmFile output.s -CreatorUrl "$(CREATOR_URL)"; \
	elif command -v pwsh >/dev/null 2>&1; then \
		pwsh -NoProfile -File scripts/open_creator.ps1 -AsmFile output.s -CreatorUrl "$(CREATOR_URL)"; \
	else \
		sh scripts/open_creator.sh output.s "$(CREATOR_URL)"; \
	fi

creator-autopaste: run
	@if command -v powershell >/dev/null 2>&1; then \
		powershell -NoProfile -ExecutionPolicy Bypass -File scripts/open_creator.ps1 -AsmFile output.s -CreatorUrl "$(CREATOR_URL)" -AutoPaste; \
	elif command -v pwsh >/dev/null 2>&1; then \
		pwsh -NoProfile -File scripts/open_creator.ps1 -AsmFile output.s -CreatorUrl "$(CREATOR_URL)"; \
	else \
		sh scripts/open_creator.sh output.s "$(CREATOR_URL)"; \
	fi

.PHONY: all clean run creator creator-autopaste
