CC = gcc
CFLAGS = -Wall
SRC ?= tests/test_opt_matrix_unroll4.cd
OPT ?= -O2
CREATOR_URL ?= https://creatorsim.github.io/creator/

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
else
EXEEXT :=
endif

COMPILER_BIN := compiler$(EXEEXT)
IR2RISCV_BIN := ir2riscv$(EXEEXT)

all: $(COMPILER_BIN) $(IR2RISCV_BIN)

$(COMPILER_BIN): parser.tab.c lex.yy.c semantic.c semantic.h ir.c ir_o0.c ir_o2.c ir.h ir_internal.h
	$(CC) $(CFLAGS) -o $(COMPILER_BIN) parser.tab.c lex.yy.c semantic.c ir.c ir_o0.c ir_o2.c

$(IR2RISCV_BIN): riscv.c
	$(CC) $(CFLAGS) -o $(IR2RISCV_BIN) riscv.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
ifeq ($(OS),Windows_NT)
	-del /Q /F compiler compiler.exe ir2riscv ir2riscv.exe 2>NUL
else
	rm -f compiler compiler.exe ir2riscv ir2riscv.exe
endif

run: all
	./$(COMPILER_BIN) $(OPT) < $(SRC)
	./$(IR2RISCV_BIN) output.ir output.s

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
