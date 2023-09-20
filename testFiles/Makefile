all: parser

parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

lex.yy.c: kyungchanLexer.l parser.tab.h
	flex kyungchanLexer.l

parser: lex.yy.c parser.tab.c parser.tab.h 
	gcc -o parser parser.tab.c lex.yy.c
	./parser testProg.cmm

clean:
	rm -f parser lexer parser.tab.c lex.yy.c parser.tab.h parser.output
	ls -l
