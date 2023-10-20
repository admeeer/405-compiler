// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    S_VARIABLE,
    S_FUNCTION
} SymbolType;

const char* SymbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        case S_FUNCTION: return "FUNCTION";
        default: return "UNDEFINED";
    }
}

typedef struct Symbol {
    char SymbolIdentifier[50];
    SymbolType SymbolType;
    NodeType SymbolNodeType;

    union SymbolValue {
        int SymbolValueInt;
        float SymbolValueFloat;
        char* SymbolValueChar;
        struct Symbol* SymbolValueChild;
    };
    char SymbolValue[25];
    struct Symbol* Adjacent;
    int IsSymbolUsed;
} Symbol;

Symbol* GlobalSymbolTable = NULL;

int SymbolTableExists(const char* identifier) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if (strcmp(Node->SymbolIdentifier, identifier) == 0) {
            return 1;
        }

        Node = Node->Adjacent;

    }
    return 0;
}

void SymbolTableExistsHandler(const char* identifier, const char* errorDescription) {
    if(!SymbolTableExists(identifier)) {
        fprintf(stderr, "Symbol Table Error: %s\n", errorDescription);
        exit(EXIT_FAILURE);
    }
}

void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType) {

    Symbol* Node = malloc(sizeof(Symbol));

    strcpy(Node->SymbolIdentifier, identifier);
    Node->SymbolType = symbolType;
    Node->SymbolNodeType = symbolNodeType;
    Node->Adjacent = GlobalSymbolTable;
    Node->IsSymbolUsed = 0;

    GlobalSymbolTable = Node;
}

void SymbolTableSetSymbolUsed(const char* identifier) {

   SymbolTableExistsHandler(identifier, "Trying to set a Symbol as used that doesn't exist in the table!");

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            Node->IsSymbolUsed = 1;
            return;
        }

        Node = Node->Adjacent;

   }
    
}

int SymbolTableGetSymbolUsed(const char* identifier) {

   SymbolTableExistsHandler(identifier, "Trying to get whether a Symbol is used that doesn't exist in the table!");

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->IsSymbolUsed;
        }

        Node = Node->Adjacent;

   }

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

char* SymbolTableGetValue(const char* identifier) {

   SymbolTableExistsHandler(identifier, "Trying to get a value of a Symbol that doesn't exist in the table!");

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->SymbolValue;
        }

        Node = Node->Adjacent;

   }


}

void SymbolTableSetValue(const char* identifier, char value[25]) {

   SymbolTableExistsHandler(identifier, "Trying to set a value of a Symbol that doesn't exist in the table!");

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

NodeType SymbolTableGetNodeType(const char* identifier) {

   SymbolTableExistsHandler(identifier, "Trying to get the NodeType of a Symbol that doesn't exist in the table!");

   Symbol* Node = GlobalSymbolTable;

   while (Node) {

        if (!strcmp(Node->SymbolIdentifier, identifier)) {
            return Node->SymbolNodeType;
        }

        Node = Node->Adjacent;

   }

}


#endif // SYMBOL_TABLE_H
