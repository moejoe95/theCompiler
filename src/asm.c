#include "mcc/asm.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ir.h"
#include "mcc/print_asm.h"

char *add_asm_float(char *line, struct mcc_asm_head *head);

int find_stack_position(char *arg, struct mcc_asm_stack *stack)
{
	assert(stack);

	struct mcc_asm_stack *current = stack->next_stack;
	while (current != NULL && (current->line_no != NULL && current->var != NULL)) {
		if (strcmp(arg, current->line_no) == 0 || strcmp(arg, current->var) == 0) {
			return current->stack_position;
		}
		current = current->next_stack;
	}
	return -1;
}

void push_on_stack(struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	struct mcc_asm_stack *new_stack_var = malloc(sizeof(*new_stack_var));
	new_stack_var->stack_position = head->offset;
	char value[14] = {0};
	sprintf(value, "(%d)", line->index);
	new_stack_var->line_no = strdup(value);
	new_stack_var->var = strdup(line->arg1);
	new_stack_var->next_stack = NULL;

	struct mcc_asm_stack *current = head->stack;
	while (current->next_stack != NULL) {
		current = current->next_stack;
	}
	current->next_stack = new_stack_var;
}

void update_stack(struct mcc_ir_line *line, struct mcc_asm_stack *stack)
{
	assert(stack);
	assert(line);

	struct mcc_asm_stack *current = stack->next_stack;
	while (current != NULL) {
		if (strcmp(line->arg1, current->var) == 0) {
			if (current->line_no != NULL) {
				free(current->line_no);
			}
			char value[14] = {0};
			sprintf(value, "(%d)", line->index);
			current->line_no = strdup(value);
		}
		current = current->next_stack;
	}
}

char *get_stack_size(struct mcc_ir_line *root)
{
	int memory_size = 0;
	while (root != NULL) {
		memory_size = memory_size + 4 * root->memory_size;
		root = root->next_line;
	}
	char memory_size_str[12] = {0};
	sprintf(memory_size_str, "%d", memory_size);

	return strdup(memory_size_str);
}

char *lookup_data_section(char *arg, struct mcc_asm_head *asm_head)
{

	int is_in_data_section = 0;
	struct mcc_asm_data_section *data_section = asm_head->data_section;
	while (data_section != NULL) {
		if (strcmp(arg, data_section->id) == 0) {
			is_in_data_section = 1;
			break;
		}
		data_section = data_section->next_data_section;
	}
	if (is_in_data_section) {
		return data_section->id;
	}

	char *f_value = add_asm_float(arg, asm_head);
	return f_value;
}

/*
This sequence of instructions is typical at the start of a subroutine to save space on the stack for local variables;
EBP is used as the base register to reference the local variables, and a value is subtracted from ESP to reserve space
on the stack. In this case, eight bytes have been reserved on the stack. Note that pushl automatically decremented ESP
by the appropriate length.
*/
void create_function_label(FILE *out, struct mcc_ir_table *current_func, struct mcc_asm_head *asm_head)
{
	assert(out);

	struct mcc_asm_stack *stack = malloc(sizeof(*stack));
	stack->stack_position = 0;
	stack->next_stack = NULL;
	stack->var = NULL;
	stack->line_no = NULL;

	if (asm_head->stack == NULL) {
		asm_head->stack = stack;
	} else {
		struct mcc_asm_stack *current = asm_head->stack;
		while (current->next_stack != NULL) {
			current = current->next_stack;
		}
		current->next_stack = stack;
	}

	fprintf(out, "\n\t.globl %s\n\n", current_func->func_name);

	fprintf(out, "%s:\n", current_func->func_name);

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, 0, -1, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_ESP, 0, MCC_ASM_REGISTER_EBP, 0);

	char *stack_size = get_stack_size(current_func->line_head->root);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_SUBL, stack_size, MCC_ASM_REGISTER_ESP, 0);
	free(stack_size);
}

