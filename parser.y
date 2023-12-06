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
%token <string> ssstruct

%token <string> COMMA
%token <string> SEMICOLON
%token <string> Equals
%token <string> WRITE
%token <string> WRITELN

%token <string> FUNCTION
%token <string> IF
%token <string> ELSE
%token <string> WHILE
%token <string> DOT

%token <string> ADD
%token <string> SUBTRACT
%token <string> MULTIPLY
%token <string> DIVIDE

%token <string> LBRACKET
%token <string> RBRACKET
%token <string> LPAREN 
%token <string> RPAREN
%token <string> LCURLY
%token <string> RCURLY
%token <string> LOGICOP

%token <string> INT
%token <string> CHAR
%token <string> FLOAT

%token <string> RETURN
%token <string> SWITCH
%token <string> BREAK
%token <string> CASE
%token <string> COLON

%token <string> CHARACTER

%left ADD
%left SUBTRACT
%left MULTIPLY
%left DIVIDE

%type <ast> Program Declaration BlockDeclaration BlockDeclarationList DeclarationList VariableDeclaration StructDeclaration StructDeclarationList FunctionCall FunctionCallParameterList FunctionDeclaration ParameterDeclarationList ParameterDeclarationListTail ParameterDeclaration CodeBlock TYPE Statement StatementList Expression AddSubtractExpression MultiplyDivideExpression Operand BuildingBlock Array Struct StructBlock If Condition IfCondition Else Switch CaseList Case CaseBlock

%start Program

%locations

%%

Program:

    DeclarationList {
        $$ = $1;
        // We call IREmission here since this is the top of the program
        printAST($$, 3);

        Scope = 0;

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

    VariableDeclaration 

    | Statement 


;

DeclarationList:

    Declaration DeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | Declaration {
        $$ = $1;
    }

;

Declaration:

    VariableDeclaration { Scope = 0; }

    | Statement { Scope = 0; }

    | FunctionDeclaration { Scope = 0;}

    | Struct { Scope = 0; }

;


Struct:

    ssstruct IDENTIFIER {

        int StructScope = SymbolTableDefineScopeValue();
        Scope = StructScope;
        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_STRUCT, T_STRUCT, Scope);

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }

    StructBlock {

        $$ = insertIntoAST(T_STRUCT, $2, "");

        $$->left = $4;
        //$$ = $4;

    }

;


StructBlock: 

    LBRACKET StructDeclarationList RBRACKET {
        $$ = $2;
        printAST($2, 3);
    }

;

StructDeclarationList:

    StructDeclaration StructDeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | StructDeclaration {
        $$ = $1;
    }

;

StructDeclaration:

    TYPE IDENTIFIER SEMICOLON {

        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {
            SymbolTableInsertInto($2, S_STRUCT_VARIABLE, $1->nodeType, Scope);

            $$ = insertIntoAST(T_TYPE, nodeTypeToString($1->nodeType), $2);
            //printf("%s\n\n\n", nodeTypeToString($1->nodeType));

            /*if (Scope != 0) {
                SymbolTableSetSymbolUsed($2, Scope);
            }*/

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }

;

VariableDeclaration:

    // Primitive var with assignment
    TYPE IDENTIFIER Equals AddSubtractExpression SEMICOLON {
        

        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_VARIABLE, $1->nodeType, Scope);


            SymbolTableSetValue($2, $4->RHS, Scope);

            $$ = insertIntoAST(T_EQUALS, $2, $4->RHS);
            //printf("%s\n\n\n", nodeTypeToString($1->nodeType));

            if(Scope != 0) {

                SymbolTableSetSymbolUsed($2, Scope);
                
            }


        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }
    // Primitive var
    | TYPE IDENTIFIER SEMICOLON {

        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_VARIABLE, $1->nodeType, Scope);

            $$ = insertIntoAST(T_TYPE, nodeTypeToString($1->nodeType), $2);
            printf("%s\n\n\n", nodeTypeToString($1->nodeType));

            if (Scope != 0) {
                SymbolTableSetSymbolUsed($2, Scope);
            }

        } else {

            fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
            exit(EXIT_FAILURE);

        }

    }
    // Array 
    | TYPE IDENTIFIER LBRACKET INTEGER RBRACKET SEMICOLON {

        if($4 < 0){
            fprintf(stderr, "Parser Error: array size of %s, array size cannot be less than zero.", $4);
            exit(EXIT_FAILURE);
        }
        
       if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_ARRAY, $1->nodeType, Scope);

            SymbolTableSetSymbolValueArrayLength($2, Scope, $4);

            char length[3];
            sprintf(length, "%d", $4);
            //printf("hi xd\n");
            $$ = insertIntoAST(T_ARRAY, $2, length);
       } else {

        fprintf(stderr, "Semantic Error: %s already exists in the SymbolTable!\n", $2);
        exit(EXIT_FAILURE);
       }

    }
