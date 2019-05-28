#ifndef THECOMPILER_PRINT_IR_H
#define THECOMPILER_PRINT_IR_H

#include <stdio.h>

#include "mcc/ast.h"
#include "mcc/ir.h"

void mcc_print_ir_table(struct mcc_ir_table *table, FILE *out);

#endif // THECOMPILER_PRINT_IR_H
