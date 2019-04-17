#include "mcc/symbol_table.h"
#include "mcc/ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static struct mcc_symbol_table *allocate_symbol_table(struct mcc_symbol_table *symbol_table_parent)
{
	struct mcc_symbol_table *symbol_table = malloc(sizeof(*symbol_table));
	if (!symbol_table) {
		return NULL;
	}

	if (symbol_table_parent == NULL) {
		symbol_table->label = "global";
	} else {
		symbol_table_parent->label = "";
	}

	symbol_table->parent = symbol_table_parent;

	struct mcc_symbol_list *symbol_list = malloc(sizeof(*symbol_list));
	if (!symbol_list) {
		return NULL;
	}
	symbol_list->head = NULL;
	symbol_table->symbols = symbol_list;

	return symbol_table;
}

void add_built_ins(struct temp_create_symbol_table *temp_st)
{
	assert(temp_st);

	// void print(const char *msg);
	insert_built_in_symbol(temp_st, "print", MCC_AST_TYPE_VOID, MCC_AST_TYPE_STRING);

	// void print_nl(void);
	insert_built_in_symbol(temp_st, "print_nl", MCC_AST_TYPE_VOID, MCC_AST_TYPE_VOID);

	// void    print_int(long x);
	insert_built_in_symbol(temp_st, "print_int", MCC_AST_TYPE_VOID, MCC_AST_TYPE_INT);

	// void print_float(double x);
	insert_built_in_symbol(temp_st, "print_float", MCC_AST_TYPE_VOID, MCC_AST_TYPE_FLOAT);

	// long read_int(void);
	insert_built_in_symbol(temp_st, "read_int", MCC_AST_TYPE_INT, MCC_AST_TYPE_VOID);

	// double read_float(void);
	insert_built_in_symbol(temp_st, "read_float", MCC_AST_TYPE_FLOAT, MCC_AST_TYPE_VOID);
}

void insert_built_in_symbol(struct temp_create_symbol_table *temp_st,
                            char *identifier,
                            enum mcc_ast_type return_type,
                            enum mcc_ast_type parameter_type)
{
	struct mcc_ast_identifier *id = malloc(sizeof(*id));

	if (!id) {
		return;
	}
	id->node.sloc.start_line = 0;
	id->node.sloc.start_col = 0;
	id->node.sloc.end_line = 0;
	id->node.sloc.end_col = 0;
	id->node.sloc.filename = NULL;
	id->name = identifier;
	struct mcc_symbol *symbol = create_symbol_built_in(return_type, id, NULL);

	// if (parameter_type != MCC_AST_TYPE_VOID) {
	// 	ARRAY_INIT(parameter_declaration);

	// 	struct mcc_ast_symbol_declaration *sym_parameter =
	// 	    create_sym_declaration_built_in(parameter_type, NULL, NULL);
	// 	ARRAY_ADD(parameter_declaration, sym_parameter);
	// 	sym_declaration->parameter_declaration = parameter_declaration;
	// }

	add_symbol_to_list(temp_st->symbol_table->symbols, symbol);
}

struct mcc_symbol *create_symbol_built_in(enum mcc_ast_type type, struct mcc_ast_identifier *identifier, long *arr_size)
{
	struct mcc_symbol *sym = malloc(sizeof(*sym));
	if (!sym) {
		return NULL;
	}
	sym->type = type;
	sym->identifier = identifier;
	sym->array_size = arr_size;
	sym->index = -1;
	sym->next_symbol = NULL;

	return sym;
}

struct mcc_symbol_table *mcc_create_symbol_table(struct mcc_ast_program *program)
{
	if (program == NULL) {
		return NULL;
	}

	struct temp_create_symbol_table *temp_st = malloc(sizeof(*temp_st));
	// temp_st->create_inner_scope = 1;
	temp_st->main_found = 0;
	// temp_st->index = 0;
	// temp_st->error_list = error_list;
	temp_st->symbol_table = allocate_symbol_table(NULL);
	// temp_st->is_duplicate = 0;
	// temp_st->check_return = NULL;

	add_built_ins(temp_st);

	struct mcc_ast_visitor visitor = generate_symbol_table_visitor(temp_st);
	mcc_ast_visit(program, &visitor);

