#include "mcc/print_asm.h"

#define INSTRUCTION_SIZE 64
#define OPERAND_SIZE 64

void get_assembly_instruction(enum mcc_asm_instruction in, char *in_string)
{
	switch (in) {
	case MCC_ASM_INSTRUCTION_MOVL:
		strcpy(in_string, "mov");
		break;
	case MCC_ASM_INSTRUCTION_PUSHL:
		strcpy(in_string, "push");
		break;
	case MCC_ASM_INSTRUCTION_POPL:
		strcpy(in_string, "pop");
		break;
	case MCC_ASM_INSTRUCTION_LEAVE:
		strcpy(in_string, "leave");
		break;
	case MCC_ASM_INSTRUCTION_RETL:
		strcpy(in_string, "ret");
		break;
	case MCC_ASM_INSTRUCTION_SUBL:
		strcpy(in_string, "sub");
		break;
	case MCC_ASM_INSTRUCTION_ADDL:
		strcpy(in_string, "add");
		break;
	case MCC_ASM_INSTRUCTION_MULL:
		strcpy(in_string, "imul");
		break;
	case MCC_ASM_INSTRUCTION_DIVL:
		strcpy(in_string, "idiv");
		break;
	case MCC_ASM_INSTRUCTION_ANDL:
		strcpy(in_string, "and");
		break;
	case MCC_ASM_INSTRUCTION_ORL:
		strcpy(in_string, "or");
		break;
	case MCC_ASM_INSTRUCTION_NOTL:
		strcpy(in_string, "not");
		break;
	case MCC_ASM_INSTRUCTION_CALL:
		strcpy(in_string, "call");
		break;
	case MCC_ASM_INSTRUCTION_JZ:
		strcpy(in_string, "jz");
		break;
	case MCC_ASM_INSTRUCTION_JL:
		strcpy(in_string, "jl");
		break;
	case MCC_ASM_INSTRUCTION_JG:
		strcpy(in_string, "jg");
		break;
	case MCC_ASM_INSTRUCTION_JGE:
		strcpy(in_string, "jge");
		break;
	case MCC_ASM_INSTRUCTION_JLE:
		strcpy(in_string, "jle");
		break;
	case MCC_ASM_INSTRUCTION_JE:
		strcpy(in_string, "je");
		break;
	case MCC_ASM_INSTRUCTION_JNE:
		strcpy(in_string, "jne");
		break;
	case MCC_ASM_INSTRUCTION_CMP:
		strcpy(in_string, "cmpl");
		break;
	case MCC_ASM_INSTRUCTION_JMP:
		strcpy(in_string, "jmp");
		break;
	case MCC_ASM_INSTRUCTION_FLDL:
		strcpy(in_string, "fldl");
		break;
	case MCC_ASM_INSTRUCTION_FSTPL:
		strcpy(in_string, "fstpl");
		break;
	case MCC_ASM_INSTRUCTION_FADDL:
		strcpy(in_string, "faddl");
		break;
	default:
		strcpy(in_string, "UNDEF");
		break;
	}
}

void print_asm_line(FILE *out, char *op, char *arg1, char *arg2)
{
	if (strcmp(arg1, "") == 0)
		fprintf(out, "\t%s\n", op);
	else if (arg2 != NULL && strcmp(arg2, "") == 0)
		fprintf(out, "\t%s\t%s\n", op, arg1);
	else if (arg2 != NULL)
		fprintf(out, "\t%s\t%s, %s\n", op, arg1, arg2);
	else
		fprintf(out, "\t%s\t%s\n", op, arg1);
}

void print_asm_instruction_call(FILE *out, enum mcc_asm_instruction in, char *lit)
{
	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};

	get_assembly_instruction(in, op);
	sprintf(arg1, "%s", lit);

	print_asm_line(out, op, arg1, "");
}

void get_assembly_operand(enum mcc_asm_operand op, int offset, char *arg)
{
	switch (op) {
	case MCC_ASM_REGISTER_EAX:
		strcpy(arg, "%eax");
		break;
	case MCC_ASM_REGISTER_EBP:
		strcpy(arg, "%ebp");
		break;
	case MCC_ASM_REGISTER_ESP:
		strcpy(arg, "%esp");
		break;
	case MCC_ASM_REGISTER_EDX:
		strcpy(arg, "%edx");
		break;
	case MCC_ASM_REGISTER_ECX:
		strcpy(arg, "%ecx");
		break;
	default:
		arg = "";
		break;
	}

	if (offset != 0) {
		char reg_off[OPERAND_SIZE];
		sprintf(reg_off, "%d(%s)", offset, arg);
		strcpy(arg, reg_off);
	}
}

void print_asm_instruction_reg(FILE *out,
                               enum mcc_asm_instruction in,
                               enum mcc_asm_operand op1,
                               int op1_offset,
                               enum mcc_asm_operand op2,
                               int op2_offset)
{

	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};
	char arg2[OPERAND_SIZE] = {0};

	get_assembly_instruction(in, op);
	get_assembly_operand(op1, op1_offset, arg1);
	get_assembly_operand(op2, op2_offset, arg2);

	print_asm_line(out, op, arg1, arg2);
}

void print_asm_instruction_lit(
    FILE *out, enum mcc_asm_instruction in, char *lit, enum mcc_asm_operand op2, int op2_offset)
{

	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};
	char arg2[OPERAND_SIZE] = {0};

	get_assembly_instruction(in, op);
	sprintf(arg1, "$%s", lit);
	get_assembly_operand(op2, op2_offset, arg2);

	print_asm_line(out, op, arg1, arg2);
}

void print_asm_instruction_load_float(FILE *out, enum mcc_asm_instruction in, char *lit)
{
	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};

	get_assembly_instruction(in, op);
	sprintf(arg1, "%s", lit);

	print_asm_line(out, op, arg1, NULL);
}

void print_asm_instruction_store_float(FILE *out, enum mcc_asm_instruction in, enum mcc_asm_operand reg, int offset)
{
	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};

	get_assembly_instruction(in, op);
	get_assembly_operand(reg, offset, arg1);

	print_asm_line(out, op, arg1, NULL);
}

void create_asm_header(FILE *out)
{
	fprintf(out, ".text\n");
}

void print_asm_data_section(FILE *out, struct mcc_asm_data_section *data)
{
	if (data->next_data_section == NULL)
		return;

	struct mcc_asm_data_section *current = data;
	fprintf(out, "%s", current->id);
	current = current->next_data_section;
	while (current != NULL) {
		fprintf(out, "\n%s:\n", current->id);
		struct mcc_asm_data_index *index = current->index;
		fprintf(out, "\t");
		int i = 0;
		while (index != NULL) {
			fprintf(out, "%s", index->value);
			index = index->next_data_index;
			if (index != NULL && i != 0)
				fprintf(out, ", ");
			i++;
		}
		current = current->next_data_section;
		fprintf(out, "\n");
	}
}
FILE *open_tmp_file()
{
	FILE *tmp_file = fopen("asm_tmp.s", "w");
	if (!tmp_file) {
		perror("fopen");
		return NULL;
	}
	return tmp_file;
}
