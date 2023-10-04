// Alexander Peltier, Matthew Powers, Parker Spaan
// CST 405


%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AST.h"
#include "parser.tab.h"
//#include "sT.h"
//#include "IRc.h"
//#include "MIPSc.h"

extern int yylex();
extern int yyparse();
extern int yydebug;
extern int yylineno;

extern FILE* yyin;

void yyerror(const char* s);

%}

%union {

    int number;
    float floatValue;
    char character;
    char* string;
    struct AST* ast;

}

%token <string> IDENTIFIER;
%token <number> INTEGER
%token <floatValue> FLOAT_T
%token <string> string

%token <string> SEMICOLON
%token <string> Equals
%token <string> WRITE
%token <string> WRITELN

%token <string> FUNCTION
%token <string> IF
%token <string> ELSE
%token <string> WHILE

%token <string> ADD
%token <string> SUBTRACT
%token <string> MULTIPLY
%token <string> DIVIDE
%token <string> BINOP

%token <string> LBRACKET
%token <string> RBRACKET
%token <string> LPAREN
%token <string> RPAREN
%token <string> LCURLY
%token <string> RCURLY

%token <string> INT
%token <string> CHAR
%token <string> FLOAT

%token <string> RETURN

%token <string> CHARACTER

%left ADD
%left SUBTRACT
%left MULTIPLY
%left DIVIDE

%type <ast> Program Declaration DeclarationList VariableDeclarationList VariableDeclaration TYPE Statement StatementList Expression AddSubtractExpression MultiplyDivideExpression Operand BuildingBlock BinOp

%start Program

%locations

%%

Program:

    DeclarationList {
        $$ = $1;
        printAST($$, 3);
    }
;
DeclarationList:

    Declaration DeclarationList {
        $1-> right = $2;
        $$ = $1;
    }

    | Declaration {
        $$ = $1;
    }
;
Declaration:

    VariableDeclaration {}

    | StatementList {

    }
;

VariableDeclarationList: 
    
    | VariableDeclaration VariableDeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | VariableDeclaration
;

VariableDeclaration:

    TYPE IDENTIFIER SEMICOLON {
        
        printf("The node type of $1 is %s", nodeTypeToString($1->nodeType));
        $$ = insertIntoAST(T_TYPE, nodeTypeToString($1->nodeType), $2);

    }

;

TYPE: INT {}
    | FLOAT {}
    | CHAR {}
;

StatementList:

    Statement
    
    | Statement StatementList {
        $1->right = $2;
        $$ = $1;
    }
;

Statement:

    SEMICOLON {
        //printf("\nRULE RECOGNIZED: SEMICOLON \n");
    }

    | Expression SEMICOLON {
        //printf("\nRULE RECOGNIZED: STATEMENT DECLARATION \n");
        $$ = $1;
    }

    | RETURN Expression SEMICOLON {
        
        $$ = insertIntoAST(T_RETURN, "", $2->RHS);

    }

    | WRITE Expression SEMICOLON {
        
        $$ = insertIntoAST(T_WRITE, "", $2->RHS);
    }

    | WRITELN SEMICOLON {
        
        $$ = insertIntoAST(T_WRITELN, "", "");

    }
;

Expression:

    BuildingBlock

    | AddSubtractExpression

    | IDENTIFIER Equals Expression {
        
        $$ = insertIntoAST(T_EQUALS, $1, $3->RHS);

    }

;

BuildingBlock:

    IDENTIFIER {

        $$ = insertIntoAST(T_IDENTIFIER, "", $1);

    }
    
    | FLOAT_T {
        
        char value[10];
        sprintf(value, "%f", $1);
        $$ = insertIntoAST(T_FLOAT, "", value);

    }

    | INTEGER {

        char value[10];
        sprintf(value, "%d", $1);
        $$ = insertIntoAST(T_INT, "", value);

    }
    
    | CHARACTER {
        
        $$ = insertIntoAST(T_CHAR, "", $1);

    }
;

AddSubtractExpression:

    MultiplyDivideExpression

    | AddSubtractExpression SUBTRACT MultiplyDivideExpression {

        
        //$$ = insertIntoAST(T_INT, "", value);

    }

    | AddSubtractExpression ADD MultiplyDivideExpression {

    }
;

MultiplyDivideExpression:

    Operand

    | MultiplyDivideExpression MULTIPLY Operand {

    }

    | MultiplyDivideExpression DIVIDE Operand {

    }
;

Operand:

    IDENTIFIER {

        $$ = insertIntoAST(T_IDENTIFIER, "", $1);

    }

    | INTEGER {

        char value[10];
        sprintf(value, "%d", $1);
        $$ = insertIntoAST(T_INT, "", value);

    }

    | LPAREN AddSubtractExpression RPAREN {
        
        $$ = insertIntoAST(T_INT, "", $2->RHS);

    }

;

BinOp: ADD {}
    | SUBTRACT {}
    | MULTIPLY {}
    | DIVIDE {}



%%

void yyerror(const char *str)
{
    fprintf(stderr,"Error | Line: %d\n%s\n",yylineno,str);
}

int main(int argc, char**argv)
{

    //yydebug = 1;
	printf("\n\n##### COMPILER STARTED #####\n\n");

	
	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}
	yyparse();

}