#include "mcc/cfg.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_node(struct mcc_block *current, struct mcc_block *new, bool add_child)
{
	if (!add_child) {
		struct mcc_block *temp = current;
		while (temp->next_block != NULL) {
			temp = temp->next_block;
		}
		temp->next_block = new;
		return;
	}

	if (current->child_blocks == NULL) {
		struct child_blocks *child = malloc(sizeof(*child));
		if (!child)
			return;

		current->child_blocks = child;
		current->child_blocks->head = new;
	} else {
		struct mcc_block *temp = current->child_blocks->head;
		while (temp->next_block != NULL) {
			temp = temp->next_block;
		}
		temp->next_block = new;
	}
}

static void generate_block(struct mcc_cfg *cfg, int table_id_start, bool add_child)
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
	block->child_blocks = NULL;
	block->next_block = NULL;

	if (cfg->current_block == NULL) {
		cfg->root_block = block;
	} else {
		add_node(cfg->current_block, block, add_child);
	}

	cfg->current_block = block;
}

// TODO Andreas remove this print and replace it with something cool
char *string_repeat2(int n, const char *s)
{
	size_t slen = strlen(s);
	char *dest = malloc(n * slen + 1);

	int i;
	char *p;
	for (i = 0, p = dest; i < n; ++i, p += slen) {
		memcpy(p, s, slen);
	}
	*p = '\0';
	return dest;
}

void print_block(struct mcc_block *temp_block, int indent)
{
	char *indention = string_repeat2(indent, "\t");
	printf("%sblock %d start %d end %d\n", indention, temp_block->block_id, temp_block->table_id_start,
	       temp_block->table_id_end);

	if (temp_block->next_block != NULL) {
		print_block(temp_block->next_block, indent);
	}

	if (temp_block->child_blocks != NULL) {
		indent = indent + 1;
		print_block(temp_block->child_blocks->head, indent);
	}
}
// TODO Andras End

struct mcc_cfg *generate_cfg(struct mcc_ir_table *ir)
{
	assert(ir);

	struct mcc_cfg *cfg = malloc(sizeof(*cfg));
	if (!cfg)
		return NULL;

	cfg->current_block = NULL;

	bool child = true;

	while (ir != NULL) {
		switch (ir->op_type) {
		case MCC_IR_TABLE_JUMP:
			if (cfg->current_block != NULL)
				cfg->current_block->table_id_end = (ir->index - 1);
			generate_block(cfg, ir->index, child);
			child = true;
			break;
		case MCC_IR_TABLE_JUMPFALSE:
			if (cfg->current_block != NULL)
				cfg->current_block->table_id_end = (ir->index - 1);
			generate_block(cfg, ir->index, true);
			child = false;
			break;
		case MCC_IR_TABLE_LABEL:
			if (cfg->current_block != NULL)
				cfg->current_block->table_id_end = (ir->index - 1);
			generate_block(cfg, ir->index, child);
			break;
		default:
			break;
		}

		if (ir->next_table == NULL) {
			cfg->current_block->table_id_end = ir->index;
		}
		ir = ir->next_table;
	}

	print_block(cfg->root_block, 0);
	return cfg;
}