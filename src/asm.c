#include "mcc/asm.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ir.h"

void create_function_label(FILE *out, char *func_name)
{
	assert(out);
	fprintf(out, "%s:\n", func_name);
}

void print_asm_instruction(FILE *out, char *op, char *arg1, char *arg2)
{
	assert(op);
	if (arg2 == NULL)
		fprintf(out, "\t%s\t%s\n", op, arg1);
	else if (arg1 == NULL)
		fprintf(out, "\t%s\n", op);
	else
		fprintf(out, "\t%s\t%s, %s\n", op, arg1, arg2);
}

void create_asm_return(FILE *out, struct mcc_ir_line *line)
{
	assert(out);
	assert(line);
	print_asm_instruction(out, "movl", "%eax", "-0(%ebp)");
}

void create_asm_line(FILE *out, struct mcc_ir_line *line)
{
	assert(out);
	assert(line);

	switch (line->op_type) {
	case MCC_IR_TABLE_RETURN:
		create_asm_return(out, line);
		break;
	default:
		break;
	}
}

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out)
{
	assert(ir);

	struct mcc_ir_table *current_func = ir->root;
	while (current_func != NULL) {
		struct mcc_ir_line *current_line = current_func->line_head->root;
		create_function_label(out, current_func->func_name);
		while (current_line != NULL) {
			create_asm_line(out, current_line);
			current_line = current_line->next_line;
		}
		current_func = current_func->next_table;
	}

	/* compile assembly with gcc -c file.S -o file.o */
}