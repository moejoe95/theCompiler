#include "mcc/cfg.h"
#include "mcc/print_cfg.h"

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
	block->child_first = NULL;
	block->child_second = NULL;
	block->target_id = 0;

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

struct mcc_block *find_block(struct mcc_cfg *cfg, int line)
{
	struct mcc_block *current_block = cfg->root_block;

	while (current_block != NULL) {
		if (current_block->table_id_start == line) {
			return current_block;
		}

		current_block = current_block->next_block;
	}
	return NULL;
}

void create_cf_graph(struct mcc_cfg *cfg)
{
	if (cfg->root_block == NULL) {
		return;
	}

	struct mcc_block *current_block = cfg->root_block;

	do {
		struct mcc_block *next_block;
		if (current_block->target_id == 0) {
			next_block = current_block->next_block;
		} else {
			next_block = find_block(cfg, current_block->target_id);
		}
		current_block->child_first = next_block;
		if (current_block->has_follower) {
			current_block->child_second = current_block->next_block;
		}
		current_block = current_block->next_block;
	} while (current_block != NULL);
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
				cfg->current_block->table_id_end = (ir->index - 1);
				cfg->current_block->target_id = ir->jump_target;
			}
			generate_block(cfg, ir->index);
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

	create_cf_graph(cfg);

	if (log_level == 2)
		print_basic_blocks(out, cfg->root_block);
	print_cfg(ir_cfg_print, cfg, out);

	return cfg;
}

void mcc_delete_cfg(struct mcc_cfg *cfg)
{
	struct mcc_block *current_block = cfg->root_block;
	struct mcc_block *temp;

	while (current_block != NULL) {
		temp = current_block->next_block;
		free(current_block);
		current_block = temp;
	}

	free(cfg);
}