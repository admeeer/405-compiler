
// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

/* 
We initialize two files, IRCMain & IRCData that merge into IRC
We did this so we can write to .data and .text concurrently
*/
FILE * IRCMain;
FILE * IRCData;
FILE * IRC;

void IRInitializeFile() {

    IRCMain = fopen("IRCMain.ir", "w");

    if(IRCMain == NULL) {
        perror("Failed to initialize file IRCMain !");
    }

    IRCData = fopen("IRCData.ir", "w");

    if(IRCData == NULL) {
        perror("Failed to initialize file IRCData !");
    }

    IRC = fopen("IRC.ir", "w");

    if(IRC == NULL) {
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

    if(leaf == NULL) {
        return;
    }

    switch(leaf->nodeType){
        case T_EQUALS:
            if(SymbolTableGetSymbolUsed(leaf->LHS)){
                IRCData = fopen("IRCData.ir", "a");
                fprintf(IRCData, "%s: word %s\n", leaf->LHS, leaf->RHS);
                fclose(IRCData);
            }
            break;
        case T_WRITE:
            IRCMain = fopen("IRCMain.ir", "a");
            fprintf(IRCMain, "write %s\n", leaf->RHS);
            fclose(IRCMain);
            
    }

    IREmission(leaf->left);
    IREmission(leaf->right);
  
}

void IREmissionCleanUp() {

    IRCData = fopen("IRCData.ir", "r");
    IRCMain = fopen("IRCMain.ir", "r");
    IRC = fopen("IRC.ir", "w");

    if(!IRC || !IRCMain || !IRCData) {
        perror("Failed to open one or more files for IR Emission CleanUp !");
    }

    char buf[100];

    while (fgets(buf, sizeof(buf), IRCData)) {
        fprintf(IRC, "%s", buf);

    }

    //fprintf(IRC, "\n");

    while(fgets(buf, sizeof(buf), IRCMain)) {
        fprintf(IRC, "%s", buf);
    }

    fclose(IRCData);
    fclose(IRCMain);
    fclose(IRC);

    MIPSEmission();

}

