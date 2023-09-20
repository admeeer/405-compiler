%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AbstractSyntaxTree.h"
#include "symbolTable.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);
%}

%union {
    char* string;
    int number;
    ASTNode* node;  // For nodes of our AST
}

%token<string> TYPE
%token<string> ID
%token<number> NUMBER
%token WRITE SEMICOLON EQ

%type<node> program declaration declarations statement statements expression

%%

program: declarations statements
        ;



declarations:
           /* empty */
           | declarations declaration
           ;

declaration: TYPE ID SEMICOLON
           {
               ASTNode* new_node = malloc(sizeof(ASTNode));
               new_node->type = NODE_TYPE_DECLARATION;
               new_node->declaration.type = $1;
               new_node->declaration.identifier = $2;
               add_symbol($2, $1);
               $$ = new_node;
           }
           ;

statements: 
           /* empty */
           | statements statement
           ;

statement: ID EQ expression SEMICOLON
          {
              if (!lookup_symbol($1)) {
                  fprintf(stderr, "Error: Variable %s not declared.\n", $1);
                  exit(1);
              }
              ASTNode* new_node = malloc(sizeof(ASTNode));
              new_node->type = NODE_TYPE_ASSIGNMENT;
              new_node->assignment.identifier = $1;
              new_node->assignment.expression = $3;
              $$ = new_node;
          }
          ;

expression: NUMBER
           {
               ASTNode* new_node = malloc(sizeof(ASTNode));
               new_node->type = NODE_TYPE_NUMBER;
               new_node->number = $1;
               $$ = new_node;
           }
         | ID
           {
               ASTNode* new_node = malloc(sizeof(ASTNode));
               new_node->type = NODE_TYPE_ID;
               new_node->identifier = $1;
               $$ = new_node;
           }
           ;

%%

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("fopen");
            return 1;
        }
    }

    yyparse();

    if (yyin != stdin) {
        fclose(yyin);
    }

    return 0;
}

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}
