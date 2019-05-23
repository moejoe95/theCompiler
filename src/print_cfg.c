#include "mcc/print_cfg.h"

static char *escape_string(const char *source_str, char *target_str)
{
	for (size_t i = 0; i < strlen(source_str); i++) {
		switch (source_str[i]) {
		case '\"':
			target_str[i] = '\'';
			break;
		default:
			target_str[i] = source_str[i];
			break;
		}
	}
	return target_str;
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

	char escaped_string[512] = {0};
	escape_string(label, escaped_string);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, escaped_string);
	free(label);
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

static char *get_table_line(struct mcc_ir_table *line)
{
	char value[256] = {0};

	switch (line->op_type) {
	case MCC_IR_TABLE_UNARY_OP:
		sprintf(value, "(%d): %s %d\n", line->index, line->arg1, line->un_op);
		break;
	case MCC_IR_TABLE_BINARY_OP:
		sprintf(value, "(%d): %s %d %s\n", line->index, line->arg1, line->bin_op, line->arg2);
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
	case MCC_IR_TABLE_BUILT_IN:
		sprintf(value, "(%d): %s %s\n", line->index, "builtin", line->arg1);
		break;
	case MCC_IR_TABLE_CALL:
		sprintf(value, "(%d): %s %s %s\n", line->index, "call", line->arg1, line->arg2);
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

	default:
		break;
	}

	return strdup(value);
}

static char *get_ir_entries(struct mcc_ir_table *ir, int start, int end)
{
	char result[256] = {0};

	while (ir != NULL) {
		if (ir->index == start) {
			while (ir != NULL && ir->index != end + 1) {
				char *line = get_table_line(ir);
				strcat(result, line);
				free(line);
				ir = ir->next_table;
			}
			break;
		}
		ir = ir->next_table;
	}

	return strdup(result);
}

static void print_block_node(struct mcc_ir_table *ir, FILE *out, struct mcc_block *temp_block, struct mcc_block *parent)
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

	print_dot_node(out, temp_block, get_ir_entries(ir, temp_block->table_id_start, temp_block->table_id_end));

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

void print_cfg(struct mcc_ir_table *ir, struct mcc_cfg *cfg, FILE *out)
{
	assert(ir);
	assert(cfg);
	assert(out);

	print_dot_begin(out);

	print_block_node(ir, out, cfg->root_block, NULL);

	print_dot_end(out);
}
