#ifndef IR_H
#define IR_H

#include <stdio.h>

#include "ast.h"

void generate_ir(ASTNode *root, FILE *out);

#endif
