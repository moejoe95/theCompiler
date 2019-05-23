#include "mcc/cfg.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void generate_block(struct mcc_cfg *cfg, int table_id_start)
{
	assert(cfg);

	struct mcc_block *block = malloc(sizeof(*block));
	if (!block)
		return;

	if (cfg->current_block == NULL) {
		block->block_id = 0;
	} else {
		block->block_id = cfg->current_block->block_id + 1;
	}
	block->table_id_start = table_id_start;
	block->table_id_end = 0;
	block->next_block = NULL;
	block->has_follower = false;

	if (cfg->current_block == NULL) {
		cfg->root_block = block;
	} else {
		struct mcc_block *temp = cfg->current_block;
		while (temp->next_block != NULL) {
			temp = temp->next_block;
		}
		temp->next_block = block;
	}

	cfg->current_block = block;
}

void print_basic_blocks(FILE *out, struct mcc_block *temp_block)
{
	fprintf(out, "%s\n", "generate basic blocks...");

	while (temp_block != NULL) {
		fprintf(out, "block %d start %d end %d target %d follower %d\n", temp_block->block_id,
		        temp_block->table_id_start, temp_block->table_id_end, temp_block->target_id,
		        temp_block->has_follower);
		temp_block = temp_block->next_block;
	}
}

struct mcc_cfg *generate_cfg(struct mcc_ir_table *ir, FILE *out, int log_level)
{
	assert(ir);

	struct mcc_ir_table *ir_cfg_print = ir;

	struct mcc_cfg *cfg = malloc(sizeof(*cfg));
	if (!cfg)
		return NULL;

	cfg->current_block = NULL;

	while (ir != NULL) {
		switch (ir->op_type) {
		case MCC_IR_TABLE_JUMP:
			if (cfg->current_block != NULL) {
				cfg->current_block->table_id_end = (ir->index);
				cfg->current_block->target_id = ir->jump_target;
			}
			generate_block(cfg, ir->next_table->index);
			break;
		case MCC_IR_TABLE_JUMPFALSE:
			if (cfg->current_block != NULL) {
				cfg->current_block->table_id_end = (ir->index);
				cfg->current_block->target_id = ir->jump_target;
				cfg->current_block->has_follower = true;
			}
			generate_block(cfg, ir->next_table->index);
			break;
		case MCC_IR_TABLE_LABEL:
			if (cfg->current_block != NULL) {
				cfg->current_block->table_id_end = (ir->index);
				cfg->current_block->target_id = ir->jump_target;
			}
			generate_block(cfg, ir->next_table->index);
			break;
		default:
			break;
		}

		if (ir->next_table == NULL) {
			cfg->current_block->table_id_end = ir->index;
			cfg->current_block->target_id = 0;
		}
		ir = ir->next_table;
	}

	if (log_level == 2)
		print_basic_blocks(out, cfg->root_block);
	// print_cfg(ir_cfg_print, cfg, stdout); // TODO change file out

	return cfg;
}