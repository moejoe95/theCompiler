#include "mcc/asm.h"

#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ir.h"
#include "mcc/print_asm.h"

char *add_asm_float(char *arg, int line_no, struct mcc_asm_head *head);
char *add_string_to_datasection(char *name, char *value, struct mcc_asm_head *head);

int find_stack_position(char *arg, struct mcc_asm_head *head)
{
	assert(head);

	struct mcc_asm_stack *current = head->stack->next_stack;
	while (current != NULL) {
		if (current->line_no != NULL && current->var != NULL) {
			if (strcmp(arg, current->line_no) == 0 || strcmp(arg, current->var) == 0) {
				return current->stack_position;
			}
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

void push_on_stack_with_offset(struct mcc_ir_line *line, struct mcc_asm_head *head, int offset)
{
	struct mcc_asm_stack *new_stack_var = malloc(sizeof(*new_stack_var));
	new_stack_var->stack_position = offset;
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
		if (current->var != NULL && strcmp(line->arg1, current->var) == 0) {
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

char *get_id_by_line_ref(char *arg, struct mcc_asm_head *asm_head, int arg_number)
{
	if (arg[0] == '(') { // search for line number and return literal value
		struct mcc_ir_line *current_line = asm_head->ir->line_head->root;
		while (current_line != NULL) {
			char value[64] = {0};
			sprintf(value, "(%d)", current_line->index);
			if (strcmp(arg, value) == 0) {
				if (arg_number == 1)
					return current_line->arg1;
				else if (current_line->op_type == MCC_IR_TABLE_POP)
					return current_line->arg2;
			}
			current_line = current_line->next_line;
		}
	}
	if (arg_number == 1)
		return arg;
	else
		return "-";
}

int is_number(char *str)
{
	for (size_t i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-') {
			return 0;
		}
	}
	return 1;
}

char *lookup_data_section_array(FILE *out, char *arg, struct mcc_asm_head *asm_head, int factor)
{
	arg = get_id_by_line_ref(arg, asm_head, 1);
	char *load = strdup(arg);
	char *id = strtok(load, "[");
	char *access_position = strtok(NULL, "]");
	char index[64] = {0};
	struct mcc_asm_data_section *data_section = asm_head->data_section;
	while (data_section != NULL) {
		if (data_section->id != NULL && strcmp(id, data_section->id) == 0) {
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, id, MCC_ASM_REGISTER_EDX, 0);
			if (!is_number(access_position)) { // access with expression
				int stack_pos = find_stack_position(access_position, asm_head);
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
				                          stack_pos, MCC_ASM_REGISTER_ECX, 0);
			} else {
				print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, access_position,
				                          MCC_ASM_REGISTER_ECX, 0);
			}
			break;
		}
		data_section = data_section->next_data_section;

		if (data_section == NULL) {
			int stack_pos_arr = find_stack_position(id, asm_head);

			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_pos_arr,
			                          MCC_ASM_REGISTER_EDX, 0);

			if (!is_number(access_position)) { // access with expression
				int stack_pos = find_stack_position(access_position, asm_head);
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
				                          stack_pos, MCC_ASM_REGISTER_ECX, 0);
			} else
				print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, access_position,
				                          MCC_ASM_REGISTER_ECX, 0);
		}
	}
	sprintf(index, "(%%edx, %%ecx, %d)", factor);

	free(load);
	return strdup(index);
}

char *lookup_data_section(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head, int arg_number)
{
	char *arg = NULL;
	if (arg_number == 1) {
		arg = line->arg1;
	} else {
		arg = line->arg2;
	}
	int number = is_number(arg);
	if (number) { // return if arg is a literal
		return NULL;
	}

	if (strchr(arg, '[')) {
		return lookup_data_section_array(out, arg, asm_head, 4);
	}

	if (strchr(arg, '(')) {
		char *l = get_id_by_line_ref(arg, asm_head, 1);
		if (strchr(l, '['))
			return lookup_data_section_array(out, l, asm_head, 4);
	}

	char val[64] = {0};

	int stack_pos = find_stack_position(arg, asm_head);
	if (stack_pos != -1) {
		sprintf(val, "%d(%%ebp)", stack_pos);
		return strdup(val);
	}

	int data_section_label_count = 0;
	struct mcc_asm_data_section *data_section = asm_head->data_section;
	while (data_section != NULL) {
		if ((data_section->id != NULL && strcmp(arg, data_section->id) == 0)) {
			data_section_label_count = data_section->label_count;
		} else if (data_section->line_no != NULL && strcmp(arg, data_section->line_no) == 0) {
			if (data_section->func_name == asm_head->ir->func_name) {
				data_section_label_count = data_section->label_count;
				arg = data_section->id;
			}
		}
		data_section = data_section->next_data_section;
	}

	if (strncmp(arg, "tmp_", 4) == 0) {
		return strdup(arg);
	}
	sprintf(val, "%s_%d", arg, data_section_label_count);
	return strdup(val);
}

