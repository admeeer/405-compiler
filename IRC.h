
// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef IRC_H
#define IRC_H

/* 
We initialize two files, IRCMain & IRCData that merge into IRC
We did this so we can write to .data and .text concurrently
*/
FILE * IRCMain;
FILE * IRCData;
FILE * IRC;

extern char* BuildDirectory;

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
    //printf("Loop through IR emission\n");
    switch (leaf->nodeType) {
        case T_RETURN:
            //printf("IRC processed an AST node with type RETURN\n");
            break;
        case T_EQUALS:
            if (SymbolTableGetSymbolUsed(leaf->LHS, 0)) {
                IRCData = fopen(IRCDataAbsolutePath, "a");
                fprintf(IRCData, "%s: word %s\n", leaf->LHS, leaf->RHS);
                fclose(IRCData);
            }
            break;
        case T_WRITE:
            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "write %s\n", leaf->RHS);
            fclose(IRCMain);
            break;
        case T_FUNCTION:
            //printf("This was called. Leaf left is %s and right is %s, type is %s\n", leaf->LHS, leaf->RHS, nodeTypeToString(leaf->nodeType));
            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "function %s %s (", leaf->LHS, leaf->RHS);
            
            if(SymbolTableSymbolValueSymbolExists(leaf->RHS, SymbolTableGetSymbolScope(leaf->RHS))){

                int FunctionScope = SymbolTableGetSymbolScope(leaf->RHS);

                if(SymbolTableSymbolValueSymbolExists(leaf->RHS, FunctionScope)){
                    //char* Parameter = SymbolTableGetSymbolValueSymbol(leaf->RHS, FunctionScope);
                    Symbol* Parameter = SymbolTableGetSymbolValueSymbol(leaf->RHS, FunctionScope);
                    //fprintf(IRCMain, "%s %s", SymbolValueTypeToString(SymbolTableGetSymbolValueType(Parameter, FunctionScope)), Parameter);

                    while(1) {
                        //fprintf(IRCMain, "%s %s", SymbolValueTypeToString(SymbolTableGetSymbolValueType(Parameter, FunctionScope)), Parameter);
                        fprintf(IRCMain, "%s %s", SymbolValueTypeToString(Parameter->SymbolValueWrapper.SymbolValueWrapperSymbolValueType), Parameter->SymbolIdentifier);
                        if(Parameter->Adjacent){
                            fprintf(IRCMain, ", ");
                            Parameter = Parameter->Adjacent;
                        }else {
                            break;
                        }
                    }
                    
                    //printf("Parameter is %s\n", Parameter);
                    /*while(1) {

                        fprintf(IRCMain, "%s %s", SymbolValueTypeToString(SymbolTableGetSymbolValueType(Parameter, FunctionScope)), Parameter);

                        if(SymbolTableSymbolValueAdjacentExists(Parameter, FunctionScope)) {
                            Parameter = SymbolTableGetSymbolValueAdjacent(Parameter, FunctionScope);
                        } else {
                            break;
                        }

                    }*/                  
                } else {
                    printf("Doesn't exist?\n");
                }

                fprintf(IRCMain, ") {\n");

                fprintf(IRCMain, "}\n");

                fclose(IRCMain);

                break;
            }

            fclose(IRCMain);

            printf("Function call in IREmission !\n");

            break;
    }

    IREmission(leaf->left);
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

    while (fgets(buf, sizeof(buf), IRCMain)) {
        fprintf(IRC, "%s", buf);
    }

    fclose(IRCData);
    fclose(IRCMain);
    fclose(IRC);

    MIPSEmission();
}

#endif // IRC_H