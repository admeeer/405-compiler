// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    S_VARIABLE
} SymbolType;

const char* SymbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        default: return "UNDEFINED";
    }
}

typedef struct Symbol {
    char SymbolIdentifier[50];
    SymbolType SymbolType;
    NodeType SymbolNodeType;
    char SymbolValue[25];
    struct Symbol* Adjacent;
} Symbol;

Symbol* GlobalSymbolTable = NULL;

// Function to add a symbol
void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType) {

    Symbol* symbol = malloc(sizeof(Symbol));

    strcpy(symbol->SymbolIdentifier, identifier);
    symbol->SymbolType = symbolType;
    symbol->SymbolNodeType = symbolNodeType;
    //strcpy(symbol->SymbolValue, value);
    symbol->Adjacent = GlobalSymbolTable;

    GlobalSymbolTable = symbol;
}

int SymbolTableExists(char identifier[50]) {

    Symbol* node = GlobalSymbolTable;

    while (node) {

        if (strcmp(node->SymbolIdentifier, identifier) == 0) {
            return 1;
        }

        node = node->Adjacent;

    }
    return 0;
}



void SymbolTablePrint() {

    printf("\nidentifier    SymbolValue  SymbolType  SymbolNodeType\n");
    printf("-                                               -\n");

    Symbol* node = GlobalSymbolTable;

    while (node) {

        printf("%10s %10s %10s %10s\n", node->SymbolIdentifier, node->SymbolValue, SymbolTypeToString(node->SymbolType), nodeTypeToString(node->SymbolNodeType));

        node = node->Adjacent;

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

        if (!strcmp(node->SymbolIdentifier, identifier)) {
            return node->SymbolValue;
        }

        node = node->Adjacent;

   }


}

void SymbolTableSetValue(char identifier[50], char value[25]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* node = GlobalSymbolTable;

   while (node) {

        if (!strcmp(node->SymbolIdentifier, identifier)) {
            strcpy(node->SymbolValue, value);
            SymbolTablePrint();
            return;
        }

        node = node->Adjacent;

   }

}

NodeType SymbolTableGetNodeType(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");

   }

   Symbol* node = GlobalSymbolTable;

   while (node) {

        if (!strcmp(node->SymbolIdentifier, identifier)) {
            return node->SymbolNodeType;
        }

        node = node->Adjacent;

   }

}


#endif // SYMBOL_TABLE_H