void create_asm_jumpfalse(FILE *out,
                          struct mcc_ir_line *line,
                          struct mcc_ir_table *current_func,
                          struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(current_func);
	assert(asm_head);

	if (strncmp(line->arg1, "(", 1) != 0) {
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, 0, MCC_ASM_REGISTER_EAX);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, "0", 0, MCC_ASM_REGISTER_EAX);
		print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JZ, line->arg2);
	} else {
		struct mcc_ir_line *temp_line = current_func->line_head->root;
		char *search_index = strtok(line->arg1, "()");
		while (temp_line->index != atoi(search_index)) {
			temp_line = temp_line->next_line;
		}

		if (temp_line->op_type == MCC_IR_TABLE_BINARY_OP) {
			switch (temp_line->bin_op) {
			case MCC_AST_BINARY_OP_ST:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JGE, line->arg2);
				break;
			case MCC_AST_BINARY_OP_GT:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JLE, line->arg2);
				break;
			case MCC_AST_BINARY_OP_SE:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JG, line->arg2);
				break;
			case MCC_AST_BINARY_OP_GE:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JL, line->arg2);
				break;
			case MCC_AST_BINARY_OP_EQ:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JNE, line->arg2);
				break;
			case MCC_AST_BINARY_OP_NEQ:
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JE, line->arg2);
				break;

			default:
				break;
			}
		}
	}
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

	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);

	if (strcmp(current_func->func_name, "main") != 0) { // main has own return procedure
		char *stack_size = get_stack_size(current_func->line_head->root);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, stack_size, MCC_ASM_REGISTER_ESP, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_POPL, MCC_ASM_REGISTER_EBP, 0, -1, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
		free(stack_size);
	}
}

void create_asm_function_call(FILE *out,
                              struct mcc_ir_line *line,
                              struct mcc_ir_table *current_func,
                              struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(current_func);
	assert(line);

	print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_CALL, line->arg1);

	if (asm_head->current_stack_size_parameters >
	    0) { // Note that after the call returns, the caller cleans up the stack using the add instruction.
		char memory_size_str[12] = {0};
		sprintf(memory_size_str, "%d", asm_head->current_stack_size_parameters);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, memory_size_str, MCC_ASM_REGISTER_ESP, 0);
		asm_head->current_stack_size_parameters = 0;
	}
}

void create_asm_push(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(asm_head);

	int stack_pos = -1;
	stack_pos = find_stack_position(line->arg1, asm_head->stack);

	if (stack_pos == -1) {
		return;
	}

	if (strncmp(line->arg1, "(", 1) == 0) {
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, stack_pos, -1, 0);
	} else {
		if (strncmp(line->arg1, "\"", 1) == 0) {
			char *loc = add_string_to_datasection(NULL, line->arg1, asm_head);
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_PUSHL, loc, -1, 0);
		} else if (line->memory_size == 2) {
			char *f_value = add_asm_float(line->arg1, asm_head);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDL, f_value);
			// TODO replace hardcoded -4 stack location
			print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPL, MCC_ASM_REGISTER_EBP, -4);
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, -4, -1, 0);
		} else {
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_PUSHL, line->arg1, -1, 0);
		}
	}
	asm_head->current_stack_size_parameters +=
	    4 * line->memory_size; // store used stack by parameters for function call clean up
}

void create_asm_binary_op_int(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	int stack_position_arg1 = -1;
	int stack_position_arg2 = -1;
	if (strncmp(line->arg1, "(", 1) == 0)
		stack_position_arg1 = find_stack_position(line->arg1, asm_head->stack);
	if (strncmp(line->arg2, "(", 1) == 0)
		stack_position_arg2 = find_stack_position(line->arg2, asm_head->stack);

	if (stack_position_arg1 != -1) // stack pos not found -> must be literal
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_position_arg1,
		                          MCC_ASM_REGISTER_EAX, 0);
	else
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);

	switch (line->bin_op) {
	case MCC_AST_BINARY_OP_ADD:
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_ADDL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;

	case MCC_AST_BINARY_OP_SUB:
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_SUBL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_SUBL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;

	case MCC_AST_BINARY_OP_MUL:
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MULL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MULL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;

	case MCC_AST_BINARY_OP_DIV:
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, "0", MCC_ASM_REGISTER_EDX, 0);
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_ECX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_DIVL, MCC_ASM_REGISTER_ECX, 0, -1, 0);
		break;

	case MCC_AST_BINARY_OP_LAND:
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_ANDL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ANDL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;

	case MCC_AST_BINARY_OP_LOR:
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_ORL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ORL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;

	default: // all compare bin op (MCC_AST_BINARY_OP_ST, MCC_AST_BINARY_OP_GT, ...)
		if (stack_position_arg2 != -1)
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_CMP, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		else
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_CMP, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		break;
	}

	asm_head->offset = asm_head->offset - 4;
	push_on_stack(line, asm_head);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          asm_head->offset);
}

