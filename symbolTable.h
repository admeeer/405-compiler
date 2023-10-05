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

Symbol* GlobalSymbolTable = NULL;

// Function to add a symbol
void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType nodeType) {

    Symbol* symbol = malloc(sizeof(Symbol));

    strcpy(symbol->symbolIdentifier, identifier);
    symbol->symbolType = symbolType;
    symbol->nodeType = nodeType;
    //strcpy(symbol->symbolValue, value);
    symbol->adjacent = GlobalSymbolTable;

    GlobalSymbolTable = symbol;
}

int SymbolTableExists(char identifier[50]) {

    Symbol* node = GlobalSymbolTable;

    while (node) {

        if (strcmp(node->symbolIdentifier, identifier) == 0) {
            return 1;
        }

        node = node->adjacent;

    }
    return 0;
}



void SymbolTablePrint() {

    printf("\nidentifier    symbolValue  symbolType  nodeType\n");
    printf("-                                               -\n");

    Symbol* node = GlobalSymbolTable;

    while (node) {

        printf("%10s %10s %10s %10s\n", node->symbolIdentifier, node->symbolValue, symbolTypeToString(node->symbolType), nodeTypeToString(node->nodeType));

        node = node->adjacent;

    }

    printf("-                                               -\n\n");
}


char* SymbolTableGetValue(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* node = GlobalSymbolTable;

   while (node) {

        if (!strcmp(node->symbolIdentifier, identifier)) {
            return node->symbolValue;
        }

        node = node->adjacent;

   }


}

void SymbolTableSetValue(char identifier[50], char value[25]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* node = GlobalSymbolTable;

   while (node) {

        if (!strcmp(node->symbolIdentifier, identifier)) {
            strcpy(node->symbolValue, value);
            SymbolTablePrint();
            return;
        }

        node = node->adjacent;

   }

}

NodeType SymbolTableGetNodeType(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");

   }

   Symbol* node = GlobalSymbolTable;

   while (node) {

        if (!strcmp(node->symbolIdentifier, identifier)) {
            return node->nodeType;
        }

        node = node->adjacent;

   }

}


#endif // SYMBOL_TABLE_H
