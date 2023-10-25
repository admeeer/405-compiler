// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

int SymbolTableScope = 0;

int SymbolTableDefineScopeValue() {
    return SymbolTableScope += 1;
}

typedef enum {
    S_VARIABLE,
    S_FUNCTION,
    S_FUNCTION_PARAMETER
} SymbolType;

typedef enum {
    SV_INT,
    SV_FLOAT,
    SV_CHAR,
    SV_PARAMETER,
    SV_FUNCTION
} SymbolValueType;

const char* SymbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        case S_FUNCTION: return "FUNCTION";
        case S_FUNCTION_PARAMETER: return "PARAMETER";
        default: return "UNDEFINED";
    }
}

const char* SymbolValueTypeToString(SymbolValueType type) {
    switch (type) {
        case SV_INT: return "INT";
        case SV_FLOAT: return "FLOAT";
        case SV_CHAR: return "CHAR";
        case SV_PARAMETER: return "PARAMETER";
        case SV_FUNCTION: return "FUNCTION";
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
            struct Symbol* SymbolValueSymbol;
        } SymbolValue;
    } SymbolValueWrapper;
    struct Symbol* Adjacent;
    int SymbolScope;
    int IsSymbolUsed;
} Symbol;

SymbolValueType SymbolTableMatchSymbolValueType(Symbol* node) {
    switch (node->SymbolNodeType) {
        case T_INT: return SV_INT;
        case T_FLOAT: return SV_FLOAT;
        case T_CHAR: return SV_CHAR;
        case T_FUNCTION: return SV_FUNCTION;    
    }
}

Symbol* GlobalSymbolTable = NULL;

int SymbolTableExistsExternalFunctionCall(const char* identifier, int scope) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if (strcmp(Node->SymbolIdentifier, identifier) == 0 && Node->SymbolScope == scope) {
            return 1;
        }

        Node = Node->Adjacent;

    }

    return 0;
}

Symbol* SymbolTableExists(const char* identifier, int scope) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if (strcmp(Node->SymbolIdentifier, identifier) == 0 && Node->SymbolScope == scope) {
            return Node;
        }

        Node = Node->Adjacent;

    }
    return NULL;
}

Symbol* SymbolTableExistsHandler(const char* identifier, int scope, const char* errorDescription) {

    Symbol* Node = SymbolTableExists(identifier, scope);

    if(Node){
        return Node;
    } else {
        fprintf(stderr, "Symbol Table Error: %s\n", errorDescription);
        exit(EXIT_FAILURE);
        return NULL; // clarity  
    }

}

void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType, int scope) {

    Symbol* Node = malloc(sizeof(Symbol));

    strcpy(Node->SymbolIdentifier, identifier);
    Node->SymbolType = symbolType;
    Node->SymbolNodeType = symbolNodeType;
    Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType = SymbolTableMatchSymbolValueType(Node);
    Node->Adjacent = GlobalSymbolTable;
    Node->SymbolScope = scope;
    Node->IsSymbolUsed = 0;

    GlobalSymbolTable = Node;
}

int SymbolTableGetSymbolScope(const char* identifier, int scope) {
    
    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the Scope of a Symbol that doesn't exist in the table!");

    return Node->SymbolScope;
}

SymbolValueType SymbolTableGetSymbolValueTypeFromScope(int scope) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if(Node->SymbolScope == scope) {
            return Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType;
        }

        Node = Node->Adjacent;

    }

    fprintf(stderr, "Symbol Table Error: Trying to get the SymbolValueType of a Symbol at Scope %d that doesn't exist in the SymbolTable!\n", scope);
    exit(EXIT_FAILURE);
}

void SymbolTableSetSymbolUsed(const char* identifier, int scope) {

   Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set a Symbol as used that doesn't exist in the table!");

    Node->IsSymbolUsed = 1;
    return;

}

int SymbolTableGetSymbolUsed(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get whether a Symbol is used that doesn't exist in the table!");

    return Node->IsSymbolUsed;

}

char* SymbolTableGetValue(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get a value of a Symbol that doesn't exist in the table!");
    char buffer[100];

    switch(Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType) {
        case SV_INT: 
            snprintf(buffer, sizeof(buffer), "%d", Node->SymbolValueWrapper.SymbolValue.SymbolValueInt);
            break;
        case SV_FLOAT: 
            snprintf(buffer, sizeof(buffer), "%f", Node->SymbolValueWrapper.SymbolValue.SymbolValueFloat);
            break;
        case SV_CHAR: 
            return Node->SymbolValueWrapper.SymbolValue.SymbolValueChar;
    }
    return strdup(buffer);

}

void SymbolTableSetValue(const char* identifier, char* value, int scope) {

   Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set a value of a Symbol that doesn't exist in the table!");

   // set symbol value based on symbol value node type
   switch(Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType) {
    case SV_INT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueInt = atoi(value);
        break;
    case SV_FLOAT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueFloat = atof(value);
        break;
    case SV_CHAR:
        char* buf = (char*)value;
        Node->SymbolValueWrapper.SymbolValue.SymbolValueChar = strdup(buf);
        break;
    default: break;
   }
}

/*void SymbolTableSetSymbolValueSymbol(const char* parent, const char* child) {

    Symbol* ParentNode = SymbolTableExistsHandler(parent, "Trying to set a value of a Symbol that doesn't exist in the table!");

    Symbol* ChildNode = SymbolTableExistsHandler(parent, "Trying to get a Symbol that doesn't exist in the table!");


    ParentNode->SymbolValueWrapper.SymbolValue.SymbolValueSymbol = ChildNode;
}*/


NodeType SymbolTableGetNodeType(const char* identifier, int scope) {

   Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the NodeType of a Symbol that doesn't exist in the table!");
   
   return Node->SymbolNodeType;

}

void SymbolTablePrint() {

    printf("\nidentifier    SymbolValue  SymbolType  SymbolNodeType Scope\n");
    printf("-                                               -\n");

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        printf("%10s %10s %10s %10s %10d\n", Node->SymbolIdentifier, SymbolTableGetValue(Node->SymbolIdentifier, Node->SymbolScope), SymbolTypeToString(Node->SymbolType), nodeTypeToString(Node->SymbolNodeType), Node->SymbolScope);

        Node = Node->Adjacent;

    }

    printf("-                                               -\n\n");
}

#endif // SYMBOL_TABLE_H