void create_asm_binary_op_float(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	asm_head->offset = asm_head->offset - 4;

	char *arg1 = lookup_data_section(line->arg1, asm_head);
	char *arg2 = lookup_data_section(line->arg2, asm_head);

	print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDL, arg1);
	print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDL, arg2);

	switch (line->bin_op) {
	case MCC_AST_BINARY_OP_ADD:
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FADDL, NULL);
		break;

	default:
		break;
	}

	print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPL, MCC_ASM_REGISTER_EBP, asm_head->offset);

	print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, asm_head->offset);
}

void create_asm_binary_op(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{

	if (line->memory_size > 1) {
		create_asm_binary_op_float(out, line, asm_head);
	} else {
		create_asm_binary_op_int(out, line, asm_head);
	}
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

char *add_string_to_datasection(char *name, char *value, struct mcc_asm_head *head)
{
	if (name == NULL) {
		char var[64];
		sprintf(var, "tmp_%d", head->temp_variable_id);
		head->temp_variable_id = head->temp_variable_id + 1;
		name = strdup(var);
	}
	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}
	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = name;
	new_data_section->next_data_section = NULL;
	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	struct mcc_asm_data_index *new_data_index = malloc(sizeof(*new_data_index));
	new_data_index->value = strdup(value);
	new_data_index->next_data_index = NULL;

	data_index_root->value = strdup(".string ");
	data_index_root->next_data_index = new_data_index;
	new_data_section->index = data_index_root;

	return name;
}

char *add_asm_float(char *arg, struct mcc_asm_head *head)
{
	char var[64];
	sprintf(var, "tmp_%d", head->temp_variable_id);
	head->temp_variable_id = head->temp_variable_id + 1;
	char *name = strdup(var);

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}

	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = strdup(var);
	new_data_section->next_data_section = NULL;

	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	data_index_root->value = strdup(".float ");

	struct mcc_asm_data_index *data_index_next = malloc(sizeof(*data_index_next));
	data_index_next->value = strdup(arg);
	data_index_next->next_data_index = NULL;

	data_index_root->next_data_index = data_index_next;

	new_data_section->index = data_index_root;

	return name;
}

void create_asm_float(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	assert(out);
	assert(line);
	assert(head);

	char var[64];
	sprintf(var, "%s", line->arg1);

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}

	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = strdup(var);
	new_data_section->next_data_section = NULL;

	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	data_index_root->value = strdup(".float ");

	struct mcc_asm_data_index *data_index_next = malloc(sizeof(*data_index_next));
	data_index_next->value = strdup(line->arg2);
	data_index_next->next_data_index = NULL;

	data_index_root->next_data_index = data_index_next;

	new_data_section->index = data_index_root;
}

void create_asm_assignment(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	int stack_position = -1;
	stack_position = find_stack_position(line->arg1, head->stack);

	if (stack_position == -1) {
		return;
	}

	if (stack_position == -1) {
		head->offset = head->offset - 4;
		stack_position = head->offset;
		push_on_stack(line, head);
	} else {
		update_stack(line, head->stack);
	}

	if (line->memory_size == 2) {
		create_asm_float(out, line, head);
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDL, line->arg1);
		int pos = find_stack_position(line->arg1, head->stack);
		print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPL, MCC_ASM_REGISTER_EBP, pos);
	} else {
		if (strncmp(line->arg2, "(", 1) == 0) {
			int pos = find_stack_position(line->arg2, head->stack);
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, pos,
			                          MCC_ASM_REGISTER_EAX, 0);
		} else {
			if (strncmp(line->arg2, "\"", 1) == 0) {
				add_string_to_datasection(line->arg1, line->arg2, head);
				print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1,
				                          MCC_ASM_REGISTER_EAX, 0);
			} else {
				print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2,
				                          MCC_ASM_REGISTER_EAX, 0);
			}
		}
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
		                          stack_position);
	}
}

void create_asm_array(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	assert(out);
	assert(line);
	assert(head);

	char var[64];
	sprintf(var, "%s", line->arg1);

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}
	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = strdup(var);
	new_data_section->next_data_section = NULL;

	struct mcc_asm_data_index *new_data_index_current = malloc(sizeof(*new_data_index_current));
	struct mcc_asm_data_index *new_data_index_root = new_data_index_current;
	char *ptr;
	int len = strtol(line->arg2, &ptr, 10);
	for (int i = 0; i < len; i++) {
		struct mcc_asm_data_index *new_data_index_tmp = malloc(sizeof(*new_data_index_tmp));
		new_data_index_tmp->value = strdup("0");
		new_data_index_current->next_data_index = new_data_index_tmp;
		new_data_index_current = new_data_index_tmp;
	}
	new_data_index_current->next_data_index = NULL;

	new_data_index_root->value = strdup(".long ");
	new_data_section->index = new_data_index_root;
	current->next_data_section = new_data_section;
}

