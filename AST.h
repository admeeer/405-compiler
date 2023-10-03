// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#include <string.h>

typedef enum {
    ASSIGNMENT,
    TYPE,
    RETURN,
    WRITE,
    WRITELN,
    EQUALS,
    INT,
    IDENTIFIER,
    FLOAT,
    CHARACTER
} NodeType;

const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case TYPE: return "TYPE";
        case ASSIGNMENT: return "ASSIGNMENT";
        case RETURN: return "RETURN";
        case WRITE: return "WRITE";
        case WRITELN: return "WRITELN";
        case EQUALS: return "EQUALS";
        case INT: return "INT";
        case IDENTIFIER: return "IDENTIFIER";
        case FLOAT: return "FLOAT";
        case CHARACTER: return "CHARACTER";
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
    printf("%s\n", tree->nodeType);
    printDots(level);
    printf("%s %s\n", tree->LHS, tree->RHS);
    if(tree->right != NULL) printAST(tree->right, level+1); else return;
    if(tree->left != NULL) printAST(tree->left, level+1); else return;
    
}

void printNode(struct AST* node) {

    printf("Current node ---->\n");
    printDots(3);
    printf("%s\n", node->nodeType);
    printDots(3);
    printf("%s %s\n\n", node->LHS, node->RHS);

}