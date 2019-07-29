// Symbol Table Infrastructure
//
// This module provides basic symbol table infrastructure for the symbol table data structure.

#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include <assert.h>
#include <mcc/ast_print.h>
#include <stdbool.h>
#include <stdio.h>

#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "mcc/error_handler.h"

struct temp_create_symbol_table {
	FILE *out;
	int create_inner_scope;
	int main_found;
	int is_duplicate;
	int is_returned;
	int is_scoped;
	int is_quiet;
	struct mcc_symbol_table *symbol_table;
	bool error_found;
};

struct mcc_symbol_table {
	char *label;
	struct mcc_ast_source_location *sloc;
	struct mcc_symbol_list *symbols;
	struct mcc_symbol_table *parent;
	struct mcc_symbol_table *next;
	struct mcc_symbol_table_list *sub_tables;
};

struct mcc_symbol_table_list {
	struct mcc_symbol_table *head;
};

struct mcc_symbol {
	enum mcc_ast_type type;
	int index;
	long *array_size;
	int numArgs;
	struct argument_type_list *argument_type_list;
	struct mcc_ast_identifier *identifier;
	struct mcc_symbol *next_symbol;
};

struct argument_type_list {
	enum mcc_ast_type type;
	enum mcc_ast_type array_type;
	struct argument_type_list *next_type;
};

struct mcc_symbol_list {
	struct mcc_symbol *head;
};

struct mcc_symbol_table *
mcc_create_symbol_table(struct mcc_ast_program *program, FILE *out, int log_level, int is_scoped, int is_quiet);

void mcc_delete_symbol_table(struct mcc_symbol_table *symbol_table);
void mcc_delete_symbol(struct mcc_symbol *symbol);
void mcc_delete_argument_typelist(struct argument_type_list *list);

struct mcc_symbol *lookup_symbol_in_scope(struct mcc_symbol_table *symbol_table, char *key);
struct mcc_symbol *lookup_symbol(struct mcc_symbol_table *symbol_table, char *symbol);

#endif // MCC_AST_SYMBOL_TABLE_H
