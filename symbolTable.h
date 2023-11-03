// CST 405 Alexander Peltier, Matthew Powers, Parker SpaanymbolValueType

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

int SymbolTableScope = 0;

int SymbolTableDefineScopeValue() {
    return SymbolTableScope += 1;
}

typedef enum {
    S_VARIABLE,
    S_ARRAY,
    S_FUNCTION,
    S_FUNCTION_PARAMETER
} SymbolType;

typedef enum {
    SV_INT,
    SV_FLOAT,
    SV_CHAR,
    SV_ARRAY,
    SV_PARAMETER,
    SV_FUNCTION,
    SV_UNDEFINED
} SymbolValueType;

const char* SymbolTypeToString(SymbolType type) {
    switch (type) {
        case S_VARIABLE: return "VARIABLE";
        case S_FUNCTION: return "FUNCTION";
        case S_ARRAY: return "ARRAY";
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
        case SV_ARRAY: return "ARRAY";
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
            struct symbolValueArray {
                int Length;
                union element {
                    int *intElement;
                    char *charElement;
                    float *floatElement;
                } Element;
            } SymbolValueArray;
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
        case T_ARRAY: return SV_ARRAY;
        case T_FUNCTION: return SV_FUNCTION;    
        default: return SV_UNDEFINED;
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
        fprintf(stderr, "Symbol Table Error: %s | %d , %s\n", identifier, scope, errorDescription);
        exit(EXIT_FAILURE);
        return NULL; // clarity  
    }

}

void SymbolTableSetSymbolValueArrayLength(const char* identifier, int scope, int length) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Tried to set the SymbolValueArray Length, but was null \n");

    Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Length = length;
    return;

    //fprintf(stderr, "Symbol Table Error: %s | %d , Tried to set the SymbolValueArray Length of %s at scope %d, but was null.\n", identifier, scope);

}

/*Symbol* SymbolTableGetSymbolValueArrayElement(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueArray Element of a symbol that doesn't exist in the SymbolTable! \n");

    if(Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element){

        return Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element;

    }

    fprintf(stderr, "Symbol Table Error: %s | %d , Tried to get the SymbolValueArray Element of a Symbol but Element is null!\n", identifier, scope);

}*/

int SymbolTableGetSymbolValueArrayLength(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope,  "Trying to get the SymbolValueArray Length of a symbol that doesn't exist in the SymbolTable! \n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Length;

}

SymbolType SymbolTableGetSymbolType(const char* identifier, int scope) {
    
    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the Symbol Type of a symbol that doesn't exist in the SymbolTable! \n");

    return Node->SymbolType;
}

// TO-DO error handling
void SymbolTableSetSymbolAsChildOfParentSymbol(Symbol* node) {

    Symbol* Parent = GlobalSymbolTable;

    Symbol* child = malloc(sizeof(Symbol));

    strcpy(child->SymbolIdentifier, node->SymbolIdentifier);
    child->SymbolType = node->SymbolType;
    child->SymbolNodeType = node->SymbolNodeType;
    child->SymbolValueWrapper.SymbolValueWrapperSymbolValueType = node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType;
    child->SymbolScope = node->SymbolScope;
    child->IsSymbolUsed = 0;

    while (Parent) {

        if(Parent->SymbolType == S_FUNCTION && Parent->SymbolScope == child->SymbolScope) {
            
            if(Parent->SymbolValueWrapper.SymbolValue.SymbolValueSymbol){

                Parent = Parent->SymbolValueWrapper.SymbolValue.SymbolValueSymbol;

                if(Parent->Adjacent){

                    while(Parent->Adjacent){
                        Parent = Parent->Adjacent;
                    }

                }

                Parent->Adjacent = child;

                return;
                
                //Symbol* itr = malloc(sizeof(Symbol));

                //itr = Parent->SymbolValueWrapper.SymbolValue.SymbolValueSymbol;

                //while(itr->Adjacent){
                //    itr = itr->Adjacent;
                //}

                //i//tr->Adjacent = child;

                //free(itr);

                //return;

            } else {

                Parent->SymbolValueWrapper.SymbolValue.SymbolValueSymbol = child;
                return;

            }

        }

        Parent = Parent->Adjacent;
    }

    return;

}

void SymbolTableInsertInto(char identifier[50], SymbolType symbolType, NodeType symbolNodeType, int scope) {

    Symbol* Node = malloc(sizeof(Symbol));

    strcpy(Node->SymbolIdentifier, identifier);
    Node->SymbolType = symbolType;
    Node->SymbolNodeType = symbolNodeType;
    Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType = SymbolTableMatchSymbolValueType(Node);
    Node->SymbolScope = scope;
    Node->IsSymbolUsed = 0;
    Node->Adjacent = GlobalSymbolTable;
    GlobalSymbolTable = Node;

    if(Node->SymbolScope != 0 && Node->SymbolType == S_FUNCTION_PARAMETER){
        SymbolTableSetSymbolAsChildOfParentSymbol(Node);
    }

}

void SymbolTableSetSymbolValueArrayElementType(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set the SymbolValueArray ElementType of a symbol that doesn't exist in the SymbolTable!\n");

    switch(Node->SymbolNodeType) {
        case T_INT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.intElement = (int *) malloc(Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Length * sizeof(int));
        break;
        case T_FLOAT:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.floatElement = (float *) malloc(Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Length * sizeof(float));
        break;
        case T_CHAR:
        Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.charElement = (char *) malloc(Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Length * sizeof(char));
        break;

    }

}

