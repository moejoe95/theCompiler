#ifndef THECOMPILER_PRINT_IR_H
#define THECOMPILER_PRINT_IR_H

#include <stdio.h>

#include "mcc/ast.h"

enum ir_table_operation_type {
    MCC_IR_TABLE_UNARY_OP,
    MCC_IR_TABLE_BINARY_OP,
    MCC_IR_TABLE_ASSIGNMENT,
    MCC_IR_TABLE_JUMPFALSE
};

void print_table_legend();

void print_table(enum ir_table_operation_type type, int index, char *arg1, char *arg2, struct mcc_ast_expression *expression);

#endif //THECOMPILER_PRINT_IR_H
