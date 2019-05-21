#include "mcc/print_cfg.h"

#define LABEL_SIZE 64

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n"
	             "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node, label);
}

static char* get_table_line(struct mcc_ir_table *line){
    char value[256] = {0};

    switch (line->op_type){
    case MCC_IR_TABLE_LABEL:
        sprintf(value, "%d: %s\n", line->index, "label"); //TODO should not exist
        break;
    case MCC_IR_TABLE_UNARY_OP:
        sprintf(value, "%d: %s %s\n", line->index, line->arg1, line->un_op);
        break;
    case MCC_IR_TABLE_BINARY_OP:
        sprintf(value, "%d: %s %s\n", line->index, line->arg1, line->bin_op, line->arg2);
        break;
    case MCC_IR_TABLE_ASSIGNMENT:
        sprintf(value, "%d: %s %s %s\n", line->index, line->arg1, "=", line->arg2);
        break;
    
    default:
        sprintf(value, "TODO\n");
        break;
    }

    return strdup(value);

}

static char* get_ir_entries(struct mcc_ir_table *ir, int start, int end){
    char result[256] = {0};
     
    while (ir != NULL) {
        if(ir->index == start){
            while(ir->index != end){
                strcat(result, get_table_line(ir)); //TODO correct indeces for calling table line (no labels, jumps etc?)
                ir = ir ->next_table;
            }
            break;
        }
        ir = ir -> next_table;
    }
    
    return strdup(result);
}

static void print_block_node(struct mcc_ir_table *ir, FILE *out, struct mcc_block *temp_block)
{
	print_dot_node(out, temp_block, get_ir_entries(ir, temp_block->table_id_start, temp_block->table_id_end));

	if (temp_block->next_block != NULL) {
		print_block_node(ir, out, temp_block->next_block);
	}

	if (temp_block->child_blocks != NULL) {
        print_dot_edge(out, temp_block, temp_block->child_blocks->head, "child");
		print_block_node(ir, out, temp_block->child_blocks->head);
	}
}


void print_cfg(struct mcc_ir_table *ir, struct mcc_cfg *cfg, FILE *out){
    assert(ir);
    assert(cfg);
    assert(out);

	print_dot_begin(out);

    print_block_node(ir, out, cfg->root_block);

	print_dot_end(out);
}

