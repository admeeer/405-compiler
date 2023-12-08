BUILD_DIR := build

TESTS := TestArrays.cmm TestFunctions.cmm TestIfElse.cmm TestStruct.cmm TestSwitch.cmm TestWhile.cmm

.PHONY: build clean setup test

build: setup $(BUILD_DIR)/parser

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: parser.y
	bison -t -v -d -o $(BUILD_DIR)/parser.tab.c parser.y

$(BUILD_DIR)/lex.yy.c: lexer.l $(BUILD_DIR)/parser.tab.h
	flex -o $(BUILD_DIR)/lex.yy.c lexer.l

$(BUILD_DIR)/parser: $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c AST.h
	gcc -o $(BUILD_DIR)/parser $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c

setup:
	mkdir -p $(BUILD_DIR)/output/irc
	mkdir -p $(BUILD_DIR)/output/asm

test: build
	@for testfile in $(TESTS); do \
		./$(BUILD_DIR)/parser tests/$$testfile; \
	done

testdevel: build
	./$(BUILD_DIR)/parser tests/TestIfElse.cmm
	python asm.py

clean:
	rm -rf $(BUILD_DIR)
	ls -l