void update_data_section_line_number(char *old, char *new, struct mcc_asm_head *asm_head)
{
	struct mcc_asm_data_section *data_section = asm_head->data_section;
	while (data_section != NULL) {
		if (data_section->line_no != NULL && strcmp(old, data_section->line_no) == 0) {
			free(data_section->line_no);
			data_section->line_no = strdup(new);
		}
		data_section = data_section->next_data_section;
	}
}

int is_reference_assignment(char *arg, struct mcc_asm_head *asm_head)
{
	if (arg[0] != '(')
		return 1;
	struct mcc_ir_line *current_line = asm_head->ir->line_head->root;
	while (current_line != NULL) {
		char value[64] = {0};
		sprintf(value, "(%d)", current_line->index);
		if (strcmp(arg, value) == 0) {
			if (current_line->op_type == MCC_IR_TABLE_ASSIGNMENT ||
			    current_line->op_type == MCC_IR_TABLE_UNARY_OP)
				return 1;
			else
				return 0;
		}
		current_line = current_line->next_line;
	}
	return 0;
}

void print_asm_instruction(FILE *out, enum mcc_asm_instruction instruction, int stack_position_arg2, char *arg2)
{
	if (stack_position_arg2 != -1) {
		print_asm_instruction_reg(out, instruction, MCC_ASM_REGISTER_EBP, stack_position_arg2,
		                          MCC_ASM_REGISTER_EAX, 0);
	} else {
		print_asm_instruction_lit(out, instruction, arg2, MCC_ASM_REGISTER_EAX, 0);
	}
}

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
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_CMP, "0", MCC_ASM_REGISTER_EAX, 0);
		if (strchr(line->arg1, '[') || strchr(get_id_by_line_ref(line->arg1, asm_head, 1), '[')) {
			char *index = lookup_data_section_array(out, line->arg1, asm_head, 4);
			print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, index, MCC_ASM_REGISTER_EAX);
		} else {
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, 0, MCC_ASM_REGISTER_EAX);
		}

		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, "0", 0, MCC_ASM_REGISTER_EAX);
		print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JZ, line->arg2);
	} else {
		struct mcc_ir_line *temp_line = current_func->line_head->root;
		char *search_index = strtok(line->arg1, "()");
		while (temp_line->index != atoi(search_index)) {
			temp_line = temp_line->next_line;
		}
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_CMP, "0", MCC_ASM_REGISTER_EAX, 0);
		print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_JE, line->arg2);
	}
}

void create_asm_return(FILE *out,
                       struct mcc_ir_line *line,
                       struct mcc_ir_table *current_func,
                       struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(current_func);

	int stack_pos = find_stack_position(line->arg1, asm_head);
	char *arr_size = get_id_by_line_ref(line->arg1, asm_head, 2);

	if (arr_size != NULL && strcmp(arr_size, "-") != 0 && arr_size[0] != '(') {
		char *arr_id = get_id_by_line_ref(line->arg1, asm_head, 1);
		char arr[124] = {0};
		sprintf(arr, "%s[%s]", arr_id, line->arg2);
		char *addr = lookup_data_section_array(out, arr, asm_head, 4);

		print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, addr, MCC_ASM_REGISTER_EAX);
	} else if (line->arg1[0] == '\"') {
		char *label = add_string_to_datasection(NULL, strdup(line->arg1), asm_head);
		print_asm_instruction(out, MCC_ASM_INSTRUCTION_MOVL, stack_pos, label);
	} else if (line->memory_size == 2 && stack_pos == -1) {
		char *tmp = add_asm_float(line->arg1, line->index, asm_head);
		print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, tmp, MCC_ASM_REGISTER_EAX);
	} else if (line->arg1[0] != '(' && line->arg1[0] != '-')
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	else if (stack_pos != -1) {
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_pos,
		                          MCC_ASM_REGISTER_EAX, 0);
	} else {
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, asm_head->offset,
		                          MCC_ASM_REGISTER_EAX, 0);
	}

	if (strcmp(current_func->func_name, "main") != 0) { // non main functions
		char *stack_size = get_stack_size(current_func->line_head->root);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ADDL, stack_size, MCC_ASM_REGISTER_ESP, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_POPL, MCC_ASM_REGISTER_EBP, 0, -1, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
		free(stack_size);
	} else { // main function
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_LEAVE, -1, 0, -1, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_RETL, -1, 0, -1, 0);
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

	if (line->memory_size == 1) {
		asm_head->offset = asm_head->offset - (4 * line->memory_size);

		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
		                          asm_head->offset);
	} else if (line->memory_size == 2) {

		asm_head->offset = asm_head->offset - (4 * line->memory_size);

		if (strcmp(line->arg1, "read_float") == 0) {
			print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
			                                  asm_head->offset);
		} else {
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0,
			                          MCC_ASM_REGISTER_EBP, asm_head->offset);
		}

		push_on_stack(line, asm_head);
	}

	push_on_stack(line, asm_head);
}

