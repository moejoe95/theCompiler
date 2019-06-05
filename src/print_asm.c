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
	case MCC_ASM_INSTRUCTION_LEAVE:
		strcpy(in_string, "leave");
		break;
	case MCC_ASM_INSTRUCTION_RETL:
		strcpy(in_string, "retl");
		break;
	default:
		strcpy(in_string, "UNDEF");
		break;
	}
}

void createRegisterOffsetAccess(char *reg, char *off_reg, int offset)
{
	sprintf(off_reg, "%d(%s)", offset, reg);
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
	default:
		arg = "";
		break;
	}

	if (offset != 0) {
		char reg_off[OPERAND_SIZE];
		createRegisterOffsetAccess(arg, reg_off, offset);
		strcpy(arg, reg_off);
	}
}

void print_asm_instruction(FILE *out,
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

	if (strcmp(arg1, "") == 0)
		fprintf(out, "\t%s\n", op);
	else
		fprintf(out, "\t%s\t%s, %s\n", op, arg1, arg2);
}