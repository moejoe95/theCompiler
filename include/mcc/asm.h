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

	// compare
	MCC_ASM_INSTRUCTION_CMP,

	// calls
	MCC_ASM_INSTRUCTION_CALL,

	// jumps
	MCC_ASM_INSTRUCTION_JE,
	MCC_ASM_INSTRUCTION_JNE,
	MCC_ASM_INSTRUCTION_JZ,
	MCC_ASM_INSTRUCTION_JG,
	MCC_ASM_INSTRUCTION_JGE,
	MCC_ASM_INSTRUCTION_JL,
	MCC_ASM_INSTRUCTION_JLE,
	MCC_ASM_INSTRUCTION_JMP

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

struct mcc_asm_data_index {
	char *value;
	struct mcc_asm_data_index *next_data_index;
};

struct mcc_asm_data_section {
	char *id;
	struct mcc_asm_data_index *index;
	struct mcc_asm_data_section *next_data_section;
};

struct mcc_asm_stack {
	char *var;
	int stack_position;
	struct mcc_asm_stack *next_stack;
};

struct mcc_asm_head {
	int offset;
	struct mcc_asm_data_section *data_section;
	int current_stack_size_parameters;
	struct mcc_asm_stack *stack;
	int temp_variable_id;
};

char *add_string_to_datasection(char *name, char *value, struct mcc_asm_head *head);

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int destination);
void mcc_delete_asm(struct mcc_asm_head *asm_head);

#endif