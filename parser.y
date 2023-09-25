// Alexander Peltier, Matthew Powers, Parker Spaan
// CST 405


%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AST.h"
//#include "sT.h"
//#include "IRc.h"
//#include "MIPSc.h"

extern int yylex();
extern int yyparse();
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

%token <string> identifier;
%token <number> integer
%token <floatValue> float_T
%token <string> string

%token <string> SEMICOLON
%token <string> EQ
%token <string> WRITE
%token <string> WRITELN

%token <string> FUNCTION
%token <string> IF
%token <string> ELSE
%token <string> WHILE

%token <string> PLUS
%token <string> MINUS
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

%type <ast> Program Declaration DeclarationList VariableDeclarationList VariableDeclaration Type Statement StatementList expression MathExpression Term BuildingBlock BinOp

%start Program

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

    Type identifier SEMICOLON {
        printf("\nRULE RECOGNIZED: VARIABLE DECLARATION %s\n", $2);
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
        printf("\nRULE RECOGNIZED: SEMICOLON \n");
    }

    | expression SEMICOLON {
        $$ = $1;
    }

    | RETURN expression SEMICOLON {
        printf("\nRULE RECOGNIZED: RETURN \n");
    }

    | WRITE expression SEMICOLON {
        printf("\nRULE RECOGNIZED: WRITE \n");
    }

    | WRITELN SEMICOLON {
        printf("\nRULE RECOGNIZED: WRITELN %s\n", $1);
    }
;
expression:
    BuildingBlock

    | MathExpression

    | identifier EQ expression {
        printf("\nRULE RECOGNIZED: ASSIGNMENT STATEMENT \n");
    }

    | identifier PLUS PLUS {

    }

    | identifier LBRACKET integer RBRACKET

;

BuildingBlock:
    identifier {

    }
    
    | float_T {
        printf("\nRULE RECOGNIZED: FLOAT \n");
    }

    | integer {
        printf("\nRULE RECOGNIZED: INTEGER \n");
    }
    
    | character {
        printf("\nRULE RECOGNIZED: CHARACTER \n");
    }
;

MathExpression:

    Term

    | MathExpression PLUS Term {

    }

    | MathExpression MINUS Term {

    }
;

Term:
;

BinOp: PLUS {}
    | MINUS {}
    | MULTIPLY {}
    | DIVIDE {}

%%


void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}

int main(int argc, char**argv)
{
/* 
	#ifdef YYDEBUG
		yydebug = 1;
	#endif */

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