void create_asm_push(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(asm_head);

	int stack_pos = -1;
	stack_pos = find_stack_position(line->arg1, asm_head);

	if (line->arg1[0] != '\"' &&
	    (strchr(line->arg1, '[') || strchr(get_id_by_line_ref(line->arg1, asm_head, 1), '['))) { // array
		char *index = lookup_data_section_array(out, line->arg1, asm_head, 4);
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_PUSHL, index);
		free(index);
		asm_head->current_stack_size_parameters += 4 * 1;
		return;
	}

	if (line->memory_size == 2) { // floats
		char *loc = lookup_data_section(out, line, asm_head, 1);
		if (loc == NULL) {
			loc = add_asm_float(line->arg1, line->index, asm_head);
		}

		asm_head->offset = asm_head->offset - 4;

		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, loc);
		print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
		                                  asm_head->offset);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, asm_head->offset, -1,
		                          0);
		free(loc);
	} else if (strncmp(line->arg1, "(", 1) == 0) {
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP, stack_pos, -1, 0);
	} else {
		if (strncmp(line->arg1, "\"", 1) == 0) { // strings
			char *loc = add_string_to_datasection(NULL, strdup(line->arg1), asm_head);
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_PUSHL, loc, -1, 0);
		} else { // literals
			int stack_pos_arr = find_stack_position(line->arg1, asm_head);

			if (stack_pos_arr != -1) { // array name
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_PUSHL, MCC_ASM_REGISTER_EBP,
				                          stack_pos_arr, -1, 0);
			} else {
				char arg[65];
				sprintf(arg, "$%s", line->arg1);
				print_asm_instruction_call(out, MCC_ASM_INSTRUCTION_PUSHL, arg);
			}
		}
	}
	asm_head->current_stack_size_parameters +=
	    4 * 1; // line->memory_size; // store used stack by parameters for function call clean up
}

void create_asm_pop(FILE *out,
                    struct mcc_ir_line *line,
                    struct mcc_asm_head *asm_head,
                    struct mcc_ir_table *current_func)
{
	assert(out);
	assert(line);
	assert(asm_head);
	assert(current_func);

	struct mcc_ir_function_signature_parameters *params = current_func->line_head->parameters;
	int top = 4 + params->total_size * 4;
	int offset = 0;

	while (params != NULL && strcmp(params->arg_name, line->arg1) != 0) {
		offset = offset + (4 * 1);
		params = params->next_parameter;
	}

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, top - offset,
	                          MCC_ASM_REGISTER_EAX, 0);

	asm_head->offset = asm_head->offset - 4;
	push_on_stack(line, asm_head);

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          asm_head->offset);
}

void create_asm_comparison_int(FILE *out,
                               enum mcc_asm_instruction type,
                               int stack_position_arg2,
                               struct mcc_ir_line *line,
                               struct mcc_asm_head *asm_head)
{
	assert(out);
	assert(line);
	assert(asm_head);

	char *arg2 = lookup_data_section(out, line, asm_head, 2);

	if (arg2 == NULL) {
		print_asm_instruction(out, MCC_ASM_INSTRUCTION_CMP, stack_position_arg2, line->arg2);
	} else {
		print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_CMP, arg2, MCC_ASM_REGISTER_EAX);
		free(arg2);
	}

	print_asm_instruction_reg(out, type, MCC_ASM_REGISTER_CL, 0, -1, 0);
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_ANDL, "1", MCC_ASM_REGISTER_CL, 0);

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVZBL, MCC_ASM_REGISTER_CL, 0, MCC_ASM_REGISTER_EAX, 0);
}

