//Abstract Syntax Tree Implementation
#include <string.h>

struct AST{
    char nodeType[50];
    char LHS[50];
    char RHS[50];
    
    struct AST * left;
    struct AST * right;

    // review pointers to structs in C
    // complete the tree struct with pointers
};



struct AST * AST_assignment(char nodeType[50], char LHS[50], char RHS[50]){
    

    struct AST* ASTassign = malloc(sizeof(struct AST));
    strcpy(ASTassign->nodeType, nodeType);
    strcpy(ASTassign->LHS, LHS);
    strcpy(ASTassign->RHS, RHS);

    // if (!ASTassign->right) {
    //     printf("right is null: %s\n", ASTassign->right);
    // }

    

/*
       =
     /   \
    x     y

*/
    return ASTassign;
    
}
struct AST * AST_BinaryExpression(char nodeType[50], char LHS[50], char RHS[50]){

    struct AST* ASTBinExp = malloc(sizeof(struct AST));
    strcpy(ASTBinExp->nodeType, nodeType);
    strcpy(ASTBinExp->LHS, LHS);
    strcpy(ASTBinExp->RHS, RHS);
    return ASTBinExp;
    
}
struct AST * AST_Type(char nodeType[50], char LHS[50], char RHS[50]){

    struct AST* ASTtype = malloc(sizeof(struct AST));
    strcpy(ASTtype->nodeType, nodeType);
    strcpy(ASTtype->LHS, LHS);
    strcpy(ASTtype->RHS, RHS);
        
    return ASTtype;
    
}

struct AST * AST_Func(char nodeType[50], char LHS[50], char RHS[50]){
    
    struct AST* ASTtype = malloc(sizeof(struct AST));
    strcpy(ASTtype->nodeType, nodeType);
    strcpy(ASTtype->LHS, LHS);
    strcpy(ASTtype->RHS, RHS);
        
    return ASTtype;
    
}

struct AST * AST_Write(char nodeType[50], char LHS[50], char RHS[50]){
    
    struct AST* ASTtype = malloc(sizeof(struct AST));
    strcpy(ASTtype->nodeType, nodeType);
    strcpy(ASTtype->LHS, LHS);
    strcpy(ASTtype->LHS, RHS);
        
    return ASTtype;
    
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