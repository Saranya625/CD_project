#ifndef IR_H
#define IR_H

#include <stdio.h>

#include "ast.h"

typedef enum {
    IR_OPT_O0 = 0,
    IR_OPT_O1 = 1,
    IR_OPT_O2 = 2
} IROptLevel;

void generate_ir(ASTNode *root, FILE *out);
void generate_ir_level(ASTNode *root, FILE *out, IROptLevel opt_level);

#endif