void create_asm_comparison_float(FILE *out, enum mcc_asm_instruction type)
{
	assert(out);

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_FCOMIP, MCC_ASM_REGISTER_ST_1, 0, MCC_ASM_REGISTER_ST, 0);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_FSTP, MCC_ASM_REGISTER_ST_0, 0, -1, 0);

	print_asm_instruction_reg(out, type, MCC_ASM_REGISTER_AL, 0, -1, 0);

	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVZBL, MCC_ASM_REGISTER_AL, 0, MCC_ASM_REGISTER_EAX, 0);
}

void create_asm_int_div(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, "0", MCC_ASM_REGISTER_EDX, 0);
	if (line->arg2[0] == '(') {
		int loc = find_stack_position(line->arg2, asm_head);
		if (loc == -1)
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_ECX, 0);
		else
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, loc,
			                          MCC_ASM_REGISTER_ECX, 0);
	} else {
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_ECX, 0);
	}
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_DIVL, MCC_ASM_REGISTER_ECX, 0, -1, 0);
}

void create_asm_binary_op_int(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	int stack_position_arg1 = -1;
	int stack_position_arg2 = -1;
	if (strncmp(line->arg1, "(", 1) == 0)
		stack_position_arg1 = find_stack_position(line->arg1, asm_head);
	if (strncmp(line->arg2, "(", 1) == 0)
		stack_position_arg2 = find_stack_position(line->arg2, asm_head);

	char *arg1 = lookup_data_section(out, line, asm_head, 1);
	char *arg2 = NULL;

	if (stack_position_arg1 != -1 && arg1 == NULL) // stack pos not found -> must be literal
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_position_arg1,
		                          MCC_ASM_REGISTER_EAX, 0);
	else if (stack_position_arg1 != -1 && stack_position_arg2 != -1) {
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_position_arg1,
		                          MCC_ASM_REGISTER_EAX, 0);
	} else if (arg1 == NULL)
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	else if (arg1 != NULL) {
		print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, arg1, MCC_ASM_REGISTER_EAX);
		arg2 = lookup_data_section(out, line, asm_head, 2);
	}

	switch (line->bin_op) {
	case MCC_AST_BINARY_OP_ADD:
		arg2 == NULL
		    ? print_asm_instruction(out, MCC_ASM_INSTRUCTION_ADDL, stack_position_arg2, line->arg2)
		    : print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_ADDL, arg2, MCC_ASM_REGISTER_EAX);
		break;

	case MCC_AST_BINARY_OP_SUB:
		arg2 == NULL
		    ? print_asm_instruction(out, MCC_ASM_INSTRUCTION_SUBL, stack_position_arg2, line->arg2)
		    : print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_SUBL, arg2, MCC_ASM_REGISTER_EAX);
		break;

	case MCC_AST_BINARY_OP_MUL:
		arg2 == NULL
		    ? print_asm_instruction(out, MCC_ASM_INSTRUCTION_MULL, stack_position_arg2, line->arg2)
		    : print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MULL, arg2, MCC_ASM_REGISTER_EAX);
		break;

	case MCC_AST_BINARY_OP_DIV:
		create_asm_int_div(out, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_LAND:
		print_asm_instruction(out, MCC_ASM_INSTRUCTION_ANDL, stack_position_arg2, line->arg2);
		break;

	case MCC_AST_BINARY_OP_LOR:
		print_asm_instruction(out, MCC_ASM_INSTRUCTION_ORL, stack_position_arg2, line->arg2);
		break;

	case MCC_AST_BINARY_OP_EQ:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_EQ, stack_position_arg2, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_NEQ:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_NEQ, stack_position_arg2, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_GT:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_GT, stack_position_arg2, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_ST:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_ST, stack_position_arg2, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_GE:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_GE, stack_position_arg2, line, asm_head);
		break;

	case MCC_AST_BINARY_OP_SE:
		create_asm_comparison_int(out, MCC_ASM_INSTRUCTION_SET_SE, stack_position_arg2, line, asm_head);
		break;

	default:
		break;
	}

	free(arg1);
	free(arg2);

	asm_head->offset = asm_head->offset - 4;
	push_on_stack(line, asm_head);
	print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
	                          asm_head->offset);
}

