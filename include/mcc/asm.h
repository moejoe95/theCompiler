#ifndef THECOMPILER_ASM_H
#define THECOMPILER_ASM_H

#include <stdio.h>
#include <string.h>

#include "mcc/ir.h"

enum asm_table_operation_type { START };

struct mcc_asm_table {
	struct mcc_asm_line *current;
	struct mcc_asm_line *root;
};

struct mcc_asm_line {
	enum asm_table_operation_type op_type;
	int index;

	struct mcc_asm_line *next_line;
};

struct mcc_asm_table *mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int log_level);
void mcc_delete_asm(struct mcc_asm_table *table);

#endif