#include "mcc/cfg.h"

static void generate_block(struct mcc_cfg* cfg, struct mcc_ir_table* ir){
    assert(cfg);
    assert(ir);
    
    struct mcc_block *block = malloc(sizeof(*block));
	if (!block)
		return NULL;

    block->block_id = cfg->current_block->block_id + 1;

    printf("current block id: %d\n", cfg->current_block->block_id);
    printf("new block id: %d\n", block->block_id);

    cfg->current_block->next_block = block;
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
                generate_block(cfg, ir);
                break;

            case MCC_IR_TABLE_JUMPFALSE:
                generate_block(cfg, ir);
                break;
            case MCC_IR_TABLE_LABEL:
                generate_block(cfg, ir);
                break;			
		}
        ir = ir->next_table;
    }

    return cfg;
}