void create_asm_binary_op_float(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{
	asm_head->offset = asm_head->offset - 4;

	char *arg1 = lookup_data_section(out, line, asm_head, 1);
	if (arg1 == NULL) {
		arg1 = add_asm_float(line->arg1, line->index, asm_head);
	}
	char *arg2 = lookup_data_section(out, line, asm_head, 2);
	if (arg2 == NULL) {
		arg2 = add_asm_float(line->arg2, line->index, asm_head);
	}

	if (line->bin_op == MCC_AST_BINARY_OP_ST || line->bin_op == MCC_AST_BINARY_OP_SE) {
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, arg1);
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, arg2);
	} else {
		char *tmp;
		tmp = lookup_data_section(out, line, asm_head, 1);
		free(tmp);
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, arg2);
		tmp = lookup_data_section(out, line, asm_head, 2);
		free(tmp);
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, arg1);
	}

	int comp = 0;

	switch (line->bin_op) {
	case MCC_AST_BINARY_OP_ADD:
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FADDS, NULL);
		break;
	case MCC_AST_BINARY_OP_SUB:
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FSUBS, NULL);
		break;
	case MCC_AST_BINARY_OP_MUL:
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FMULS, NULL);
		break;
	case MCC_AST_BINARY_OP_DIV:
		print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FDIVS, NULL);
		break;
	case MCC_AST_BINARY_OP_EQ:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_EQ);
		comp++;
		break;
	case MCC_AST_BINARY_OP_NEQ:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_NEQ);
		comp++;
		break;
	case MCC_AST_BINARY_OP_GT:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_A);
		comp++;
		break;
	case MCC_AST_BINARY_OP_ST:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_A);
		comp++;
		break;
	case MCC_AST_BINARY_OP_GE:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_NB);
		comp++;
		break;
	case MCC_AST_BINARY_OP_SE:
		create_asm_comparison_float(out, MCC_ASM_INSTRUCTION_SET_NB);
		comp++;
		break;
	default:
		break;
	}

	if (!comp) {
		push_on_stack(line, asm_head);
		print_asm_instruction_load_float_reg(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
		                                     asm_head->offset);
	} else {
		push_on_stack(line, asm_head);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
		                          asm_head->offset);
	}

	free(arg1);
	free(arg2);
}

void create_asm_binary_op(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{

	if (line->memory_size > 1) {
		create_asm_binary_op_float(out, line, asm_head);
	} else {
		create_asm_binary_op_int(out, line, asm_head);
	}
}

void create_asm_unary_minus(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *asm_head)
{

	char value[64] = {0};
	if (line->memory_size == 2) { // float
		char *arg1 = lookup_data_section(out, line, asm_head, 1);
		if (arg1 == NULL) {
			sprintf(value, "%s%s", get_un_op_string(line->un_op), line->arg1);
			arg1 = add_asm_float(value, line->index, asm_head);
		} else {
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, arg1);

			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FCHS, NULL);
			push_on_stack(line, asm_head);
			print_asm_instruction_load_float_reg(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
			                                     asm_head->offset);
		}
		free(arg1);
	} else { // int
		if (line->arg1[0] != '(') {
			sprintf(value, "%s%s", get_un_op_string(line->un_op), line->arg1);
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, value, MCC_ASM_REGISTER_EAX, 0);
		} else {
			int stack_pos = find_stack_position(line->arg1, asm_head);

			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_pos,
			                          MCC_ASM_REGISTER_EAX, 0);

			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_NEGL, MCC_ASM_REGISTER_EAX, 0, -1, 0);
		}
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
		                          asm_head->offset);
		push_on_stack(line, asm_head);
	}
}

void create_asm_unary(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	head->offset = head->offset - 4;

	if (line->un_op == MCC_AST_UNARY_OP_NOT) {
		if (line->arg1[0] == '(') {
			int stack_pos = find_stack_position(line->arg1, head);
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_pos,
			                          MCC_ASM_REGISTER_EAX, 0);
		} else if (strchr(line->arg1, '[') || strchr(get_id_by_line_ref(line->arg1, head, 1), '[')) {
			char *index = lookup_data_section_array(out, line->arg1, head, 4);
			print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, index, MCC_ASM_REGISTER_EAX);
			free(index);
		} else {
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
		}

		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_XORL, "1", MCC_ASM_REGISTER_EAX, 0);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0, MCC_ASM_REGISTER_EBP,
		                          head->offset);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, head->offset,
		                          MCC_ASM_REGISTER_EAX, 0);
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_CMP, "0", MCC_ASM_REGISTER_EAX, 0);
		push_on_stack(line, head);
	}

	if (line->un_op == MCC_AST_UNARY_OP_MINUS) {
		create_asm_unary_minus(out, line, head);
	}
}

