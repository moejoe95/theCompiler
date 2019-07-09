#ifndef MCC_PRINT_ASM_H
#define MCC_PRINT_ASM_H

#include "mcc/asm.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void print_asm_instruction_reg(FILE *out,
                               enum mcc_asm_instruction in,
                               enum mcc_asm_operand op1,
                               int op1_offset,
                               enum mcc_asm_operand op2,
                               int op2_offset);

void print_asm_instruction_call(FILE *out, enum mcc_asm_instruction in, char *lit);
void print_asm_instruction_lit(
    FILE *out, enum mcc_asm_instruction in, char *lit, enum mcc_asm_operand op2, int op2_offset);
void print_asm_instruction_load_float(FILE *out, enum mcc_asm_instruction in, char *lit);
void print_asm_instruction_load_float_reg(FILE *out, enum mcc_asm_instruction in, enum mcc_asm_operand reg, int offset);
void print_asm_instruction_store_float(FILE *out, enum mcc_asm_instruction in, enum mcc_asm_operand reg, int offset);

void create_asm_header(FILE *out);
FILE *open_tmp_file();

void print_asm_data_section(FILE *out, struct mcc_asm_data_section *data);

#endif // MCC_PRINT_ASM
