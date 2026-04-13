#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <stdio.h>

int semantic_analysis(ASTNode *root_node);
int semantic_error_count(void);
void semantic_set_symbol_output(FILE *out);

#endif