void create_asm_store(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	assert(out);
	assert(line);
	assert(head);

	char *load = strdup(line->arg1);
	char *id = strtok(load, "[");
	char *access_position = strtok(NULL, "]");
	char *ptr;
	int access_position_int = strtol(access_position, &ptr, 10);

	struct mcc_asm_data_section *data = head->data_section;
	while (data != NULL) {
		if (strcmp(data->id, id) == 0) {
			struct mcc_asm_data_index *index = data->index;
			for (int i = 0; i <= access_position_int; i++) {
				index = index->next_data_index;
			}
			index->value = strdup(line->arg2);
		}
		data = data->next_data_section;
	}

	free(load);

	create_asm_assignment(out, line, head);
}

void create_asm_line(FILE *out,
                     struct mcc_ir_line *line,
                     struct mcc_asm_head *asm_head,
                     struct mcc_ir_table *current_func)
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
	case MCC_IR_TABLE_ARRAY:
		create_asm_array(out, line, asm_head);
		break;
	case MCC_IR_TABLE_STORE:
		create_asm_store(out, line, asm_head);
		break;
	case MCC_IR_TABLE_RETURN:
		create_asm_return(out, line, current_func);
		break;
	case MCC_IR_TABLE_PUSH:
		create_asm_push(out, line, asm_head);
		break;
	case MCC_IR_TABLE_CALL:
		create_asm_function_call(out, line, current_func, asm_head);
		break;
	case MCC_IR_TABLE_JUMPFALSE:
		create_asm_jumpfalse(out, line, current_func, asm_head);
		break;
	case MCC_IR_TABLE_JUMP:
		print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JMP, line->arg1);
		break;
	case MCC_IR_TABLE_BR_LABEL:
		fprintf(out, "%s:\n", line->arg1);
		break;
	default:
		break;
	}
}

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int destination)
{
	assert(ir);

	// 0 = console only
	// 1 = file only
	// 2 = console and file
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
	asm_head->current_stack_size_parameters = 0;
	asm_head->temp_variable_id = 0;
	asm_head->stack = NULL;

	struct mcc_asm_data_section *data_root = malloc(sizeof(*data_root));
	data_root->id = strdup("\n.data\n");
	data_root->index = NULL;
	data_root->next_data_section = NULL;

	asm_head->data_section = data_root;

	struct mcc_ir_table *current_func = ir->root;
	while (current_func != NULL) {
		struct mcc_ir_line *current_line = current_func->line_head->root;
		create_function_label(tmpfile, current_func, asm_head);
		while (current_line != NULL) {
			create_asm_line(tmpfile, current_line, asm_head, current_func);
			current_line = current_line->next_line;
		}
		if (strcmp(current_func->func_name, "main") == 0) {
			/*
			leave frees the space saved on the stack by copying EBP into ESP, then popping the saved value
			of EBP back to EBP.
			equivalent to:
			        mov   %ebp, %esp
			        pop   %ebp
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

	print_asm_data_section(tmpfile, data_root);

	if (destination != 0) {
		fclose(tmpfile);
		if (destination >= 2) {
			tmpfile = fopen("asm_tmp.s", "r");
			int c;
			c = fgetc(tmpfile);
			while (c != EOF) {
				printf("%c", c);
				c = fgetc(tmpfile);
			}
			fclose(tmpfile);
		}
	}

	mcc_delete_asm(asm_head);
}

void mcc_delete_data_index(struct mcc_asm_data_index *index)
{
	if (index->next_data_index != NULL) {
		mcc_delete_data_index(index->next_data_index);
	}

	free(index->value);
	free(index);
}

void mcc_delete_stack(struct mcc_asm_stack *stack)
{
	if (stack->next_stack != NULL) {
		mcc_delete_stack(stack->next_stack);
	}
	free(stack->line_no);
	free(stack->var);
	free(stack);
}

void mcc_delete_asm(struct mcc_asm_head *asm_head)
{
	struct mcc_asm_data_section *data = asm_head->data_section;
	while (data != NULL) {

		struct mcc_asm_data_section *temp = data->next_data_section;
		if (data->index != NULL) {
			mcc_delete_data_index(data->index);
		}
		free(data->id);
		free(data);
		data = temp;
	}
	if (asm_head->stack != NULL) {
		mcc_delete_stack(asm_head->stack);
	}
	free(asm_head);
}
