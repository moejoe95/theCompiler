#include "mcc/asm.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ir.h"

void mcc_print_asm_table(struct mcc_asm_table *table, FILE *out)
{
	fprintf(out, "ASM table\n\n");
	struct mcc_asm_line *line = table->root;
	while (line != NULL) {
		fprintf(out, "print line\n");
		line = line->next_line;
	}
}

struct mcc_asm_table *mcc_create_asm(struct mcc_ir_table_head *ir, FILE *out, int log_level)
{
	assert(ir);

	struct mcc_asm_table *table = malloc(sizeof(*table));
	table->current = NULL;
	table->root = NULL;

	if (log_level > 0)
		mcc_print_asm_table(table, out);

	return table;
}

void mcc_delete_asm_line(struct mcc_asm_line *line)
{
	if (line->next_line != NULL) {
		mcc_delete_asm_line(line->next_line);
	}
	free(line);
}

void mcc_delete_asm(struct mcc_asm_table *table)
{
	if (table->root != NULL)
		mcc_delete_asm_line(table->root);
	free(table);
}
