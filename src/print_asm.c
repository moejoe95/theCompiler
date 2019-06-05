#include "mcc/print_asm.h"

void print_asm_instruction(FILE *out, char *op, char *arg1, char *arg2)
{
	assert(op);
	if (arg1 == NULL && arg2 == NULL)
		fprintf(out, "\t%s\n", op);
	else if (arg2 == NULL)
		fprintf(out, "\t%s\t%s\n", op, arg1);
	else
		fprintf(out, "\t%s\t%s, %s\n", op, arg1, arg2);
}