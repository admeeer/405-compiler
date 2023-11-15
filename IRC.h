
// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef IRC_H
#define IRC_H

#include "MIPSC.h"

/* 
We initialize two files, IRCMain & IRCData that merge into IRC
We did this so we can write to .data and .text concurrently
*/
FILE * IRCMain;
FILE * IRCData;
FILE * IRC;

extern int Scope;

extern char* BuildDirectory;

int label = 0;

int GetNewLabel() {
    label = label + 1;
    return label;
}

int GetCurrentLabel() {
    return label;
}

void IRInitializeFile() {
    char IRCMainAbsolutePath[256];
    char IRCDataAbsolutePath[256];

    snprintf(IRCMainAbsolutePath, sizeof(IRCMainAbsolutePath), "%s/output/irc/IRCMain.ir", BuildDirectory);
    snprintf(IRCDataAbsolutePath, sizeof(IRCDataAbsolutePath), "%s/output/irc/IRCData.ir", BuildDirectory);

    IRCMain = fopen(IRCMainAbsolutePath, "w");

    if (IRCMain == NULL) {
        perror("Failed to initialize file IRCMain !");
    }

    IRCData = fopen(IRCDataAbsolutePath, "w");

    if (IRCData == NULL) {
        perror("Failed to initialize file IRCData !");
    }

    char IRCAbsolutePath[256];

    snprintf(IRCAbsolutePath, sizeof(IRCAbsolutePath), "%s/output/irc/IRC.ir", BuildDirectory);

    IRC = fopen(IRCAbsolutePath, "w");
    if (IRC == NULL) {
        perror("Failed to initialize file IRC !");
    }

    fprintf(IRCMain, "main:\n\n");
    fprintf(IRCData, "data:\n\n");

    fclose(IRCMain);
    fclose(IRCData);
}