;

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

    }

    ParameterDeclarationList RPAREN CodeBlock {

        //$$ = insertIntoAST(T_FUNCTION, nodeTypeToString($2->nodeType), $3);
        $$ = insertSyntaxTreeFunction(T_FUNCTION, nodeTypeToString($2->nodeType), $3, $8, $6);

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

        // If the parameter doesn't exist
        if(!SymbolTableExistsExternalFunctionCall($2, Scope)) {

            SymbolTableInsertInto($2, S_FUNCTION_PARAMETER, $1->nodeType, Scope);
            SymbolTableSetSymbolUsed($2, Scope);

            $$ = insertIntoAST(T_PARAMETER, nodeTypeToString($1->nodeType), $2);

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
        

        // Does the return type match the function type?


        if(strcmp(nodeTypeToString($2->nodeType), SymbolValueTypeToString(SymbolTableGetSymbolValueTypeFromFunctionScope(Scope))) == 0) {

            //printf("Return statement got here, too!\n");
            $$ = insertIntoAST(T_RETURN, "", $2->RHS);
            
        
        } else {

            fprintf(stderr, "Semantic Error: %s Return type of %s != function declaration type of %s\n", nodeTypeToString($2->nodeType), SymbolValueTypeToString(SymbolTableGetSymbolValueTypeFromFunctionScope(Scope)));
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

    | If 

    | Switch

;

Switch:

    SWITCH LPAREN BuildingBlock RPAREN {

        int SwitchScope = SymbolTableDefineScopeValue();
        Scope = SwitchScope;
        
    }
    
    LBRACKET CaseList RBRACKET {

        $$ = insertSyntaxTreeSwitchStatement(T_SWITCH, nodeTypeToString($3->nodeType), $3->RHS, Scope, $7);

    }

;

CaseList:

    Case CaseList {
        $1->right = $2;
        $$ = $1;
    }

    | Case {
        $$ = $1;
    }

;

Case:

    CASE BuildingBlock COLON BREAK SEMICOLON {

        $$ = insertIntoAST(T_CASE, "", $2->RHS);

    }

    | CASE BuildingBlock COLON CaseBlock BREAK SEMICOLON {

        $$ = insertIntoAST(T_CASE, "", $2->RHS);
        $$->left = $4;

    }

;

CaseBlock:

    BlockDeclaration BlockDeclarationList {
        $1->right = $2;
        $$ = $1;
    }

    | BlockDeclaration {
        $$ = $1;
    }

;

If:

    IfCondition CodeBlock {

        $$ = insertSyntaxTreeIfStatement(T_IF, "", "", $1, Scope);
        $$->left = $2;

    }


    | IfCondition CodeBlock Else {

        $$ = insertSyntaxTreeIfElseStatement(T_IF_ELSE, "", "", $1, $3, Scope);
        $$->left = $2;

    }

;

IfCondition:

    IF LPAREN Condition RPAREN {
        
        $$ = $3;
        int IfScope = SymbolTableDefineScopeValue();
        Scope = IfScope;

    }

Else:

    ELSE CodeBlock {

        $$ = $2;

    }

;




Condition:

    BuildingBlock LOGICOP BuildingBlock {

        if($1->nodeType != $3->nodeType) {
            fprintf(stderr, "Type Mismatch Error: type of %s does not match type of %s !", $1->RHS, $3->RHS);
            exit(EXIT_FAILURE);
        }

        $$ = insertSyntaxTreeCondition(T_CONDITION, $1->RHS, $3->RHS, $2);

    }



;


Array:

    IDENTIFIER LBRACKET INTEGER RBRACKET Equals BuildingBlock {

        if(SymbolTableExistsExternalFunctionCall($1, Scope)) {

            if(SymbolTableGetNodeType($1, Scope) == $6->nodeType) {

                $$ = insertSyntaxTreeArrayAssignment(T_ARRAY_ELEMENT_EQUALS, $1, $6->RHS, $3);

            } else {
                
                fprintf(stderr, "Parser Error: Type Mismatch\n");
                exit(EXIT_FAILURE);
                //fprintf("Parser Error: Type mismatch, tried setting array element with type %s to primitive with type %s", nodeTypeToString(SymbolTableGetNodeType($1, Scope)), nodeTypeToString($6->nodeType));

            }

        } else if (SymbolTableExistsExternalFunctionCall($1, 0)) {

        if(SymbolTableGetNodeType($1, Scope) == $6->nodeType) {

                 $$ = insertSyntaxTreeArrayAssignment(T_ARRAY_ELEMENT_EQUALS, $1, $6->RHS, $3);

            } else {

                ///fprintf("Parser Error: Type mismatch, tried setting array element with type %s to primitive with type %s", nodeTypeToString(SymbolTableGetNodeType($1, 0)), nodeTypeToString($6->nodeType));
                fprintf(stderr, "Parser Error: Type Mismatch\n");
                exit(EXIT_FAILURE);
            }

        } else {

            fprintf(stderr, "Semantic Error: %s does not exist in this scope!\n", $1);
            exit(EXIT_FAILURE);

        }

    }

    | IDENTIFIER Equals IDENTIFIER LBRACKET INTEGER RBRACKET {

        int VariableScope;
        int ArrayScope;

        if(SymbolTableExistsExternalFunctionCall($1, Scope)) {
            VariableScope = Scope;
        } else if(SymbolTableExistsExternalFunctionCall($1, 0)) {
            VariableScope = 0;
        } else {
            // left hand side variable doesn't exist ... TO-DO ERROR
            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $1);
            exit(EXIT_FAILURE);
        }

        if(SymbolTableExistsExternalFunctionCall($3, Scope)) {
            ArrayScope = Scope;
        } else if(SymbolTableExistsExternalFunctionCall($3, 0)) {
            ArrayScope = 0;
        } else {
            // right hand side variable doesn't exist ... TO-DO ERROR
            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $3);
            exit(EXIT_FAILURE);
        }

        int ArraySize = SymbolTableGetSymbolValueArrayLength($3, ArrayScope);

        if($5 > ArraySize) {
            fprintf(stderr, "Semantic Error: %d out of bounds of Array, array size is only %d\n", $5, ArraySize);
            exit(EXIT_FAILURE);
        }

        $$ = insertSyntaxTreeArrayAssignment(T_EQUALS_ARRAY_ELEMENT, $1, $3, $5);

    }


Expression:

    BuildingBlock

    | AddSubtractExpression

    | Array

    | IDENTIFIER Equals Expression {

        int VariableScope = 0;
        
        if(Scope != 0) {

            if(SymbolTableExistsExternalFunctionCall($1, Scope)) {

                VariableScope = Scope;

            } else if(SymbolTableExistsExternalFunctionCall($1, 0)) {

                VariableScope = 0;

            } else {
                
                fprintf(stderr, "Semantic Error: Tried setting %s to a value, but %s is undeclared\n", $1, $1);
                exit(EXIT_FAILURE);

            }

        }


        if(SymbolTableGetNodeType($1, VariableScope) != $3->nodeType) {
            //printf("%s and %s also %s and %s", nodeTypeToString(SymbolTableGetNodeType($1)), nodeTypeToString($3->nodeType), $3->LHS, $3->RHS);
            fprintf(stderr, "Semantic Error: Type Mismatch. Attempted to assign type %s to type %s\n", SymbolTableGetNodeType($1, Scope), $3->nodeType);
            exit(EXIT_FAILURE);
        } 

        SymbolTableSetValue($1, $3->RHS, VariableScope);
        //printf("Identifier equals expression called, expression is %s and %s\n", $3->LHS, $3->RHS);
        //printf("Identifier equals expression called, expression is %s\n", $3);
        $$ = insertIntoAST(T_EQUALS, $1, $3->RHS);

    }

    | IDENTIFIER Equals IDENTIFIER FunctionCall {

        int VariableScope = 0;
        
        if(Scope != 0) {

            if(SymbolTableExistsExternalFunctionCall($1, Scope)) {

                VariableScope = Scope;

            } else if(SymbolTableExistsExternalFunctionCall($1, 0)) {

                VariableScope = 0;

            } else {
                
                fprintf(stderr, "Semantic Error: Tried setting %s to a value, but %s is undeclared\n", $1, $1);
                exit(EXIT_FAILURE);

            }

        }
        
        int FunctionCallScope = SymbolTableGetSymbolScope($3, S_FUNCTION);

        if(SymbolTableGetNodeType($1, VariableScope) == SymbolTableGetNodeType($3, FunctionCallScope)) {
            $$ = insertIntoAST(T_EQUALS_FUNCTION, $1, $3);
            $$->left = $4;
            //printf("$4 is %s\n", $4->RHS);
            //printAST($4, 3);
        } else {

            fprintf(stderr, "Semantic Error: Type Mismatch. Attempted to assign type %s to type %s\n", nodeTypeToString(SymbolTableGetNodeType($1, VariableScope)), nodeTypeToString(SymbolTableGetNodeType($3, FunctionCallScope)));
            exit(EXIT_FAILURE);
        }

    }

    | IDENTIFIER DOT IDENTIFIER Equals Expression {

        int StructScope = SymbolTableGetSymbolScope($1, S_STRUCT);

        if(SymbolTableExistsExternalFunctionCall($3, StructScope)) {
            
        }else {

            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $3);
            exit(EXIT_FAILURE);

        }

        if(SymbolTableGetNodeType($3, StructScope) == $5->nodeType) {

            $$ = insertSyntaxTreeStructAssignment(T_STRUCT_VARIABLE_EQUALS, $1, $5->RHS, $3);

        }



    }

    | IDENTIFIER Equals IDENTIFIER DOT IDENTIFIER {

        if(!SymbolTableExistsExternalFunctionCall($1, Scope) && !SymbolTableExistsExternalFunctionCall($1, 0)) {

            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $1);

        }
        
        int StructScope = SymbolTableGetSymbolScope($3, S_STRUCT);

        if(SymbolTableExistsExternalFunctionCall($3, StructScope)) {
            
        }else {

            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $3);
            exit(EXIT_FAILURE);

        }

        $$ = insertSyntaxTreeStructAssignment(T_EQUALS_STRUCT_VARIABLE, $1, $3, $5);

    }

