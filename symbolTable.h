// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    S_VARIABLE,
    S_FUNCTION
} SymbolType;

typedef enum {
    S_INT,
    S_FLOAT,
    S_CHAR,
    S_FUNCTION_CHILD
} SymbolValueType;

const char* SymbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        case S_FUNCTION: return "FUNCTION";
        default: return "UNDEFINED";
    }
}

const char* SymbolValueTypeToString(SymbolValueType type) {
    switch (type) {
        case S_INT: return "INT";
        case S_FLOAT: return "FLOAT";
        case S_CHAR: return "CHAR";
        case S_FUNCTION_CHILD: return "FUNCTION";
        default: return "UNDEFINED";
    }
}

typedef struct Symbol {
    char SymbolIdentifier[50];
    SymbolType SymbolType;
    NodeType SymbolNodeType;

    struct symbolValueWrapper {
        SymbolValueType SymbolValueWrapperSymbolValueType;
        union symbolValue {
            int SymbolValueInt;
            float SymbolValueFloat;
            char* SymbolValueChar;
            struct Symbol* SymbolValueChild;
        } SymbolValue;
    } SymbolValueWrapper;

    char SymbolValue[25];
    struct Symbol* Adjacent;
    int IsSymbolUsed;
} Symbol;

SymbolValueType SymbolTableMatchSymbolValueType(Symbol* node) {
    switch (node->SymbolNodeType) {
        case T_INT: return S_INT;
        case T_FLOAT: return S_FLOAT;
        case T_CHAR: return T_CHAR;
        case T_FUNCTION: return T_FUNCTION;    
    }
}

Symbol* GlobalSymbolTable = NULL;

Symbol* SymbolTableExists(const char* identifier) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if (strcmp(Node->SymbolIdentifier, identifier) == 0) {
            return Node;
        }

        Node = Node->Adjacent;

    }
    return NULL;
}

Symbol* SymbolTableExistsHandler(const char* identifier, const char* errorDescription) {

    Symbol* Node = SymbolTableExists(identifier);

    if(Node){
        return Node;
    } else {
        fprintf(stderr, "Symbol Table Error: %s\n", errorDescription);
        exit(EXIT_FAILURE);
        return NULL; // clarity  
    }

}

void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType) {

    Symbol* Node = malloc(sizeof(Symbol));

    strcpy(Node->SymbolIdentifier, identifier);
    Node->SymbolType = symbolType;
    Node->SymbolNodeType = symbolNodeType;
    Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType = SymbolTableMatchSymbolValueType(Node);
    Node->Adjacent = GlobalSymbolTable;
    Node->IsSymbolUsed = 0;

    GlobalSymbolTable = Node;
}

void SymbolTableSetSymbolUsed(const char* identifier) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to set a Symbol as used that doesn't exist in the table!");

    Node->IsSymbolUsed = 1;
    return;

}

int SymbolTableGetSymbolUsed(const char* identifier) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to get whether a Symbol is used that doesn't exist in the table!");

    return Node->IsSymbolUsed;

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

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to get a value of a Symbol that doesn't exist in the table!");

   return Node->SymbolValue;

}

void SymbolTableSetValue(const char* identifier, void* value) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to set a value of a Symbol that doesn't exist in the table!");
   // get symbol NodeType, set symbol value SymbolType, set symbol value

   


}

NodeType SymbolTableGetNodeType(const char* identifier) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to get the NodeType of a Symbol that doesn't exist in the table!");
   
   return Node->SymbolNodeType;

}


#endif // SYMBOL_TABLE_H
