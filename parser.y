// Alexander Peltier, Matthew Powers, Parker Spaan
// CST 405


%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../AST.h"
#include "parser.tab.h"
#include "../symbolTable.h"
#include "../IRC.h"
#include "../MIPSC.h"

extern int yylex();
extern int yyparse();
extern int yydebug;
extern int yylineno;

int ErrorFlag = 0;

int Scope = 0;

char* BuildDirectory = "build/";

extern FILE* yyin;

int computeEquation(struct AST* num1, struct AST* num2, char operator);

void yyerror(const char* s);

%}

%union {

    int number;
    float floatValue;
    char* character;
    char* string;
    struct AST* ast;

}

%token <string> IDENTIFIER;
%token <number> INTEGER
%token <floatValue> FLOAT_T
%token <string> STRING

%token <string> COMMA
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

%type <ast> Program Declaration BlockDeclaration BlockDeclarationList DeclarationList VariableDeclarationList VariableDeclaration FunctionCall FunctionCallList FunctionDeclaration ParameterDeclarationList ParameterDeclarationListTail ParameterDeclaration CodeBlock TYPE Statement StatementList Expression AddSubtractExpression MultiplyDivideExpression Operand BuildingBlock BinOp

%start Program

%locations

%%

Program:

    DeclarationList {
        $$ = $1;
        // We call IREmission here since this is the top of the program
        printAST($$, 3);
        IREmission($$);
        //printAST($$, 3);
    }
;

BlockDeclarationList:
    
    BlockDeclaration BlockDeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | BlockDeclaration {
        $$ = $1;
    }
;

BlockDeclaration:

    VariableDeclaration {}

    | StatementList {}

;

DeclarationList:

    Declaration DeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | Declaration {
        $$ = $1;
    }

    //| FunctionDeclarationList
;

Declaration:

    VariableDeclaration { Scope = 0; }

    | StatementList { Scope = 0; }

    | FunctionDeclaration { Scope = 0;}

;

VariableDeclarationList: 
    
    | VariableDeclaration VariableDeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | VariableDeclaration
;

VariableDeclaration:

    TYPE IDENTIFIER Equals AddSubtractExpression SEMICOLON {
        

        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_VARIABLE, $1->nodeType, Scope);


            SymbolTableSetValue($2, $4->RHS, Scope);

            $$ = insertIntoAST(T_TYPE, nodeTypeToString($1->nodeType), $2);
            //printf("%s\n\n\n", nodeTypeToString($1->nodeType));


        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }

    | TYPE IDENTIFIER SEMICOLON {

        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_VARIABLE, $1->nodeType, Scope);

            $$ = insertIntoAST(T_TYPE, nodeTypeToString($1->nodeType), $2);
            printf("%s\n\n\n", nodeTypeToString($1->nodeType));

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }

;

/*FunctionDeclarationList:

    FunctionDeclaration

    | FunctionDeclaration FunctionDeclarationList {

    }
;*/

FunctionDeclaration:

    FUNCTION TYPE IDENTIFIER LPAREN {


        int FunctionScope = SymbolTableDefineScopeValue();
        Scope = FunctionScope;

        if(!SymbolTableExistsExternalFunctionCall($3, Scope)) {

            SymbolTableInsertInto($3, S_FUNCTION, $2->nodeType, Scope);
            //$$ = insertIntoAST(T_FUNCTION, $2, $3);

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

        //$$ = insertIntoAST(T_FUNCTION, nodeTypeToString($2->nodeType), $3);


    }

    ParameterDeclarationList RPAREN CodeBlock {

        $$ = insertIntoAST(T_FUNCTION, nodeTypeToString($2->nodeType), $3);

    }
;

ParameterDeclarationList:


    | ParameterDeclarationListTail {

    }


ParameterDeclarationListTail:
    ParameterDeclaration {

    }
    
    | ParameterDeclaration COMMA ParameterDeclarationListTail {
        $1->right = $3;
        $$ = $1;
    }
;

ParameterDeclaration:

    TYPE IDENTIFIER {


        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_FUNCTION_PARAMETER, $1->nodeType, Scope);

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }


    }
;
CodeBlock:

    LBRACKET BlockDeclarationList RBRACKET {
        $$ = $2;
        printAST($2, 3);
    }

;

TYPE: 

    INT {
    
        $$ = insertIntoAST(T_INT, "", "");
    
    }
    | FLOAT {

        $$ = insertIntoAST(T_FLOAT, "", "");

    }
    | CHAR {

        $$ = insertIntoAST(T_CHAR, "", "");

    }
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

    }

    | Expression SEMICOLON {

        $$ = $1;

    }

    | RETURN Expression SEMICOLON {
        
        //rintf("RETURN statement accessed!\n");
        if(strcmp(nodeTypeToString($2->nodeType), SymbolValueTypeToString(SymbolTableGetSymbolValueTypeFromScope(Scope))) == 0) {
            //printf("Return statement got here, too!\n");
            $$ = insertIntoAST(T_RETURN, "", $2->RHS);
            
        
        } else {

            fprintf(stderr, "Semantic Error: %s Return type of %s != function declaration type of %s\n", nodeTypeToString($2->nodeType), SymbolValueTypeToString(SymbolTableGetSymbolValueTypeFromScope(Scope)));
            exit(EXIT_FAILURE);

        }

    }

    | WRITE Expression SEMICOLON {
        
        if($2->nodeType == T_IDENTIFIER){
            SymbolTableSetSymbolUsed($2->RHS, Scope);
        }

        $$ = insertIntoAST(T_WRITE, "", $2->RHS);
    }

    | WRITELN SEMICOLON {
        
        $$ = insertIntoAST(T_WRITELN, "", "");

    }