;

FunctionCall:

    LPAREN FunctionCallParameterList RPAREN {
        $$ = $2;
        printAST($2, 3);
    }

;

FunctionCallParameterList: 

    BuildingBlock COMMA FunctionCallParameterList {
        $1->right = $3;
        $$ = $1;
    }

    | BuildingBlock {
        $$ = $1;
    }
;

BuildingBlock:

    IDENTIFIER {
        //printf("Checking identifier %s\n", $1);
        // Are we in a function?
        if(Scope != 0){
            // Is the variable whose value we are trying to extract declared in the function?
           if(SymbolTableExistsExternalFunctionCall($1, Scope)){


            // Is the variable a parameter?
            if(SymbolTableGetSymbolType($1, Scope) == S_FUNCTION_PARAMETER) {
                $$ = insertIntoAST(T_INT, "", $1);
                break;
            } else if(SymbolTableGetSymbolUsed($1, Scope)) {
                $$ = insertIntoAST(T_INT, "", $1);
            }else {

                $$ = insertIntoAST(T_INT, "", $1);
                
            }


           } else {

            // It doesn't, so the variable must be declared in the global scope.
            if(SymbolTableExistsExternalFunctionCall($1, 0)) {

                $$ = insertIntoAST(T_INT, "", $1);

            } else {
                // The variable doesn't exist in the function or global scope, meaning it doesn't exist, or we are accessing something we shouldn't. Failure!
            fprintf(stderr, "Parser Error: Variable %s does not exist in the function or global scope.", $1);
            exit(EXIT_FAILURE);

            }

           }



        } else {

            $$ = insertIntoAST(T_INT, "", $1);

        }


    }

    | IDENTIFIER DOT IDENTIFIER {

        int StructScope = SymbolTableGetSymbolScope($1, S_STRUCT);

        if(SymbolTableExistsExternalFunctionCall($3, StructScope)) {
            
        }else {

            fprintf(stderr, "Semantic Error: %s does not exist in any scope!\n", $3);
            exit(EXIT_FAILURE);

        }

        $$ = insertIntoAST(T_INT, "", $3);

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

/*MathExpressionList:

    MathExpression {
        $$ = $1;
    }

    | MathExpression MathExpressionList {
        $1->right = $2;
        $$ = $1;
    }

;

MathExpression:

    Operand

    | Operand ADD MathExpression {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        char Expression[100];

        //sprintf(Expression, "%s%c%s", $1->RHS, operatorArray[0], $3->RHS);

        ///$$ = insertIntoAST(T_INT, "", Expression);

        //printf("Called, expression is %s%c%s\n", $1->RHS, operatorArray[0], $3->RHS);

    }

    | Operand SUBTRACT MathExpression

    | Operand MULTIPLY MathExpression 

    | Operand DIVIDE MathExpression

;*/

AddSubtractExpression:

    MultiplyDivideExpression

    | AddSubtractExpression SUBTRACT MultiplyDivideExpression {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        char Expression[100];

        sprintf(Expression, "%s%c%s", $1->RHS, operatorArray[0], $3->RHS);

        $$ = insertIntoAST(T_INT, "", Expression);

        printf("Called, expression is %s%c%s\n", $1->RHS, operatorArray[0], $3->RHS);

    }

    | AddSubtractExpression ADD MultiplyDivideExpression {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        char Expression[100];

        sprintf(Expression, "%s%c%s", $1->RHS, operatorArray[0], $3->RHS);

        $$ = insertIntoAST(T_INT, "", Expression);

        printf("Called, expression is %s%c%s\n", $1->RHS, operatorArray[0], $3->RHS);

    }
    
;

MultiplyDivideExpression:

    Operand

    | MultiplyDivideExpression MULTIPLY Operand {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        char Expression[100];

        sprintf(Expression, "%s%c%s", $1->RHS, operatorArray[0], $3->RHS);

        $$ = insertIntoAST(T_INT, "", Expression);

        printf("Called, expression is %s%c%s\n", $1->RHS, operatorArray[0], $3->RHS);

    }

    | MultiplyDivideExpression DIVIDE Operand {

        char value[5];

        char operatorArray[3];

        sprintf(operatorArray, "%s", $2);

        char Expression[100];

        sprintf(Expression, "%s%c%s", $1->RHS, operatorArray[0], $3->RHS);

        $$ = insertIntoAST(T_INT, "", Expression);

        printf("Called, expression is %s%c%s\n", $1->RHS, operatorArray[0], $3->RHS);

    }

;

Operand:

    IDENTIFIER {
        
        // Are we in a function?
        if(Scope != 0){
            // Is the variable whose value we are trying to extract declared in the function?
           if(SymbolTableExistsExternalFunctionCall($1, Scope)){


            // Is the variable a parameter?
            if(SymbolTableGetSymbolType($1, Scope) == S_FUNCTION_PARAMETER) {
                $$ = insertIntoAST(T_INT, "", $1);
                break;
            } else {

                $$ = insertIntoAST(T_INT, "", $1);
                
            }

           } else {

            // It doesn't, so the variable must be declared in the global scope.
            if(SymbolTableExistsExternalFunctionCall($1, 0)) {


                $$ = insertIntoAST(T_INT, "", $1);

            } else {
                // The variable doesn't exist in the function or global scope, meaning it doesn't exist, or we are accessing something we shouldn't. Failure!
            fprintf(stderr, "Parser Error: Variable %s does not exist in the function or global scope.", $1);
            exit(EXIT_FAILURE);

            }

           }
        } else {

            $$ = insertIntoAST(T_INT, "", $1);

        }

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