void create_asm_standalone(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	head->offset = head->offset - 4;

	if (line->arg1[0] == '(') {
		int stack_pos = find_stack_position(line->arg1, head);
		print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, stack_pos,
		                          MCC_ASM_REGISTER_EAX, 0);
	} else {
		print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg1, MCC_ASM_REGISTER_EAX, 0);
	}
	print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_CMP, "0", MCC_ASM_REGISTER_EAX, 0);
}

char *add_string_to_datasection(char *name, char *value, struct mcc_asm_head *head)
{
	if (name == NULL) {
		char var[64];
		sprintf(var, "tmp_%d", head->temp_variable_id);
		name = strdup(var);
	}

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}
	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = name;
	new_data_section->line_no = NULL;
	new_data_section->next_data_section = NULL;
	new_data_section->label_count = head->temp_variable_id;
	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	struct mcc_asm_data_index *new_data_index = malloc(sizeof(*new_data_index));
	new_data_index->value = value;
	new_data_index->next_data_index = NULL;

	data_index_root->value = strdup(".string ");
	data_index_root->next_data_index = new_data_index;
	new_data_section->index = data_index_root;

	head->temp_variable_id = head->temp_variable_id + 1;

	return name;
}

char *add_asm_float(char *arg, int line_no, struct mcc_asm_head *head)
{
	char var[64];
	sprintf(var, "tmp_%d", head->temp_variable_id);

	char *name = strdup(var);

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}

	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = strdup(var);
	new_data_section->line_no = NULL;
	new_data_section->next_data_section = NULL;
	new_data_section->label_count = head->temp_variable_id;
	new_data_section->func_name = head->ir->func_name;

	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	data_index_root->value = strdup(".float ");

	struct mcc_asm_data_index *data_index_next = malloc(sizeof(*data_index_next));
	data_index_next->value = strdup(arg);
	data_index_next->next_data_index = NULL;

	data_index_root->next_data_index = data_index_next;

	new_data_section->index = data_index_root;

	sprintf(var, "(%d)", line_no);
	new_data_section->line_no = strdup(var);

	head->temp_variable_id = head->temp_variable_id + 1;

	return name;
}

char *create_asm_float(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	assert(out);
	assert(line);
	assert(head);

	if (line->arg2[0] == '(')
		return NULL;

	struct mcc_asm_data_section *current = head->data_section;
	while (current->next_data_section != NULL) {
		current = current->next_data_section;
	}

	struct mcc_asm_data_section *new_data_section = malloc(sizeof(*new_data_section));
	new_data_section->id = strdup(line->arg1);
	new_data_section->line_no = NULL;
	new_data_section->next_data_section = NULL;
	new_data_section->label_count = head->temp_variable_id;
	new_data_section->func_name = head->ir->func_name;

	current->next_data_section = new_data_section;

	struct mcc_asm_data_index *data_index_root = malloc(sizeof(*data_index_root));
	data_index_root->value = strdup(".float ");

	struct mcc_asm_data_index *data_index_next = malloc(sizeof(*data_index_next));
	data_index_next->value = strdup(line->arg2);
	data_index_next->next_data_index = NULL;

	data_index_root->next_data_index = data_index_next;

	new_data_section->index = data_index_root;

	char var[64];
	sprintf(var, "(%d)", line->index);
	new_data_section->line_no = strdup(var);

	sprintf(var, "%s_%d", line->arg1, head->temp_variable_id);
	head->temp_variable_id = head->temp_variable_id + 1;

	return strdup(var);
}