/*
Create IR code based on the AST and SymbolTables
This function should use the AST NodeType and SymbolTable SymbolType to correctly emit IR code
*/
void IREmission(struct AST* leaf) {

    if (leaf == NULL) {
        return;
    }

    char IRCDataAbsolutePath[256];
    char IRCMainAbsolutePath[256];

    snprintf(IRCDataAbsolutePath, sizeof(IRCDataAbsolutePath), "%s/output/irc/IRCData.ir", BuildDirectory);
    snprintf(IRCMainAbsolutePath, sizeof(IRCMainAbsolutePath), "%s/output/irc/IRCMain.ir", BuildDirectory);

    switch (leaf->nodeType) {

        case T_SWITCH:

            struct AST* SwitchNode = malloc(sizeof(struct AST));
            SwitchNode = leaf;

            Scope = leaf->StructType.SwitchNode.Scope;

            int breakLabel = GetNewLabel();

            int nextCaseLabel = GetNewLabel();

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            SwitchNode = SwitchNode->StructType.SwitchNode.CaseList;

            fprintf(IRCMain, "if %s != %s goto L%d\n", leaf->RHS, SwitchNode->RHS, nextCaseLabel);

            fclose(IRCMain);

            IREmission(SwitchNode->left);

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            fprintf(IRCMain, "goto L%d\n", breakLabel);

            fclose(IRCMain);

            while(SwitchNode->right){

                IRCMain = fopen(IRCMainAbsolutePath, "a");

                SwitchNode = SwitchNode->right;

                int CurrentCaseLabel = GetCurrentLabel();

                int NextCaseLabel = GetNewLabel();

                fprintf(IRCMain, "L%d: if %s != %s goto L%d\n", CurrentCaseLabel, leaf->RHS, SwitchNode->RHS, NextCaseLabel);

                fclose(IRCMain);

                IREmission(SwitchNode->left);

                IRCMain = fopen(IRCMainAbsolutePath, "a");

                fprintf(IRCMain, "goto L%d\n", breakLabel);

                fclose(IRCMain);

            }

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            fprintf(IRCMain, "L%d: goto L%d\nL%d:\n", GetCurrentLabel(), breakLabel, breakLabel);

            fclose(IRCMain);

            Scope = 0;

            break;

        case T_IF:

            struct AST* IfNode = malloc(sizeof(struct AST));
            IfNode = leaf;
            IfNode = IfNode->StructType.IfNode.condition;

            Scope = leaf->StructType.IfNode.Scope;

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            int ifBlockLabel = GetNewLabel();

            fprintf(IRCMain, "if %s %s %s goto L%d\n", IfNode->LHS, IfNode->StructType.ConditionNode.Operator, IfNode->RHS, ifBlockLabel);
            
            fprintf(IRCMain, "L%d:\n", ifBlockLabel);

            fclose(IRCMain);

            if(leaf->left){
                IREmission(leaf->left);
            }

            Scope = 0;

            break;
        
        case T_IF_ELSE:

            struct AST* IfElseNode = malloc(sizeof(struct AST));
            IfElseNode = leaf;
            IfElseNode = IfElseNode->StructType.IfElseNode.Condition;

            Scope = leaf->StructType.IfElseNode.Scope;

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            int ifElseBlockLabel = GetNewLabel();
            int ElseBlockLabel = GetNewLabel();

            fprintf(IRCMain, "if %s %s %s goto L%d else goto L%d\n", IfElseNode->LHS, IfElseNode->StructType.ConditionNode.Operator, IfElseNode->RHS, ifElseBlockLabel, ElseBlockLabel);
            
            fprintf(IRCMain, "L%d:\n", ifElseBlockLabel);

            fclose(IRCMain);

            if(leaf->left){
                IREmission(leaf->left);
            }

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            fprintf(IRCMain, "L%d:\n", ElseBlockLabel);

            fclose(IRCMain);

            if(leaf->StructType.IfElseNode.ElseCodeBlock) {
                IREmission(leaf->StructType.IfElseNode.ElseCodeBlock);
            }

            Scope = 0;

            break;

        case T_EQUALS_STRUCT_VARIABLE:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s = %s.%s\n", leaf->LHS, leaf->RHS, leaf->StructType.StructEquals.variable);
            fclose(IRCMain);

            break;
        
        case T_STRUCT_VARIABLE_EQUALS:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s.%s = %s\n", leaf->LHS, leaf->StructType.StructEquals.variable, leaf->RHS);
            fclose(IRCMain);

            break;
        
        case T_ARRAY_ELEMENT_EQUALS:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s[%d] = %s\n", leaf->LHS, leaf->StructType.ArrayEquals.index, leaf->RHS);
            fclose(IRCMain);

            break;
        
        case T_EQUALS_ARRAY_ELEMENT:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s = %s[%d]\n", leaf->LHS, leaf->RHS, leaf->StructType.ArrayEquals.index);
            fclose(IRCMain);

            break;

        case T_RETURN:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "return %s\n", leaf->RHS);
            fclose(IRCMain);

            break;

        case T_EQUALS_FUNCTION:

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s = %s(", leaf->LHS, leaf->RHS);

            if(leaf->left){
                struct AST* node = malloc(sizeof(struct AST));
                node = leaf->left;
                //fprinft("%s", leaf->RHS);
                while(node){
                    fprintf(IRCMain, "%s", node->RHS);
                    if(node->right){
                        fprintf(IRCMain, ", ");
                    }
                    node = node->right;
                }
            }

            fprintf(IRCMain, ")\n");
            fclose(IRCMain);
            break;
        case T_EQUALS:
            //printf("Checking if symbol %s is used at Scope %d\n", leaf->LHS, Scope);
            int equalsScope;
            if(SymbolTableExistsExternalFunctionCall(leaf->LHS, 0)) {
                equalsScope = 0;
            } else {
                equalsScope = Scope;
            }

            if(Scope == 0) {

                IRCData = fopen(IRCDataAbsolutePath, "a");
                fprintf(IRCData, "%s word %s\n", leaf->LHS, leaf->RHS);
                fclose(IRCData);

            }

            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "%s = %s\n", leaf->LHS, leaf->RHS);
            fclose(IRCMain);

            break;
        
        case T_WRITE:
            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "write %s\n", leaf->RHS);
            fclose(IRCMain);
            break;
        case T_FUNCTION:

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            Scope = SymbolTableGetSymbolScope(leaf->RHS, S_FUNCTION);

            fprintf(IRCMain, "function %s %s (", leaf->LHS, leaf->RHS);
            
            if(leaf->StructType.FunctionNode.Parameters){

                //struct AST* IfElseNode = malloc(sizeof(struct AST));
                struct AST* ParameterPointer = malloc(sizeof(struct AST));

                ParameterPointer = leaf->StructType.FunctionNode.Parameters;

                while(ParameterPointer) {
                    
                    fprintf(IRCMain, "%s %s ", ParameterPointer->LHS, ParameterPointer->RHS);
                    ParameterPointer = ParameterPointer->right;

                }

            }

            fprintf(IRCMain, ") {\n");

            fclose(IRCMain);

            if(leaf->StructType.FunctionNode.CodeBlock){

                IREmission(leaf->StructType.FunctionNode.CodeBlock);
                
            }

            IRCMain = fopen(IRCMainAbsolutePath, "a");

            fprintf(IRCMain, "}\n");

            fclose(IRCMain);

            Scope = 0;
            //printf("Function call in IREmission !\n");


            break;
    }



    IREmission(leaf->right);
}

void IREmissionCleanUp() {
    char IRCDataAbsolutePath[256];
    char IRCMainAbsolutePath[256];
    char IRCPath[256];

    snprintf(IRCDataAbsolutePath, sizeof(IRCDataAbsolutePath), "%s/output/irc/IRCData.ir", BuildDirectory);
    snprintf(IRCMainAbsolutePath, sizeof(IRCMainAbsolutePath), "%s/output/irc/IRCMain.ir", BuildDirectory);
    snprintf(IRCPath, sizeof(IRCPath), "%s/output/irc/IRC.ir", BuildDirectory);

    IRCData = fopen(IRCDataAbsolutePath, "r");
    IRCMain = fopen(IRCMainAbsolutePath, "r");
    IRC = fopen(IRCPath, "w");

    if (!IRC || !IRCMain || !IRCData) {
        perror("Failed to open one or more files for IR Emission CleanUp !");
    }

    char buf[100];

    while (fgets(buf, sizeof(buf), IRCData)) {
        fprintf(IRC, "%s", buf);
    }
    
    fprintf(IRC, "\n");

    while (fgets(buf, sizeof(buf), IRCMain)) {
        fprintf(IRC, "%s", buf);
    }

    fclose(IRCData);
    fclose(IRCMain);
    fclose(IRC);

    MIPSEmission();
}

#endif // IRC_H