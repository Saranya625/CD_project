CC = gcc
CFLAGS = -Wall

all: compiler

compiler: parser.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o compiler parser.tab.c lex.yy.c -lfl

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm -f compiler lex.yy.c parser.tab.c parser.tab.h

.PHONY: all clean
