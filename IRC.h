
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

    switch (leaf->nodeType) {
        case T_EQUALS:
            if (SymbolTableGetSymbolUsed(leaf->LHS)) {
                IRCData = fopen(IRCDataAbsolutePath, "a");
                fprintf(IRCData, "%s: word %s\n", leaf->LHS, leaf->RHS);
                fclose(IRCData);
            }
            break;
        case T_WRITE:
            IRCMain = fopen(IRCMainAbsolutePath, "a");
            fprintf(IRCMain, "write %s\n", leaf->RHS);
            fclose(IRCMain);
        case T_FUNCTION:
            printf("Function call in IREmission !\n");
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