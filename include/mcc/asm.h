#ifndef THECOMPILER_ASM_H
#define THECOMPILER_ASM_H

#include <stdio.h>
#include <string.h>

#include "mcc/ir.h"

enum mcc_asm_instruction {
    MCC_ASM_INSTRUCTION_MOVL,
    MCC_ASM_INSTRUCTION_PUSHL,
    MCC_ASM_INSTRUCTION_LEAVE,
    MCC_ASM_INSTRUCTION_RETL,
};

enum mcc_asm_operand {
    MCC_ASM_REGISTER_EAX,
    MCC_ASM_REGISTER_EBP,
    MCC_ASM_REGISTER_ESP,
};

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out);

#endif