	// temp_st = visitor.userdata;

	if (!temp_st->main_found) {
		// TODO Andreas, print error
	}

	struct mcc_symbol_table *symbol_table = temp_st->symbol_table;

	free(temp_st);
	return symbol_table;
}

static void symbol_table_declaration(struct mcc_ast_declare_assign *declaration, void *data)
{
	assert(declaration);
	assert(data);

	struct temp_create_symbol_table *temp = data;
	struct mcc_symbol *previous_declaration =
	    lookup_symbol_in_scope(temp->symbol_table, declaration->declare_id->identifier->name);

	if (previous_declaration != NULL) {
		// TODO Andreas, add error
		printf("error, duplicate declaration\n");
		return;
	}

	struct mcc_symbol *symbol =
	    create_symbol_built_in(declaration->declare_type, declaration->declare_id->identifier->name, NULL);

	add_symbol_to_list(temp->symbol_table->symbols, symbol);

	// if (!userdata->create_inner_scope && !userdata->is_duplicate) {
	// 	assert(userdata->current_function->parameter_declaration);
	// 	ARRAY_ADD(userdata->current_function->parameter_declaration, tmp);
	// }
}

struct mcc_symbol *lookup_symbol_in_scope(struct mcc_symbol_table *symbol_table, char *key)
{
	struct mcc_symbol *tmp = symbol_table->symbols->head;

	while (tmp != NULL && tmp->next_symbol != NULL) {
		// FIXME, segfault
		// printf(tmp->identifier->name);
		// printf("\n");
		if (tmp->identifier->name == key) {
			return tmp;
		}
		tmp = tmp->next_symbol;
	}

	return NULL;
}

struct mcc_ast_visitor generate_symbol_table_visitor(struct temp_create_symbol_table *temp_st)
{
	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_PRE_ORDER,

	    .userdata = temp_st,
	    // .pre_visit_function = 1,

	    .declaration = symbol_table_declaration

	    // .statement_compound = symbol_table_compound_pre,
	    // .statement_compound_post = symbol_table_compound_post,
	    // .declaration = symbol_table_declaration,
	    // .function_def = symbol_table_function_def,
	    // .expression_identifier = symbol_table_identifier,
	    // .expression_function_call = symbol_table_function_call,
	    // .pre_function = symbol_table_pre_function_def,

	    // .function_def_post = post_function_def,
	    // .statement_if = check_if_stmt_start_if_else,
	    // .statement_return = check_if_statement_return,
	    // .statement_if_end = check_if_statement_end_if_else,
	    // .statement_if_start_else = check_if_statement_end_if_else,
	};
}

void mcc_delete_symbol_table(struct mcc_symbol_table *symbol_table)
{
	free(symbol_table);
}

void mcc_print_symbol_table(FILE *out, struct mcc_symbol_table *symbol_table)
{
	assert(out);

	if (!symbol_table) {
		return;
	}

	fprintf(out, "symbol_table ");
	fprintf(out, symbol_table->label);
	fprintf(out, "\n\nname\t\t|\ttype\n--------------------------\n");

	struct mcc_symbol *current_symbol = symbol_table->symbols->head;
	while (current_symbol != NULL) {
		// FIXME segfault
		// fprintf(out, current_symbol->identifier->name);
		fprintf(out, "\t\t|\t");
		fprintf(out, get_type_string(current_symbol->type));
		fprintf(out, "\n");

		current_symbol = current_symbol->next_symbol;
	}
}

void add_symbol_to_list(struct mcc_symbol_list *list, struct mcc_symbol *symbol)
{
	assert(list);
	assert(symbol);

	struct mcc_symbol *current = list->head;

	if (current == NULL) {
		list->head = symbol;
		return;
	}

	while (current->next_symbol != NULL) {
		current = current->next_symbol;
	}

	current->next_symbol = symbol;
}

void add_symbol_table_to_list(struct mcc_symbol_table_list *list, struct mcc_symbol_table *table)
{
	struct mcc_symbol_table *current = list->head;

	if (current == NULL) {
		list->head = table;
		return;
	}

	while (current->next != NULL) {
		current = current->next;
	}

	current->next = table;
}