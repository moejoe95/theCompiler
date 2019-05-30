#ifndef THECOMPILER_PRINT_IR_H
#define THECOMPILER_PRINT_IR_H

#include "mcc/ast.h"
#include "mcc/ir.h"
#include <stdio.h>

void mcc_print_ir_table(struct mcc_ir_line *table, char *name, FILE *out);

#endif // THECOMPILER_PRINT_IR_H
