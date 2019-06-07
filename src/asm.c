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
void create_function_label(FILE *out, struct mcc_ir_table *current_func)
{
	assert(out);
	fprintf(out, ".globl %s\n\n", current_func->func_name);
	fprintf(out, "%s:\n", current_func->func_name);
	int memory_size = 0;
	struct mcc_ir_line *current_line = current_func->line_head->root;
	while (current_line != NULL) {
		memory_size = memory_size + 4 * current_line->memory_size;
		current_line = current_line->next_line;
	}
	char memory_size_str[12] = {0};
	sprintf(memory_size_str, "%d", memory_size);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, 0, -1, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_ESP, 0, MCC_ASM_REGISTER_EBP, 0);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_SUBL, memory_size_str, MCC_ASM_REGISTER_ESP, 0);
}

/*
This line stores zero (return value) in EAX. The C calling convention is to store return values in EAX when exiting
a routine.
*/
void create_asm_return(FILE *out, struct mcc_ir_line *line, struct mcc_ir_table *current_func)
{
	assert(out);
	assert(line);
	assert(current_func);

	if(strcmp(current_func->func_name, "main") == 0){ //main has own return procedure
		return;
	}

	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
}

void create_asm_function_call(FILE *out, struct mcc_ir_line *line, struct mcc_ir_table *current_func)
{
	assert(out);
	assert(line);

	int used_stack_size = 0;
	struct mcc_ir_line *current_line = current_func->line_head->root;
	while (current_line != NULL && current_line->op_type == MCC_IR_TABLE_PUSH) {
		if(current_line->op_type == MCC_IR_TABLE_CALL && strcmp(current_line->arg1, line->arg1) == 0){
			break;
		}
		used_stack_size = used_stack_size + 4 * current_line->memory_size;
		current_line = current_line->next_line;
	}
	char memory_size_str[12] = {0};
	sprintf(memory_size_str, "%d", used_stack_size);

	print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_CALL, line->arg1);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, memory_size_str, MCC_ASM_REGISTER_ESP, 0); //TODO wrong memory?
}

void create_asm_push(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(asm_head);

	if (strncmp(line->arg1, "(", 1) == 0)
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, asm_head->offset, -1,
		                          0);
	else
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_PUSHL, line->arg1, -1, 0);
}

void create_asm_binary_op(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	asm_head->offset = asm_head->offset - 4;
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	switch (line->bin_op) {
	case MCC_AST_BINARY_OP_ADD:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	case MCC_AST_BINARY_OP_SUB:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_SUBL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	case MCC_AST_BINARY_OP_MUL:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MULL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	case MCC_AST_BINARY_OP_DIV:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, "0", MCC_ASM_REGISTER_EDX, 0);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_ECX, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_DIVL, MCC_ASM_REGISTER_ECX, 0, -1, 0);
		break;
	case MCC_AST_BINARY_OP_LAND:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ANDL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	case MCC_AST_BINARY_OP_LOR:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ORL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	default:
		break;
	}
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          asm_head->offset);
}

void create_asm_unary(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	head->offset = head->offset - 4;
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	switch (line->un_op) {
	case MCC_AST_UNARY_OP_NOT:
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_NOTL, MCC_ASM_REGISTER_EAX, 0, -1, 0);
		break;
	default:
		break;
	}
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          head->offset);
}

void create_asm_assignment(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	head->offset = head->offset - 4;
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          head->offset);
}

void create_asm_line(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head, struct mcc_ir_table *current_func)
{
	assert(out);
	assert(line);

	switch (line->op_type) {
	case MCC_IR_TABLE_BINARY_OP:
		create_asm_binary_op(out, line, asm_head);
		break;
	case MCC_IR_TABLE_UNARY_OP:
		create_asm_unary(out, line, asm_head);
		break;
	case MCC_IR_TABLE_ASSIGNMENT:
		create_asm_assignment(out, line, asm_head);
		break;
	case MCC_IR_TABLE_RETURN:
		create_asm_return(out, line, current_func);
		break;
	case MCC_IR_TABLE_PUSH:
		create_asm_push(out, line, asm_head);
		break;
	case MCC_IR_TABLE_CALL:
		create_asm_function_call(out, line, current_func);
		break;
	default:
		break;
	}
}

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int destination)
{
	assert(ir);

	FILE *tmpfile;
	if (destination == 0) {
		tmpfile = out;
	} else {
		tmpfile = open_tmp_file();
		if (tmpfile == NULL) {
			return;
		}
	}

	create_asm_header(tmpfile);

	struct mcc_asm_head *asm_head = malloc(sizeof(*asm_head));
	asm_head->offset = 0;

	struct mcc_ir_table *current_func = ir->root;
	while (current_func != NULL) {
		struct mcc_ir_line *current_line = current_func->line_head->root;
		create_function_label(tmpfile, current_func);
		while (current_line != NULL) {
			create_asm_line(tmpfile, current_line, asm_head, current_func);
			current_line = current_line->next_line;
		}
		if (strcmp(current_func->func_name, "main") == 0) {
			/*
			leave frees the space saved on the stack by copying EBP into ESP, then popping the saved value
			of EBP back to EBP.
			*/
			print_asm_instruction_reg(tmpfile, MCC_ASM_INSTRUCTION_LEAVE, -1, 0, -1, 0);
			/*
			This line returns control to the calling procedure by popping the saved instruction pointer
			from the stack.
			*/
			print_asm_instruction_reg(tmpfile, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
		}
		current_func = current_func->next_table;
	}

	fclose(tmpfile);
}
