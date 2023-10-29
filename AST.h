// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef AST_H
#define AST_H

#include <string.h>

/*
We want to capture what type of AST node we are creating so we can later reference it easily.
Each AST node should contain a value, ASTValue, which is a struct based on rules (NodeTypes)
*/
typedef enum {
    T_ASSIGNMENT,
    T_TYPE,
    T_RETURN,
    T_WRITE,
    T_WRITELN,
    T_EQUALS,
    T_INT,
    T_IDENTIFIER,
    T_FLOAT,
    T_CHAR,
    T_FUNCTION,
    T_FUNCTIONCALL
} NodeType;

const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case T_ASSIGNMENT: return "ASSIGNMENT";
        case T_TYPE: return "TYPE";
        case T_RETURN: return "RETURN";
        case T_WRITE: return "WRITE";
        case T_WRITELN: return "WRITELN";
        case T_EQUALS: return "EQUALS";
        case T_INT: return "INT";
        case T_IDENTIFIER: return "IDENTIFIER";
        case T_FLOAT: return "FLOAT";
        case T_CHAR: return "CHARACTER";
        case T_FUNCTION: return "FUNCTION";
        case T_FUNCTIONCALL: return "FUNCTIONCALL";
        default: return "UNDEFINED";
    }
}

struct AST{

    NodeType nodeType;

    char *LHS;
    char *RHS;
    
    struct AST* left;
    struct AST* right;

};

struct AST* insertIntoAST(NodeType nodeType, const char *LHS, const char *RHS) {

    struct AST* node = malloc(sizeof(struct AST));
    node->nodeType = nodeType;
    node->LHS = strdup(LHS);
    node->RHS = strdup(RHS);
    //printf("Node with type %s and LHS %s and RHS %s inserted into tree\n", nodeTypeToString(node->nodeType), node->LHS, node->RHS);

    return node;
}

void printDots(int num)
{
    for (int i = 0; i < num; i++)
        printf("      ");
}

void printAST(struct AST* tree, int level){
    if (tree == NULL) return;
    printDots(level);
    printf("%s\n", nodeTypeToString(tree->nodeType));
    printDots(level);
    printf("%s %s\n", tree->LHS, tree->RHS);
    if(tree->left) {
        //printf("AST test, tree is currently %s and %s, left is %s and %s\n", tree->LHS, tree->RHS, tree->left->LHS, tree->left->RHS);
    }
    if(tree->right) {
        // printf("AST test, tree is currently %s and %s, and right is %s and %s\n", tree->LHS, tree->RHS,tree->right->LHS, tree->right->RHS);
    }
    if(tree->right != NULL) printAST(tree->right, level+1); else return;
    if(tree->left != NULL) printAST(tree->left, level+1); else return;
    
}

void printNode(struct AST* node) {

    printf("Current node ---->\n");
    printDots(3);
    printf("%s\n", nodeTypeToString(node->nodeType));
    printDots(3);
    printf("%s %s\n\n", node->LHS, node->RHS);

}

#endif // AST_H
