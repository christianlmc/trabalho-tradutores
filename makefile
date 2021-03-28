CC = gcc
LIB = -lfl -L/usr/local/opt/flex/lib

all: main

lex: lex.yy.c

bison: y.tab.c

yacc: bison

lex.yy.c: lex.l
	flex lex.l 
	
syntax.tab.c: syntax.y
	bison -dvt -Wall -Wcounterexamples syntax.y

y.tab.h: syntax.y

main: lex.yy.c syntax.tab.c y.tab.h
	$(CC) -g -Wall lex.yy.c syntax.tab.c -o main $(LIB)
	mkdir -p dist
	mv -t dist lex.yy.c *.tab.* syntax.output

clean:
	rm -rf dist
	rm main