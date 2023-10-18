BUILD_DIR := build

.PHONY: all clean setup

all: $(BUILD_DIR)/parser

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: parser.y
	bison -t -v -d -o $(BUILD_DIR)/parser.tab.c parser.y

$(BUILD_DIR)/lex.yy.c: lexer.l $(BUILD_DIR)/parser.tab.h
	flex -o $(BUILD_DIR)/lex.yy.c lexer.l

$(BUILD_DIR)/parser: $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c AST.h
	gcc -o $(BUILD_DIR)/parser $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c
	./$(BUILD_DIR)/parser tests/test4.cmm

setup:
	mkdir -p $(BUILD_DIR)/output/irc
	mkdir -p $(BUILD_DIR)/output/asm

clean:
	rm -rf $(BUILD_DIR)
	ls -l
