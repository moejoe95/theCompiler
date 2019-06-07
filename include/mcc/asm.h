#ifndef THECOMPILER_ASM_H
#define THECOMPILER_ASM_H

#include <stdio.h>
#include <string.h>

#include "mcc/ir.h"

enum mcc_asm_instruction {
	// moving instruction
	MCC_ASM_INSTRUCTION_MOVL,

	// stack instructions
	MCC_ASM_INSTRUCTION_PUSHL,
	MCC_ASM_INSTRUCTION_POPL,

	// return instructions
	MCC_ASM_INSTRUCTION_LEAVE,
	MCC_ASM_INSTRUCTION_RETL,

	// integer arithmetic
	MCC_ASM_INSTRUCTION_SUBL,
	MCC_ASM_INSTRUCTION_ADDL,
	MCC_ASM_INSTRUCTION_MULL,
	MCC_ASM_INSTRUCTION_DIVL,
	MCC_ASM_INSTRUCTION_NEGL,

	// logical arithmetic
	MCC_ASM_INSTRUCTION_ANDL,
	MCC_ASM_INSTRUCTION_ORL,
	MCC_ASM_INSTRUCTION_NOTL,

	// calls
	MCC_ASM_INSTRUCTION_CALL
};

enum mcc_asm_operand {
	// 32 bit registers
	MCC_ASM_REGISTER_EAX,
	MCC_ASM_REGISTER_EBX,
	MCC_ASM_REGISTER_EDX,
	MCC_ASM_REGISTER_ECX,

	// 32 bit registers
	MCC_ASM_REGISTER_EDI,
	MCC_ASM_REGISTER_ESI,

	// Stack pointers (32 bit)
	MCC_ASM_REGISTER_EBP,
	MCC_ASM_REGISTER_ESP,
};

struct mcc_asm_head {
	int offset;
};

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int destination);
void mcc_delete_asm(struct mcc_asm_head *asm_head);

#endif