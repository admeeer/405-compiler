// Alexander Peltier, Matthew Powers, Parker Spaan
// CST 405


%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "AST.h"
//#include "sT.h"
//#include "IRc.h"
//#include "MIPSc.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

%}

%union {

    int number;
    float float_T;
    char character;
    char* string;

}

%token <string> identifier;
%token <number> integer
%token <float_T> float
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


