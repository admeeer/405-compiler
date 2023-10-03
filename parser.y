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
%token <number> integer
%token <floatValue> float_T
%token <string> string

%token <string> SEMICOLON
%token <string> EQUALS
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

%token <string> character

%left ADD
%left SUBTRACT
%left MULTIPLY
%left DIVIDE

%type <ast> Program Declaration DeclarationList VariableDeclarationList VariableDeclaration Type Statement StatementList Expression AddSubtractExpression MultiplyDivideExpression Operand BuildingBlock BinOp

%start Program

%locations

%%

Program:

    DeclarationList {
        $$ = $1;
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

    Type IDENTIFIER SEMICOLON {
        //printf("\nRULE RECOGNIZED: VARIABLE DECLARATION %s\n", $2);
    }

;

Type: INT {}
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
        //printf("\nRULE RECOGNIZED: RETURN \n");
    }

    | WRITE Expression SEMICOLON {
        //printf("\nRULE RECOGNIZED: WRITE \n");
    }

    | WRITELN SEMICOLON {
        //printf("\nRULE RECOGNIZED: WRITELN %s\n", $1);
    }
;

Expression:

    BuildingBlock

    | AddSubtractExpression

    | IDENTIFIER EQUALS Expression {
        //printf("\nMatched an ASSIGNMENT Expression: %s = \n");
        //printf("\nRULE RECOGNIZED: ASSIGNMENT STATEMENT \n");
    }

    | IDENTIFIER ADD ADD {

    }

;

BuildingBlock:

    IDENTIFIER {

    }
    
    | float_T {
        //printf("\nRULE RECOGNIZED: FLOAT \n");
    }

    | integer {
        //printf("\nRULE RECOGNIZED: INTEGER \n");
    }
    
    | character {
        //printf("\nRULE RECOGNIZED: CHARACTER \n");
    }
;

AddSubtractExpression:

    MultiplyDivideExpression

    | AddSubtractExpression SUBTRACT MultiplyDivideExpression {

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

    }

    | integer {

    }

    | LPAREN AddSubtractExpression RPAREN {
        
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