void create_asm_array_assignment(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{
	char *index = lookup_data_section_array(out, line->arg1, head, 4);
	int stack_position_arg2 = -1;
	stack_position_arg2 = find_stack_position(line->arg2, head);

	if (line->memory_size == 2) { // float arrays
		if (line->arg2[0] != '(' && line->arg2[strlen(line->arg2) - 1] != ']') {
			char *label = add_asm_float(line->arg2, line->index, head);

			head->offset = head->offset - 4;

			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, label);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FSTPS, index);
			push_on_stack(line, head);
			free(label);
		} else if (line->arg2[0] == '(') {
			print_asm_instruction_load_float_reg(out, MCC_ASM_INSTRUCTION_FLDS, MCC_ASM_REGISTER_EBP,
			                                     stack_position_arg2);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FSTPS, index);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, index);
		} else if (line->arg2[strlen(line->arg2) - 1] == ']') {
			char *index2 = lookup_data_section_array(out, line->arg2, head, 4);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, index2);
			free(index);
			index = lookup_data_section_array(out, line->arg1, head, 4);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FSTPS, index);
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, index);
			free(index2);
		}
	} else { // int arrays

		if (strncmp(line->arg2, "(", 1) == 0) {
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
			                          stack_position_arg2, MCC_ASM_REGISTER_EAX, 0);
		} else if (line->arg2[strlen(line->arg2) - 1] == ']') {
			char *arr_val = lookup_data_section_array(out, line->arg2, head, 4);
			print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, arr_val, MCC_ASM_REGISTER_EAX);
			free(index);
			index = lookup_data_section_array(out, line->arg1, head, 4);
			free(arr_val);
		} else {
			print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2, MCC_ASM_REGISTER_EAX, 0);
		}

		print_asm_instruction_array_set(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, index);
		print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, index, MCC_ASM_REGISTER_EAX);
	}
	free(index);
}

void create_asm_assignment(FILE *out, struct mcc_ir_line *line, struct mcc_asm_head *head)
{

	int stack_position = -1;
	stack_position = find_stack_position(line->arg1, head);
	if (line->op_type == MCC_IR_TABLE_STORE) {
		create_asm_array_assignment(out, line, head);
		return;
	}

	if (line->memory_size == 2) { // single float values
		if (strchr(line->arg2, '[')) {
			char *index = lookup_data_section_array(out, line->arg2, head, 4);
			head->offset = head->offset - 4;
			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, index);
			print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
			                                  head->offset);
			push_on_stack(line, head);
			free(index);
		} else if (line->arg2[0] != '(') {
			char *label = create_asm_float(out, line, head);

			head->offset = head->offset - 4;

			print_asm_instruction_load_float(out, MCC_ASM_INSTRUCTION_FLDS, label);
			print_asm_instruction_store_float(out, MCC_ASM_INSTRUCTION_FSTPS, MCC_ASM_REGISTER_EBP,
			                                  head->offset);
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_FINIT, -1, 0, -1, 0);
			push_on_stack(line, head);
			free(label);
		} else {
			char label[64] = {0};
			sprintf(label, "(%d)", line->index);
			int stack_pos_1 = find_stack_position(line->arg1, head);
			int stack_pos_2 = find_stack_position(line->arg2, head);
			if (stack_pos_1 == -1 && stack_pos_2 != -1) {
				head->offset = head->offset - 4;
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
				                          stack_pos_2, MCC_ASM_REGISTER_EAX, 0);
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0,
				                          MCC_ASM_REGISTER_EBP, head->offset);
				push_on_stack(line, head);
			} else if (stack_pos_1 != -1 && stack_pos_2 != -1) {
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP,
				                          stack_pos_2, MCC_ASM_REGISTER_EAX, 0);
				print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EAX, 0,
				                          MCC_ASM_REGISTER_EBP, stack_pos_1);
				push_on_stack_with_offset(line, head, stack_pos_1);
			} else
				update_data_section_line_number(line->arg2, label, head);
		}
	} else {
		if (strncmp(line->arg2, "(", 1) == 0) {
			int pos = find_stack_position(line->arg2, head);
			print_asm_instruction_reg(out, MCC_ASM_INSTRUCTION_MOVL, MCC_ASM_REGISTER_EBP, pos,
			                          MCC_ASM_REGISTER_EAX, 0);
		} else {
			if (strncmp(line->arg2, "\"", 1) == 0) {
				add_string_to_datasection(strdup(line->arg1), strdup(line->arg2), head);
				char label[64] = {0};
				sprintf(label, "%s_%d", line->arg1, head->temp_variable_id - 1);
				print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, label, MCC_ASM_REGISTER_EAX,
				                          0);
			} else {
				if (strchr(line->arg2, '[')) {
					char *index = lookup_data_section_array(out, line->arg2, head, 4);
					print_asm_instruction_array_get(out, MCC_ASM_INSTRUCTION_MOVL, index,
					                                MCC_ASM_REGISTER_EAX);
					free(index);
				} else {
					print_asm_instruction_lit(out, MCC_ASM_INSTRUCTION_MOVL, line->arg2,
					                          MCC_ASM_REGISTER_EAX, 0);
				}
			}
		}

		if (stack_position == -1) {
			head->offset = head->offset - 4;
			stack_position = head->offset;
			push_on_stack(line, head);
		} else {
			update_stack(line, head->stack);
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
	new_data_section->line_no = NULL;
	new_data_section->next_data_section = NULL;
	new_data_section->label_count = -1;
	new_data_section->func_name = head->ir->func_name;

	struct mcc_asm_data_index *new_data_index_current = malloc(sizeof(*new_data_index_current));
	struct mcc_asm_data_index *new_data_index_root = new_data_index_current;
	char *ptr;
	int len = strtol(line->arg2, &ptr, 10);
	for (int i = 0; i < len; i++) {
		struct mcc_asm_data_index *new_data_index_tmp = malloc(sizeof(*new_data_index_tmp));
		if (line->memory_size != 2) {
			new_data_index_tmp->value = strdup("0");
		} else {
			new_data_index_tmp->value = strdup("0.0");
		}
		new_data_index_current->next_data_index = new_data_index_tmp;
		new_data_index_current = new_data_index_tmp;
	}
	new_data_index_current->next_data_index = NULL;

	if (line->memory_size != 2) {
		new_data_index_root->value = strdup(".long ");
	} else {
		new_data_index_root->value = strdup(".float ");
	}
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
		create_asm_return(out, line, current_func, asm_head);
		break;
	case MCC_IR_TABLE_PUSH:
		create_asm_push(out, line, asm_head);
		break;
	case MCC_IR_TABLE_POP:
		create_asm_pop(out, line, asm_head, current_func);
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
	case MCC_IR_TABLE_BOOL:
		create_asm_standalone(out, line, asm_head);
		break;
	default:
		break;
	}
}