;

Expression:

    BuildingBlock

    | AddSubtractExpression

    | FunctionCall

    | IDENTIFIER Equals Expression {
        
        //printf("IDENTIFIER Equals Expression $1 = %s and $3 = %s\n", $1, $3->RHS);

        if(!SymbolTableExistsExternalFunctionCall($1, Scope)) {
            fprintf(stderr, "Semantic Error: Tried setting %s at scope %d to a value, but %s is undeclared", $1, Scope, $1);
            exit(EXIT_FAILURE);
        }

        if(SymbolTableGetNodeType($1, Scope) != $3->nodeType) {
            //printf("%s and %s also %s and %s", nodeTypeToString(SymbolTableGetNodeType($1)), nodeTypeToString($3->nodeType), $3->LHS, $3->RHS);
            fprintf(stderr, "Semantic Error: Type Mismatch. Attempted to sign type %s to type %s", SymbolTableGetNodeType($1, Scope), $3->nodeType);
            exit(EXIT_FAILURE);
        } 

        SymbolTableSetValue($1, $3->RHS, Scope);

        $$ = insertIntoAST(T_EQUALS, $1, $3->RHS);

    }

;

FunctionCall:
    IDENTIFIER LPAREN FunctionCallList RPAREN {
        //printf("Recognized rule: Function Call \n");
        $$ = insertIntoAST(T_FUNCTIONCALL, nodeTypeToString($3->nodeType), "0");
    }
;

FunctionCallList: {}
    | BuildingBlock {

    }

    | BuildingBlock COMMA FunctionCallList {}

;


BuildingBlock:

    IDENTIFIER {
        //printf("Uh oh raggy!\n");
        //printf("Value of identifier %s at Scope %d is %s\n", $1, Scope, SymbolTableGetValue($1, Scope));
        $$ = insertIntoAST(T_INT, "", SymbolTableGetValue($1, Scope));

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

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        //printf("$1 = %s and $3 = %s\n", $1->RHS, $3->RHS);
        sprintf(value, "%d", computeEquation($1, $3, operatorArray[0]));

        $$ = insertIntoAST(T_INT, "", value);

    }

    | AddSubtractExpression ADD MultiplyDivideExpression {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        //printf("$1 = %s and $3 = %s\n", $1->RHS, $3->RHS);
        sprintf(value, "%d", computeEquation($1, $3, operatorArray[0]));


        //printf("the value of compute equation is %d and the value of value is %s\n", computeEquation($1, $3, operatorArray[0]), value);

        $$ = insertIntoAST(T_INT, "", value);

    }
;

MultiplyDivideExpression:

    Operand

    | MultiplyDivideExpression MULTIPLY Operand {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        sprintf(value, "%d", computeEquation($1, $3, operatorArray[0]));

        $$ = insertIntoAST(T_INT, "", value);

    }

    | MultiplyDivideExpression DIVIDE Operand {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        sprintf(value, "%d", computeEquation($1, $3, operatorArray[0]));

        $$ = insertIntoAST(T_INT, "", value);

    }
;

Operand:

    IDENTIFIER {

        $$ = insertIntoAST(T_IDENTIFIER, $1, SymbolTableGetValue($1, Scope));

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
    ErrorFlag = 1;
}

int main(int argc, char**argv)
{

    //yydebug = 1;

	printf("\n\nLexer & Parser Initialized!\n\n");

    printf("Initializing Intermediate Code Representation Files ...\n\n");
    
    IRInitializeFile();

    MIPSInitializeFile();

	if (argc > 1){
	  if(!(yyin = fopen(argv[1], "r")))
          {
		perror(argv[1]);
		return(1);
	  }
	}

	yyparse();


    if(!ErrorFlag) {
        printf("\nCalling for Emission to Intermediate Code Representation Files ...\n\n");

        SymbolTablePrint();

        IREmissionCleanUp();       
    } else {
        printf("Aborting\n");
    }

}

int computeEquation(struct AST* num1, struct AST* num2, char operator) {
	char mipsVal1[10];
	char mipsVal2[10];
	int val1; int val2;	
	val1 = atoi(num1->RHS);
	val2 = atoi(num2->RHS);
	
	if (!(num1->nodeType == T_IDENTIFIER)) {
        //printf("got here at some point\n");
		strcpy(mipsVal1, num1->LHS);
        //printf("num1 lhs is %s\n", num1->LHS);
	} else {
		strcpy(mipsVal1, num1->RHS);
	}

	if (!(num2->nodeType == T_IDENTIFIER)) {
		strcpy(mipsVal2, num2->LHS);
	} else {
		strcpy(mipsVal2, num2->RHS);
	}
	/*if (inFunction) {*/
	/*	emitMIPSEquation(mipsVal1, mipsVal2, operator);*/
	/*}*/

	/* char newVal[3]; */
	int newVal;
	/* printf("Equation detected: %d %c %d", val1, operator, val2); */

	switch (operator) {
		case '+':
			newVal = val1 + val2;
			/* sprintf(newVal, "%d", val1 + val2); */
			/* printf("Addtion Expr found!\n"); */
			break;
		case '-':
			newVal = val1 - val2;
			/* sprintf(newVal, "%d", val1 - val2); */
			/* printf("Subtraction Expr found!\n"); */
			break;
		case '*':
			newVal = val1 * val2;
			/* sprintf(newVal, "%d", val1 * val2); */
			/* printf("Multiplication Expr found!\n"); */
			break;
		case '/':
			newVal = val1 / val2;
			/* sprintf(newVal, "%d", val1 / val2); */
			/* printf("Division Expr found!\n"); */
			break;
	}
    return newVal;
	/* printf("Newval = %d\n", newVal); */

}