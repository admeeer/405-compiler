
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

extern int Scope;

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
            printf("Checking if symbol %s is used at Scope %d\n", leaf->LHS, Scope);
            if (SymbolTableGetSymbolUsed(leaf->LHS, Scope)) {

                if(Scope == 0) {

                    IRCData = fopen(IRCDataAbsolutePath, "a");
                    fprintf(IRCData, "%s: word %s\n", leaf->LHS, leaf->RHS);
                    fclose(IRCData);

                } else {

                    IRCMain = fopen(IRCMainAbsolutePath, "a");
                    fprintf(IRCMain, "%s = %s\n", leaf->LHS, leaf->RHS);
                    fclose(IRCMain);

                }


            }
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
            
            // If the function has any parameters
            if(SymbolTableSymbolValueSymbolExists(leaf->RHS, SymbolTableGetSymbolScope(leaf->RHS, S_FUNCTION))){
                // Get the scope value of the function
                int FunctionScope = SymbolTableGetSymbolScope(leaf->RHS, S_FUNCTION);

                if(SymbolTableSymbolValueSymbolExists(leaf->RHS, FunctionScope)){

                    Symbol* Parameter = SymbolTableGetSymbolValueSymbol(leaf->RHS, FunctionScope);

                    while(1) {

                        fprintf(IRCMain, "%s %s", SymbolValueTypeToString(Parameter->SymbolValueWrapper.SymbolValueWrapperSymbolValueType), Parameter->SymbolIdentifier);
                        if(Parameter->Adjacent){
                            fprintf(IRCMain, ", ");
                            Parameter = Parameter->Adjacent;
                            //printf("The parameter is %s\n", Parameter->SymbolIdentifier);
                        }else {
                            break;
                        }

                    }               
                } else {

                    //TO-DO ... something
                }

            }

            fprintf(IRCMain, ") {\n");

            fclose(IRCMain);

            if(leaf->left){

                //printf("Hey, we got here!\n");

                IREmission(leaf->left);
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

    while (fgets(buf, sizeof(buf), IRCMain)) {
        fprintf(IRC, "%s", buf);
    }

    fclose(IRCData);
    fclose(IRCMain);
    fclose(IRC);

    MIPSEmission();
}

#endif // IRC_H