//void SymbolTableSetSymbolValueArrayElement(const char* identifier, int scope, in)

int SymbolTableGetSymbolValueArrayElementInt(const char* identifier, int scope, int index) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueArrayElementInt but Symbol doesn't exist!\n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.intElement[index];

}

void SymbolTableSetSymbolValueArrayElementInt(const char* identifier, int scope, int index, int value) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set the SymbolValueArrayElementInt but Symbol doesn't exist!\n");

    Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.intElement[index] = value;
}

float SymbolTableGetSymbolValueArrayElementFloat(const char* identifier, int scope, int index) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueArrayElementFloat but Symbol doesn't exist!\n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.floatElement[index];
}

void SymbolTableSetSymbolValueArrayElementFloat(const char* identifier, int scope, int index, float value) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set the SymbolValueArrayElementFloat but Symbol doesn't exist!\n");

    Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.floatElement[index] = value;

}

char SymbolTableGetSymbolValueArrayElementChar(const char* identifier, int scope, int index) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueArrayElementChar but Symbol doesn't exist!\n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.charElement[index];
}

void SymbolTableSetSymbolValueArrayElementChar(const char* identifier, int scope, int index, char value) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to set the SymbolValueArrayElementChar but Symbol doesn't exist!\n");

    Node->SymbolValueWrapper.SymbolValue.SymbolValueArray.Element.charElement[index] = value;
}

int SymbolTableGetSymbolScope(const char* identifier) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {
        
        if(strcmp(Node->SymbolIdentifier, identifier) == 0 && Node->SymbolType == S_FUNCTION) {
            return Node->SymbolScope;
        }

        Node = Node->Adjacent;

    }

    fprintf(stderr, "Symbol Table Error: Trying to get the SymbolScope of a Symbol with type FUNCTION that doesn't exist in the SymbolTable!\n");
    exit(EXIT_FAILURE);

}

SymbolValueType SymbolTableGetSymbolValueTypeFromFunctionScope(int scope) {

    Symbol* Node = GlobalSymbolTable;

    while (Node) {

        if(Node->SymbolScope == scope && Node->SymbolType == S_FUNCTION) {
            return Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType;
        }

        Node = Node->Adjacent;

    }

    fprintf(stderr, "Symbol Table Error: Trying to get the SymbolValueType of a Symbol at Scope %d that doesn't exist in the SymbolTable!\n", scope);
    exit(EXIT_FAILURE);
}

int SymbolTableSymbolValueSymbolExists(char* identifier, int scope) {
    
    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to check to see if the SymbolValueSymbol exists of a symbol that doesn't exist in the SymbolTable!\n");

    if(Node->SymbolValueWrapper.SymbolValue.SymbolValueSymbol) {
        return 1;
    }

    return 0;
}

/*char* SymbolTableGetSymbolValueSymbol(char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueSymbol of a symbol that doesn't exist in the SymbolTable!\n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueSymbol->SymbolIdentifier;

}*/

Symbol* SymbolTableGetSymbolValueSymbol(char* identifier, int scope) {
    
    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueSymbol of a symbol that doesn't exist in the SymbolTable!\n");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueSymbol;
}

char* SymbolTableGetSymbolValueAdjacent(char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the adjacent symbol of a symbol that doesn't exist in the SymbolTable!");

    return Node->SymbolValueWrapper.SymbolValue.SymbolValueSymbol->Adjacent->SymbolIdentifier;
}

int SymbolTableSymbolValueAdjacentExists(char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to check to see if the adjacent symbol exists of a symbol that doesn't exist in the SymbolTable!");

    if(Node->SymbolValueWrapper.SymbolValue.SymbolValueSymbol->Adjacent) {
        return 1;
    }

    return 0;
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

    if(Node->SymbolType == S_FUNCTION) {
        return "-1";
    } else {

    char buffer[100];
    
    switch(Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType) {
        case SV_INT:
            snprintf(buffer, sizeof(buffer), "%d", Node->SymbolValueWrapper.SymbolValue.SymbolValueInt);
            return strdup(buffer);
        case SV_FLOAT: 
            snprintf(buffer, sizeof(buffer), "%f", Node->SymbolValueWrapper.SymbolValue.SymbolValueFloat);
            return strdup(buffer);
        case SV_CHAR: 
            return Node->SymbolValueWrapper.SymbolValue.SymbolValueChar;
        default:
            return;
    }

    return strdup(buffer);
    
    }

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

SymbolValueType SymbolTableGetSymbolValueType(const char* identifier, int scope) {

    Symbol* Node = SymbolTableExistsHandler(identifier, scope, "Trying to get the SymbolValueType of a Symbol that doesn't exist in the Symbol Table!");

    return Node->SymbolValueWrapper.SymbolValueWrapperSymbolValueType;
}

void SymbolTablePrint() {


    printf("\nIdentifier SymbolValue SymbolType  SymbolNodeType  Scope\n");

    printf("-                                                       -\n");

    Symbol* Node = GlobalSymbolTable;

    while (Node) {


        printf("%10s %10s %10s %10s %10d\n", Node->SymbolIdentifier, SymbolTableGetValue(Node->SymbolIdentifier, Node->SymbolScope), SymbolTypeToString(Node->SymbolType), nodeTypeToString(Node->SymbolNodeType), Node->SymbolScope);


        Node = Node->Adjacent;

    }

    printf("-                                                       -\n\n");
}

#endif // SYMBOL_TABLE_H
