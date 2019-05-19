#ifndef MCC_CFG_H
#define MCC_CFG_H

#include "mcc/ir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct mcc_cfg{
    char* function_label;
    struct mcc_block *root_block;
    struct mcc_block *current_block;

    struct mcc_cfg *next_function;
};

struct mcc_block{
    int block_id;

    struct mcc_ir_table *entities;

    struct mcc_block *next_block;
};

struct mcc_cfg* generate_cfg(struct mcc_ir_table *ir);

#endif // MCC_CFG_H