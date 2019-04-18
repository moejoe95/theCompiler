// Symbol Table Infrastructure
//
// This module provides basic symbol table infrastructure for the symbol table data structure.

#ifndef MCC_SYMBOL_TABLE_H
#define MCC_SYMBOL_TABLE_H

#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include <assert.h>
#include <mcc/ast_print.h>
#include <stdio.h>

struct temp_create_symbol_table {
	// TODO Andreas, define errors
	int create_inner_scope;
	int main_found;
	// int index;
	int is_duplicate;
	struct mcc_symbol_table *symbol_table;
	// struct mcc_symbol *current_function;
	// struct mcc_sc_if_else_stmt *check_return;
	// mcc_array *error_list;
};

struct mcc_symbol_table {
	char *label;
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
	struct mcc_ast_identifier *identifier;
	struct mcc_symbol *next_symbol;
};

struct mcc_symbol_list {
	struct mcc_symbol *head;
};

void add_symbol_to_list(struct mcc_symbol_list *list, struct mcc_symbol *symbol);
void add_symbol_table_to_list(struct mcc_symbol_table_list *list, struct mcc_symbol_table *table);

struct mcc_symbol_table *mcc_create_symbol_table(struct mcc_ast_program *program);

struct mcc_ast_visitor generate_symbol_table_visitor(struct temp_create_symbol_table *temp_st);

// struct mcc_ast_symbol_declaration *mcc_ast_lookup_function_symbol(struct mcc_ast_symbol_table *symbol_table,
//                                                                   char *symbol);

// struct mcc_ast_symbol_declaration *mcc_ast_lookup_symbol(struct mcc_ast_symbol_table *symbol_table, char *symbol);

struct mcc_symbol *lookup_symbol_in_scope(struct mcc_symbol_table *symbol_table, char *key);

void insert_built_in_symbol(struct temp_create_symbol_table *temp_st,
                            char *identifier,
                            enum mcc_ast_type return_type,
                            enum mcc_ast_type parameter_type);

struct mcc_symbol *
create_symbol_built_in(enum mcc_ast_type type, struct mcc_ast_identifier *identifier, long *arr_size);

void insert_symbol_function(struct temp_create_symbol_table *tmp, struct mcc_ast_func_definition *function_def);

void enter_scope(struct temp_create_symbol_table *tmp, struct mcc_symbol_table *symbol_table);
void exit_scope(struct temp_create_symbol_table *tmp);

void mcc_delete_symbol_table(struct mcc_symbol_table *symbol_table);

void mcc_print_symbol_table(FILE *out, struct mcc_symbol_table *symbol_table);

#endif // MCC_AST_SYMBOL_TABLE_H