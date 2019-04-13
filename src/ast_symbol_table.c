#include "mcc/ast_symbol_table.h"
#include "mcc/ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct mcc_ast_symbol_table *mcc_ast_create_symbol_table(struct mcc_ast_program *program)
{
	struct mcc_ast_symbol_table *st = malloc(sizeof(*st));
	if (!st) {
		return NULL;
	}

	return st;
}

void mcc_ast_delete_symbol_table(struct mcc_ast_symbol_table *symbol_table)
{
	free(symbol_table);
}
