#include "mcc/cfg.h"

static void generate_block(struct mcc_cfg* cfg, struct mcc_ir_table* ir, int table_id_start, int table_id_end){
    assert(cfg);
    assert(ir);
    
    struct mcc_block *block = malloc(sizeof(*block));
	if (!block)
		return NULL;

    block->block_id = cfg->current_block->block_id + 1;
    block->table_id_start = table_id_start;
    block->table_id_end = table_id_end;

    printf("current block id: %d\n", cfg->current_block->block_id);
    printf("new block id: %d\n", block->block_id);

    cfg->current_block->child_blocks->child = block;
    cfg->current_block = block;
}

struct mcc_cfg* generate_cfg(struct mcc_ir_table *ir){

    assert(ir);

	struct mcc_cfg *cfg = malloc(sizeof(*cfg));
	if (!cfg)
		return NULL;

    struct mcc_block *mcc_block = malloc(sizeof(*mcc_block));
	if (!mcc_block)
		return NULL;

    mcc_block->block_id = 0;

    cfg->root_block = mcc_block;
    cfg->current_block = mcc_block;


    while (ir != NULL) {
        switch (ir->op_type){
            case MCC_IR_TABLE_JUMP:
                generate_block(cfg, ir, 0, 4);
                break;

            case MCC_IR_TABLE_JUMPFALSE:
                generate_block(cfg, ir, 0, 1);
                break;
            case MCC_IR_TABLE_LABEL:
                generate_block(cfg, ir, 0, 1);
                break;			
		}
        ir = ir->next_table;
    }

    return cfg;
}