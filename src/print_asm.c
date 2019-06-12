#include "mcc/print_asm.h"

#define INSTRUCTION_SIZE 64
#define OPERAND_SIZE 64

void getAssemblyInstruction(enum mcc_asm_instruction in, char *in_string)
{
	switch (in) {
	case MCC_ASM_INSTRUCTION_MOVL:
		strcpy(in_string, "movl");
		break;
	case MCC_ASM_INSTRUCTION_PUSHL:
		strcpy(in_string, "pushl");
		break;
	case MCC_ASM_INSTRUCTION_POPL:
		strcpy(in_string, "popl");
		break;
	case MCC_ASM_INSTRUCTION_LEAVE:
		strcpy(in_string, "leave");
		break;
	case MCC_ASM_INSTRUCTION_RETL:
		strcpy(in_string, "ret");
		break;
	case MCC_ASM_INSTRUCTION_SUBL:
		strcpy(in_string, "subl");
		break;
	case MCC_ASM_INSTRUCTION_ADDL:
		strcpy(in_string, "addl");
		break;
	case MCC_ASM_INSTRUCTION_MULL:
		strcpy(in_string, "imul");
		break;
	case MCC_ASM_INSTRUCTION_DIVL:
		strcpy(in_string, "idivl");
		break;
	case MCC_ASM_INSTRUCTION_ANDL:
		strcpy(in_string, "andl");
		break;
	case MCC_ASM_INSTRUCTION_ORL:
		strcpy(in_string, "orl");
		break;
	case MCC_ASM_INSTRUCTION_NOTL:
		strcpy(in_string, "notl");
		break;
	case MCC_ASM_INSTRUCTION_CALL:
		strcpy(in_string, "call");
		break;
	case MCC_ASM_INSTRUCTION_JZ:
		strcpy(in_string, "jz");
		break;
	case MCC_ASM_INSTRUCTION_CMP:
		strcpy(in_string, "cmp");
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
	else if (strcmp(arg2, "") == 0)
		fprintf(out, "\t%s\t%s\n", op, arg1);
	else
		fprintf(out, "\t%s\t%s, %s\n", op, arg1, arg2);
}

void print_asm_instruction_call(FILE *out, enum mcc_asm_instruction in, char *lit)
{
	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};

	getAssemblyInstruction(in, op);
	sprintf(arg1, "%s", lit);

	print_asm_line(out, op, arg1, "");
}

void getAssemblyOperand(enum mcc_asm_operand op, int offset, char *arg)
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

	getAssemblyInstruction(in, op);
	getAssemblyOperand(op1, op1_offset, arg1);
	getAssemblyOperand(op2, op2_offset, arg2);

	print_asm_line(out, op, arg1, arg2);
}

void print_asm_instruction_lit(
    FILE *out, enum mcc_asm_instruction in, char *lit, enum mcc_asm_operand op2, int op2_offset)
{

	char op[INSTRUCTION_SIZE] = {0};
	char arg1[OPERAND_SIZE] = {0};
	char arg2[OPERAND_SIZE] = {0};

	getAssemblyInstruction(in, op);
	sprintf(arg1, "$%s", lit);
	getAssemblyOperand(op2, op2_offset, arg2);

	print_asm_line(out, op, arg1, arg2);
}

void create_asm_header(FILE *out)
{
	fprintf(out, "\t.text\n");
}

void print_asm_data_section(FILE *out, struct mcc_asm_data_section *data)
{
	struct mcc_asm_data_section *current = data;
	while (current != NULL && current->array != NULL) {
		fprintf(out, "%s\n", current->id);
		fprintf(out, "\t%s\n", current->array);
		current = current->next_data_section;
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
