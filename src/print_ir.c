#include "mcc/print_ir.h"
#include <string.h>

void print_table_line(FILE *out)
{
	for (int i = 0; i < 109; i++)
		fprintf(out, "-");
	fprintf(out, "\n");
}

void print_table_legend(FILE *out)
{
	fprintf(out, "generate IR code...\n\n");
	print_table_line(out);
	fprintf(out, "| %s\t| %s\t| %-40s| %-40s|\n", "index", "operation", "argument 1", "argument 2");
	print_table_line(out);
}

void print_row(struct mcc_ir_table *table, FILE *out)
{

	if (!table->arg2) {
		char *entity2 = "-";
		table->arg2 = entity2;
	}

	char operation[32] = {0};
	switch (table->op_type) {
	case MCC_IR_TABLE_UNARY_OP:
		sprintf(operation, "%s", get_un_op_string(table->un_op));
		strcat(operation, "\t");
		break;
	case MCC_IR_TABLE_BINARY_OP:
		sprintf(operation, "%s", get_bin_op_string(table->bin_op));
		strcat(operation, "\t");
		break;
	case MCC_IR_TABLE_ASSIGNMENT:
		sprintf(operation, "%s", "assign");
		break;
	case MCC_IR_TABLE_JUMPFALSE:
		sprintf(operation, "%s", "jumpfalse");
		break;
	case MCC_IR_TABLE_JUMP:
		sprintf(operation, "%s", "jump\t");
		break;
	case MCC_IR_TABLE_LABEL:
		sprintf(operation, "%s", "label\t");
		break;
	case MCC_IR_TABLE_COPY:
		sprintf(operation, "%s", "copy\t");
		break;
	case MCC_IR_TABLE_POP:
		sprintf(operation, "%s", "pop\t");
		break;
	case MCC_IR_TABLE_PUSH:
		sprintf(operation, "%s", "push\t");
		break;
	case MCC_IR_TABLE_LOAD:
		sprintf(operation, "%s", "load\t");
		break;
	case MCC_IR_TABLE_STORE:
		sprintf(operation, "%s", "store\t");
		break;
	case MCC_IR_TABLE_BUILT_IN:
		sprintf(operation, "%s", "builtin\t");
		break;
	default:
		sprintf(operation, "%s", "undef op");
		break;
	}

	fprintf(out, "| %d\t| %s\t| %-40.40s| %-40.40s|\n", table->index, operation, table->arg1, table->arg2);
}

void mcc_print_ir_table(struct mcc_ir_table *table, FILE *out)
{
	print_table_legend(out);
	struct mcc_ir_table *current_table = table->next_table;
	while (current_table != NULL) {
		print_row(current_table, out);
		current_table = current_table->next_table;
	}
	print_table_line(out);
}
