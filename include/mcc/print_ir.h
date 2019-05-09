#ifndef THECOMPILER_PRINT_IR_H
#define THECOMPILER_PRINT_IR_H

#include <stdio.h>
#include "mcc/ir.h"
#include "mcc/ast.h"

void mcc_print_ir_table(struct mcc_ir_table *table, FILE *out);

#endif //THECOMPILER_PRINT_IR_H
