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
    int IsSymbolUsed;
} Symbol;

Symbol* GlobalSymbolTable = NULL;

void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType) {

    Symbol* Node = malloc(sizeof(Symbol));

    strcpy(Node->SymbolIdentifier, identifier);
    Node->SymbolType = symbolType;
    Node->SymbolNodeType = symbolNodeType;
    Node->Adjacent = GlobalSymbolTable;
    Node->IsSymbolUsed = 0;

    GlobalSymbolTable = Node;
}

void SymbolTableSetSymbolUsed(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried setting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            Node->IsSymbolUsed = 1;
            return;
        }

        Node = Node->Adjacent;

   }
    
}

int SymbolTableGetSymbolUsed(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried setting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->IsSymbolUsed;
        }

        Node = Node->Adjacent;

   }
   
}

int SymbolTableExists(char identifier[50]) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if (strcmp(Node->SymbolIdentifier, identifier) == 0) {
            return 1;
        }

        Node = Node->Adjacent;

    }
    return 0;
}

void SymbolTablePrint() {

    printf("\nidentifier    SymbolValue  SymbolType  SymbolNodeType\n");
    printf("-                                               -\n");

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        printf("%10s %10s %10s %10s\n", Node->SymbolIdentifier, Node->SymbolValue, SymbolTypeToString(Node->SymbolType), nodeTypeToString(Node->SymbolNodeType));

        Node = Node->Adjacent;

    }

    printf("-                                               -\n\n");
}

char* SymbolTableGetValue(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->SymbolValue;
        }

        Node = Node->Adjacent;

   }


}

void SymbolTableSetValue(char identifier[50], char value[25]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");
    
    return NULL;

   }

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            strcpy(Node->SymbolValue, value);
            //SymbolTablePrint();
            return;
        }

        Node = Node->Adjacent;

   }

}

NodeType SymbolTableGetNodeType(char identifier[50]) {

   if(!SymbolTableExists(identifier)){
    perror("SymbolTable Error! Tried getting a value of a symbol that doesn't exist!");

   }

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->SymbolNodeType;
        }

        Node = Node->Adjacent;

   }

}

#endif // SYMBOL_TABLE_H
