#ifndef MCC_CFG_H
#define MCC_CFG_H

#include "mcc/ir.h"
#include <assert.h>
#include <stdbool.h>
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
	int target_id;
	bool has_follower;

	struct mcc_block *next_block;

	struct mcc_block *child_first;
	struct mcc_block *child_second;
};

struct mcc_cfg *generate_cfg(struct mcc_ir_table *ir, FILE *out, int log_level);
void mcc_delete_cfg(struct mcc_cfg *cfg);

#endif // MCC_CFG_H