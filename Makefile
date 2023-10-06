all: parser

parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser: lex.yy.c parser.tab.c parser.tab.h AST.h
	gcc -o parser parser.tab.c lex.yy.c
	./parser tests/test.cmm

clean:
	clear
	rm -f parser lexer parser.tab.c lex.yy.c parser.tab.h parser.output IRCMain.ir IRCData.ir IRC.ir
	ls -l
