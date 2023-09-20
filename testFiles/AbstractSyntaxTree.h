#ifndef AST_H
#define AST_H

typedef enum {
    NODE_TYPE_NUMBER,         // Represents a number
    NODE_TYPE_ID,             // Represents an identifier
    NODE_TYPE_DECLARATION,    // Represents a declaration (e.g., int x;)
    NODE_TYPE_ASSIGNMENT     // Represents an assignment (e.g., x = 5;)
} NodeType;

typedef struct ASTNode {
    NodeType type;

    union {
        int number;            // if the node is a number
        char* identifier;      // if the node is an identifier

        struct {               // if the node is a declaration
            char* type;
            char* identifier;
        } declaration;

        struct {               // if the node is an assignment
            char* identifier;
            struct ASTNode* expression;
        } assignment;
    };

} ASTNode;

#endif // AST_H
