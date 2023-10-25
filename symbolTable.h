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
    struct Symbol* Adjacent;
    int IsSymbolUsed;
} Symbol;

SymbolValueType SymbolTableMatchSymbolValueType(Symbol* node) {
    switch (node->SymbolNodeType) {
        case T_INT: return S_INT;
        case T_FLOAT: return S_FLOAT;
        case T_CHAR: return S_CHAR;
        case T_FUNCTION: return S_FUNCTION_CHILD;    
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

char* SymbolTableGetValue(const char* identifier) {

    Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to get a value of a Symbol that doesn't exist in the table!");
    char buffer[100];

    switch(Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType) {
        case S_INT: 
            snprintf(buffer, sizeof(buffer), "%d", Node->SymbolValueWrapper.SymbolValue.SymbolValueInt);
            break;
        case S_FLOAT: 
            snprintf(buffer, sizeof(buffer), "%f", Node->SymbolValueWrapper.SymbolValue.SymbolValueFloat);
            break;
        case S_CHAR: 
            return Node->SymbolValueWrapper.SymbolValue.SymbolValueChar;
    }
    return strdup(buffer);

}

void SymbolTableSetValue(const char* identifier, char* value) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to set a value of a Symbol that doesn't exist in the table!");

   // set symbol value based on symbol value node type
   switch(Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType) {
    case S_INT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueInt = atoi(value);
        break;
    case S_FLOAT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueFloat = atof(value);
        break;
    case S_CHAR:
        char* buf = (char*)value;
        Node->SymbolValueWrapper.SymbolValue.SymbolValueChar = strdup(buf);
        break;
    case S_FUNCTION_CHILD:
        break;
   }
}


NodeType SymbolTableGetNodeType(const char* identifier) {

   Symbol* Node = SymbolTableExistsHandler(identifier, "Trying to get the NodeType of a Symbol that doesn't exist in the table!");
   
   return Node->SymbolNodeType;

}

void SymbolTablePrint() {

    printf("\nidentifier    SymbolValue  SymbolType  SymbolNodeType\n");
    printf("-                                               -\n");

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        printf("%10s %10s %10s %10s\n", Node->SymbolIdentifier, SymbolTableGetValue(Node->SymbolIdentifier), SymbolTypeToString(Node->SymbolType), nodeTypeToString(Node->SymbolNodeType));

        Node = Node->Adjacent;

    }

    printf("-                                               -\n\n");
}

#endif // SYMBOL_TABLE_H
