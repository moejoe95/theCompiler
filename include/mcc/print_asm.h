#ifndef MCC_PRINT_ASM_H
#define MCC_PRINT_ASM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mcc/asm.h"


void print_asm_instruction(FILE *out,
                           enum mcc_asm_instruction in,
                           enum mcc_asm_operand op1,
                           int op1_offset,
                           enum mcc_asm_operand op2,
                           int op2_offset);

#endif // MCC_PRINT_ASM
