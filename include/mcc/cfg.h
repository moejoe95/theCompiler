#ifndef MCC_CFG_H
#define MCC_CFG_H

#include "mcc/ir.h"
#include "mcc/print_cfg.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct mcc_cfg {
	struct mcc_block *root_block;
	struct mcc_block *current_block;
};

struct mcc_block {
	int block_id;
	int table_id_start;
	int table_id_end;

	struct child_blocks *child_blocks;
	struct mcc_block *next_block;
};

struct child_blocks {
	struct mcc_block *head;
};

struct mcc_cfg *generate_cfg(struct mcc_ir_table *ir);

#endif // MCC_CFG_H