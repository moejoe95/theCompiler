#ifndef THECOMPILER_ASM_H
#define THECOMPILER_ASM_H

#include <stdio.h>
#include <string.h>

#include "mcc/ir.h"

void mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out);

#endif