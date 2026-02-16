#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    char *type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;
} ASTNode;

ASTNode* createNode(char *type, char *value,
                    ASTNode *left,
                    ASTNode *right,
                    ASTNode *third);
void printAST(ASTNode* node, int level);

#endif
