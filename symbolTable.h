// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    S_VARIABLE
} SymbolType;

const char* symbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        default: return "UNDEFINED";
    }
}

typedef struct Symbol {
    char symbolIdentifier[50];
    SymbolType symbolType;
    NodeType nodeType;
    char symbolValue[25];
    struct Symbol* adjacent;
} Symbol;

Symbol* globalSymbolTable = NULL;

// Function to add a symbol
void symbolTable_insertInto(char identifier[50], char value[50], SymbolType symbolType, NodeType nodeType) {

    Symbol* symbol = malloc(sizeof(Symbol));

    strcpy(symbol->symbolIdentifier, identifier);
    symbol->symbolType = symbolType;
    symbol->nodeType = nodeType;
    strcpy(symbol->symbolValue, value);
    symbol->adjacent = globalSymbolTable;

    globalSymbolTable = symbol;
}

int symbolTable_LookUp(char identifier[50]) {

    Symbol* node = globalSymbolTable;

    while (node) {
        if (strcmp(node->symbolIdentifier, identifier) == 0) {
            return 1;
        }
        node = node->adjacent;
    }
    return 0;
}



void symbolTable_print() {

    printf("identifier  symbolType  nodeType\n");
    printf("-                              -\n");

    Symbol* node = globalSymbolTable;

    while (node) {
        printf("%10s %10s %10s", node->symbolIdentifier, symbolTypeToString(node->symbolType), nodeTypeToString(node->nodeType));
    }

    printf("-                              -\n");
}


char* SymbolTableGetValue(char identifier[50]) {

   if(!symbolTable_LookUp(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* node = globalSymbolTable;

   while (node) {
        if (!strcmp(node->symbolIdentifier, identifier)) {
            return node->symbolValue;
        }
   }

}

void SymbolTableSetValue(char identifier[50], char value[25]) {

   if(!symbolTable_LookUp(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* node = globalSymbolTable;

   while (node) {
        if (!strcmp(node->symbolIdentifier, identifier)) {
            strcpy(node->symbolValue, value);
        }
   }

}


#endif // SYMBOL_TABLE_H
