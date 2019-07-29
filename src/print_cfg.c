#include "mcc/print_cfg.h"

#define LINE_SIZE 256
#define BLOCK_SIZE 4096

static char *escape_string(char *source_str)
{
	for (size_t i = 0; i < strlen(source_str); i++) {
		switch (source_str[i]) {
		case '\"':
			source_str[i] = '\'';
			break;
		default:
			break;
		}
	}
	return source_str;
}

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

static void print_dot_node(FILE *out, const void *node, char *label)
{
	assert(out);
	assert(node);
	assert(label);

	escape_string(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);

	if (label != NULL) {
		fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node, label);
	} else {
		fprintf(out, "\t\"%p\" -> \"%p\"\n", src_node, dst_node);
	}
}

static void get_table_line(struct mcc_ir_line *line, char *target)
{
	char value[LINE_SIZE] = {0};

	switch (line->op_type) {
	case MCC_IR_TABLE_UNARY_OP:
		sprintf(value, "(%d): %s %s\n", line->index, line->arg1, get_un_op_string(line->un_op));
		break;
	case MCC_IR_TABLE_BINARY_OP:
		sprintf(value, "(%d): %s %s %s\n", line->index, line->arg1, get_bin_op_string(line->bin_op),
		        line->arg2);
		break;
	case MCC_IR_TABLE_ASSIGNMENT:
		sprintf(value, "(%d): %s %s %s\n", line->index, line->arg1, "=", line->arg2);
		break;
	case MCC_IR_TABLE_PUSH:
		sprintf(value, "(%d): %s %s\n", line->index, "push", line->arg1);
		break;
	case MCC_IR_TABLE_POP:
		sprintf(value, "(%d): %s %s\n", line->index, "pop", line->arg1);
		break;
	case MCC_IR_TABLE_CALL:
		sprintf(value, "(%d): %s %s\n", line->index, "call", line->arg1);
		break;
	case MCC_IR_TABLE_RETURN:
		sprintf(value, "(%d): %s %s\n", line->index, "return", line->arg1);
		break;
	case MCC_IR_TABLE_COPY:
		sprintf(value, "(%d): %s %s %s\n", line->index, "copy", line->arg1, line->arg2);
		break;
	case MCC_IR_TABLE_STORE:
		sprintf(value, "(%d): %s %s %s %s\n", line->index, "store", line->arg2, "in", line->arg1);
		break;
	case MCC_IR_TABLE_LOAD:
		sprintf(value, "(%d): %s %s %s %s\n", line->index, "load", line->arg2, "to", line->arg1);
		break;

	case MCC_IR_TABLE_JUMPFALSE:
		sprintf(value, "(%d): %s %s %s %s\n", line->index, "if not", line->arg1, "goto", line->arg2);
		break;
	case MCC_IR_TABLE_JUMP:
		sprintf(value, "(%d): %s %s\n", line->index, "goto", line->arg1);
		break;
	case MCC_IR_TABLE_LABEL:
		sprintf(value, "(%d): %s %s\n", line->index, "label", line->arg1);
		break;
	case MCC_IR_TABLE_NULL:
		sprintf(value, "(%d): %s \n", line->index, "null");
		break;

	default:
		break;
	}
	strcat(target, value);
}

static char *get_ir_entries(struct mcc_ir_line *ir, int start, int end, char *target)
{

	while (ir != NULL) {
		if (ir->index == start) {
			while (ir != NULL && ir->index != end + 1) {
				get_table_line(ir, target);
				ir = ir->next_line;
			}
			break;
		}
		ir = ir->next_line;
	}

	return target;
}

static void print_block_node(struct mcc_ir_line *ir, FILE *out, struct mcc_block *temp_block, struct mcc_block *parent)
{
	assert(ir);
	assert(out);
	assert(temp_block);

	if (temp_block->printed) {
		if (parent != NULL) {
			print_dot_edge(out, parent, temp_block, NULL);
		}
		return;
	} else {
		temp_block->printed = true;
	}

	char target[BLOCK_SIZE] = {0};
	char *entity = get_ir_entries(ir, temp_block->table_id_start, temp_block->table_id_end, target);
	print_dot_node(out, temp_block, entity);

	if (parent != NULL) {
		print_dot_edge(out, parent, temp_block, NULL);
	}

	if (temp_block->child_first != NULL) {
		print_block_node(ir, out, temp_block->child_first, temp_block);
	}

	if (temp_block->child_second != NULL) {
		print_block_node(ir, out, temp_block->child_second, temp_block);
	}
}

struct mcc_block *find_start_block(struct mcc_cfg *cfg, int table_id)
{
	struct mcc_block *current_block = cfg->root_block;

	while (current_block != NULL) {
		if (current_block->is_start_of == table_id) {
			return current_block;
		}

		current_block = current_block->next_block;
	}
	return NULL;
}

void print_cfg(struct mcc_ir_table_head *ir_table_head, struct mcc_cfg *cfg, FILE *out)
{
	assert(ir_table_head);
	assert(cfg);
	assert(out);

	struct mcc_ir_table *ir_table = ir_table_head->root;

	print_dot_begin(out);

	while (ir_table != NULL) {
		struct mcc_block *start_block = find_start_block(cfg, ir_table->id);
		if (start_block != NULL) {
			print_block_node(ir_table->line_head->root, out, start_block, NULL);
		}
		ir_table = ir_table->next_table;
	}

	print_dot_end(out);
}