void mcc_delete_stack(struct mcc_asm_stack *stack)
{
	if (stack == NULL) {
		return;
	}

	if (stack->next_stack != NULL) {
		mcc_delete_stack(stack->next_stack);
	}
	free(stack->line_no);
	free(stack->var);
	free(stack);
}

void create_asm_functions(FILE *tmpfile, struct mcc_ir_table *first_line, struct mcc_asm_head *asm_head, int is_main)
{
	struct mcc_ir_table *current_func = first_line;
	while (current_func != NULL) {
		int is_current_main = is_main ? strcmp(current_func->func_name, "main") == 0
		                              : strcmp(current_func->func_name, "main") != 0;

		if (is_current_main) {
			struct mcc_ir_line *current_line = current_func->line_head->root;
			asm_head->offset = 0;
			asm_head->ir = current_func;
			create_function_label(tmpfile, current_func, asm_head);
			while (current_line != NULL) {
				create_asm_line(tmpfile, current_line, asm_head, current_func);
				current_line = current_line->next_line;
			}
		}
		current_func = current_func->next_table;
		mcc_delete_stack(asm_head->stack);
		asm_head->stack = NULL;
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
	asm_head->current_stack_size_parameters = 0;
	asm_head->temp_variable_id = 0;
	asm_head->stack = NULL;

	struct mcc_asm_data_section *data_root = malloc(sizeof(*data_root));
	data_root->id = strdup("\n.data\n");
	data_root->line_no = NULL;
	data_root->index = NULL;
	data_root->next_data_section = NULL;
	data_root->func_name = NULL;

	asm_head->data_section = data_root;

	create_asm_functions(tmpfile, ir->root, asm_head, 1); // create main first
	create_asm_functions(tmpfile, ir->root, asm_head, 0); // then other functions

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

void mcc_delete_data_section(struct mcc_asm_data_section *section)
{
	if (section->next_data_section != NULL) {
		mcc_delete_data_section(section->next_data_section);
	}

	if (section->index != NULL) {
		mcc_delete_data_index(section->index);
	}
	free(section->id);
	free(section->line_no);
	free(section);
}

void mcc_delete_asm(struct mcc_asm_head *asm_head)
{
	if (asm_head->data_section != NULL) {
		mcc_delete_data_section(asm_head->data_section);
	}

	if (asm_head->stack != NULL) {
		mcc_delete_stack(asm_head->stack);
	}
	free(asm_head);
}
