#include "mcc/asm.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ir.h"
#include "mcc/print_asm.h"

/*
This sequence of instructions is typical at the start of a subroutine to save space on the stack for local variables;
EBP is used as the base register to reference the local variables, and a value is subtracted from ESP to reserve space
on the stack. In this case, eight bytes have been reserved on the stack. Note that pushl automatically decremented ESP
by the appropriate length.
*/
void create_function_label(FILE *out, char *func_name)
{
	assert(out);
	fprintf(out, "\t.globl %s\n\n", func_name);
	fprintf(out, "%s:\n", func_name);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, 0, -1, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_ESP, 0, MCC_ASM_REGISTER_EBP, 0);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_SUBL, 8, MCC_ASM_REGISTER_ESP, 0);
}

/*
This line stores zero (return value) in EAX. The C calling convention is to store return values in EAX when exiting
a routine.
*/
void create_asm_return(FILE *out, struct mcc_ir_line *line)
{
	assert(out);
	assert(line);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, 0, MCC_ASM_REGISTER_EAX, 0);
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

	create_asm_header(out);

	struct mcc_ir_table *current_func = ir->root;
	while (current_func != NULL) {
		struct mcc_ir_line *current_line = current_func->line_head->root;
		create_function_label(out, current_func->func_name);
		while (current_line != NULL) {
			create_asm_line(out, current_line);
			current_line = current_line->next_line;
		}
		if (strcmp(current_func->func_name, "main") == 0) {
			/*
			leave frees the space saved on the stack by copying EBP into ESP, then popping the saved value
			of EBP back to EBP.
			*/
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_LEAVE, -1, 0, -1, 0);
			/*
			This line returns control to the calling procedure by popping the saved instruction pointer
			from the stack.
			*/
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
		}
		current_func = current_func->next_table;
	}

	/* compile assembly with
	        gcc -c file.s -o file.o
	        gcc -o file file.o
	*/
}