#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

int semantic_analysis(ASTNode *root_node);
int semantic_error_count(void);

#endif
