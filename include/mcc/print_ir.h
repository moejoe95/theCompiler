#ifndef THECOMPILER_PRINT_IR_H
#define THECOMPILER_PRINT_IR_H

#include <stdio.h>
#include "mcc/ir.h"
#include "mcc/ast.h"


void print_table_legend();

void print_table(struct mcc_ir_table *table, int index, char *arg1, char *arg2);

#endif //THECOMPILER_PRINT_IR_H
