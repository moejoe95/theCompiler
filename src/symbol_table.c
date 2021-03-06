#include "mcc/symbol_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/error_handler.h"

struct mcc_ast_visitor generate_symbol_table_visitor(struct temp_create_symbol_table *temp_st);
struct mcc_ast_identifier *
create_ast_identifier(int start_line, int start_col, int end_line, int end_col, char *filename, char *identifier);

void add_child_symbol_table(struct mcc_symbol_table *parent, struct mcc_symbol_table *table);
void add_symbol_to_list(struct mcc_symbol_list *list, struct mcc_symbol *symbol);
void insert_built_in_symbol(struct temp_create_symbol_table *temp_st,
                            char *identifier,
                            enum mcc_ast_type return_type,
                            enum mcc_ast_type parameter_type);
void insert_symbol_function(struct mcc_symbol_table *st,
                            struct mcc_ast_func_definition *function_def,
                            int numArgs,
                            struct argument_type_list *argument_type_list);
void mcc_print_symbol_table(FILE *out, struct mcc_symbol_table *symbol_table, int indent);

void enter_scope(struct temp_create_symbol_table *tmp, struct mcc_symbol_table *symbol_table)
{
	tmp->symbol_table = symbol_table;
}

void exit_scope(struct temp_create_symbol_table *tmp)
{
	tmp->symbol_table = tmp->symbol_table->parent;
}

static struct argument_type_list *create_argument_type_list()
{
	struct argument_type_list *argument_type_list = malloc(sizeof(*argument_type_list));
	if (!argument_type_list) {
		return NULL;
	}
	argument_type_list->next_type = NULL;
	return argument_type_list;
}

static struct mcc_symbol_table *
allocate_symbol_table(struct mcc_symbol_table *symbol_table_parent, char *label, struct mcc_ast_source_location *sloc)
{
	struct mcc_symbol_table *symbol_table = malloc(sizeof(*symbol_table));
	if (!symbol_table) {
		return NULL;
	}

	if (label != NULL) {
		symbol_table->label = label;
	} else {
		symbol_table->label = "anonymous";
	}

	symbol_table->parent = symbol_table_parent;

	struct mcc_symbol_list *symbol_list = malloc(sizeof(*symbol_list));
	if (!symbol_list) {
		return NULL;
	}
	symbol_list->head = NULL;
	symbol_table->symbols = symbol_list;
	symbol_table->next = NULL;
	symbol_table->sub_tables = NULL;
	symbol_table->sloc = sloc;

	return symbol_table;
}

struct mcc_symbol *lookup_symbol_in_scope(struct mcc_symbol_table *symbol_table, char *key)
{
	if (symbol_table == NULL || symbol_table->symbols == NULL || symbol_table->symbols->head == NULL) {
		return NULL;
	}

	struct mcc_symbol *tmp = symbol_table->symbols->head;

	while (tmp != NULL) {
		if (strcmp(tmp->identifier->name, key) == 0) {
			return tmp;
		}
		if (tmp->next_symbol == NULL) {
			break;
		}
		tmp = tmp->next_symbol;
	}

	return NULL;
}

struct mcc_symbol *lookup_symbol(struct mcc_symbol_table *symbol_table, char *symbol)
{
	struct mcc_symbol *sym = NULL;
	do {
		sym = lookup_symbol_in_scope(symbol_table, symbol);
		if (symbol_table != NULL)
			symbol_table = symbol_table->parent;
	} while (!sym && symbol_table);

	return sym;
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
	struct mcc_ast_identifier *id = create_ast_identifier(0, 0, 0, 0, NULL, identifier);

	struct mcc_symbol *sym = malloc(sizeof(*sym));
	if (!sym) {
		return;
	}
	sym->type = return_type;
	sym->identifier = id;
	sym->array_size = 0;
	sym->numArgs = 0;
	sym->index = -1;
	sym->next_symbol = NULL;
	sym->argument_type_list = NULL;

	if (parameter_type != MCC_AST_TYPE_VOID) {
		sym->numArgs = 1;
		struct argument_type_list *argument_type_list = create_argument_type_list();
		argument_type_list->type = parameter_type;
		sym->argument_type_list = argument_type_list;
	}

	add_symbol_to_list(temp_st->symbol_table->symbols, sym);
}

struct mcc_ast_identifier *
create_ast_identifier(int start_line, int start_col, int end_line, int end_col, char *filename, char *identifier)
{
	struct mcc_ast_identifier *id = malloc(sizeof(*id));

	char *name = strdup(identifier);

	if (!id) {
		return NULL;
	}
	id->node.sloc.start_line = start_line;
	id->node.sloc.start_col = start_col;
	id->node.sloc.end_line = end_line;
	id->node.sloc.end_col = end_col;
	id->node.sloc.filename = filename;
	id->name = name;

	return id;
}

struct mcc_symbol *create_symbol(enum mcc_ast_type type,
                                 struct mcc_ast_identifier *identifier,
                                 long *arr_size,
                                 int numArgs,
                                 struct argument_type_list *argument_type_list)
{
	struct mcc_ast_identifier *id = create_ast_identifier(
	    identifier->node.sloc.start_line, identifier->node.sloc.start_col, identifier->node.sloc.end_line,
	    identifier->node.sloc.end_col, NULL, identifier->name);

	struct mcc_symbol *sym = malloc(sizeof(*sym));
	if (!sym) {
		return NULL;
	}
	sym->type = type;
	sym->identifier = id;
	sym->array_size = arr_size;
	sym->numArgs = numArgs;
	sym->index = -1;
	sym->next_symbol = NULL;
	sym->argument_type_list = argument_type_list;

	return sym;
}

struct mcc_symbol_table *
mcc_create_symbol_table(struct mcc_ast_program *program, FILE *out, int log_level, int is_scoped, int is_quiet)
{
	if (program == NULL) {
		return NULL;
	}

	struct temp_create_symbol_table *temp_st = malloc(sizeof(*temp_st));
	temp_st->create_inner_scope = 1;
	temp_st->main_found = 0;
	temp_st->symbol_table = allocate_symbol_table(NULL, "global", NULL);
	temp_st->is_returned = 0;
	temp_st->out = out;
	temp_st->error_found = false;
	temp_st->is_scoped = is_scoped;
	temp_st->is_quiet = is_quiet;

	add_built_ins(temp_st);

	struct mcc_ast_visitor visitor = generate_symbol_table_visitor(temp_st);
	mcc_ast_visit(program, &visitor);

	if (!temp_st->main_found && !temp_st->is_scoped) {
		temp_st->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_NO_MAIN);

		if (program->type == MCC_AST_PROGRAM_TYPE_EMPTY) {
			error->sloc = &program->node.sloc;
		} else {
			error->sloc = &program->function_list->node.sloc;
		}

		print_semantic_error(error, temp_st->out, temp_st->is_quiet);
	}

	struct mcc_symbol_table *symbol_table = temp_st->symbol_table;
	if (temp_st->error_found) {
		symbol_table = NULL;
		mcc_delete_symbol_table(temp_st->symbol_table);
	}

	free(temp_st);

	if (log_level != 0) {
		mcc_print_symbol_table(out, symbol_table, 0);
		fprintf(out, "\n");
	}
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
		temp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_DUPLICATE_DECLARATION);
		error->sloc = &declaration->node.sloc;
		error->identifier = declaration->declare_id->identifier;
		print_semantic_error(error, temp->out, temp->is_quiet);
		return;
	}

	struct mcc_symbol *symbol = create_symbol(declaration->declare_type, declaration->declare_id->identifier,
	                                          declaration->declare_array_size, 0, NULL);

	add_symbol_to_list(temp->symbol_table->symbols, symbol);

	// if (!userdata->create_inner_scope && !userdata->is_duplicate) {
	// 	assert(userdata->current_function->parameter_declaration);
	// 	ARRAY_ADD(userdata->current_function->parameter_declaration, tmp);
	// }
}

static void symbol_table_compound(struct mcc_ast_statement *statement, void *data, enum mcc_ast_visit_order order)
{
	struct mcc_symbol_table *symbol_table;
	struct temp_create_symbol_table *tmp = data;

	switch (order) {
	case MCC_AST_VISIT_PRE_ORDER:
		symbol_table = allocate_symbol_table(tmp->symbol_table, "compound", &statement->node.sloc);
		add_child_symbol_table(tmp->symbol_table, symbol_table);
		enter_scope(tmp, symbol_table);
		break;

	case MCC_AST_VISIT_POST_ORDER:
		exit_scope(tmp);
		break;
	}
}

static void symbol_table_compound_pre(struct mcc_ast_statement *statement, void *data)
{
	struct temp_create_symbol_table *tmp = data;
	if (tmp->create_inner_scope) {
		symbol_table_compound(statement, data, MCC_AST_VISIT_PRE_ORDER);
	} else {
		tmp->create_inner_scope = 1;
	}
}

static void symbol_table_compound_post(struct mcc_ast_statement *statement, void *data)
{
	symbol_table_compound(statement, data, MCC_AST_VISIT_POST_ORDER);
}

// forward declaration
static void check_compound_return(struct temp_create_symbol_table *tmp, struct mcc_ast_statement_list *list);

static void check_if_statement_return(struct temp_create_symbol_table *tmp, struct mcc_ast_statement *stmt)
{
	assert(stmt);

	if (stmt->else_stat == NULL) { // if there is no else, there must be a return in the scope above
		return;
	}

	if (stmt->if_stat->type == MCC_AST_STATEMENT_RETURN) {
		if (stmt->else_stat->type == MCC_AST_STATEMENT_RETURN)
			tmp->is_returned = 1;
		else if (stmt->else_stat->type == MCC_AST_STATEMENT_COMPOUND)
			check_compound_return(tmp, stmt->else_stat->compound);
	} else if (stmt->if_stat->type == MCC_AST_STATEMENT_COMPOUND) {
		check_compound_return(tmp, stmt->if_stat->compound);
		if (tmp->is_returned) {
			tmp->is_returned = 0;
			check_compound_return(tmp, stmt->else_stat->compound);
		}
	}
}

static void check_compound_return(struct temp_create_symbol_table *tmp, struct mcc_ast_statement_list *list)
{
	if (list == NULL)
		return; // for empty compounds
	assert(list);

	while (list != NULL) {
		struct mcc_ast_statement *stmt = list->statement;
		switch (stmt->type) {
		case MCC_AST_STATEMENT_RETURN:
			tmp->is_returned = 1;
			return;
			break;

		case MCC_AST_STATEMENT_COMPOUND:
			check_compound_return(tmp, stmt->compound);
			break;

		case MCC_AST_STATEMENT_IF:
			check_if_statement_return(tmp, stmt);
			break;

		default:
			break;
		}

		list = list->next_statement;
	}
}

static void check_return(struct temp_create_symbol_table *tmp, struct mcc_ast_func_definition *function_def)
{
	assert(function_def);

	// void functions need no return statement
	if (function_def->func_type == MCC_AST_TYPE_VOID)
		tmp->is_returned = 1;

	struct mcc_ast_statement_list *list = function_def->func_compound->compound;
	check_compound_return(tmp, list);
}

static void symbol_table_function_def(struct mcc_ast_func_definition *function, void *data)
{
	struct temp_create_symbol_table *tmp = data;
	char *func_id = function->func_identifier->identifier->name;

	if (!tmp->main_found) {
		int compare = strcmp("main", func_id);
		if (compare == 0 && function->func_type == MCC_AST_TYPE_INT && function->parameter_list == NULL) {
			tmp->main_found = 1;
		}
	}

	struct mcc_symbol *previous_declaration = lookup_symbol_in_scope(tmp->symbol_table, func_id);
	if (previous_declaration != NULL) {
		tmp->error_found = true;
		struct mcc_semantic_error *error =
		    get_mcc_semantic_error_struct(MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION);
		error->sloc = &function->node.sloc;
		error->identifier = function->func_identifier->identifier;
		print_semantic_error(error, tmp->out, tmp->is_quiet);
		return;
	}

	// check if non-void function returns value
	check_return(tmp, function);
	if (!tmp->is_returned && function->func_type != MCC_AST_TYPE_VOID) {
		tmp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_NO_RETURN);
		error->sloc = &function->node.sloc;
		error->identifier = function->func_identifier->identifier;
		print_semantic_error(error, tmp->out, tmp->is_quiet);
	}
	tmp->is_returned = 0;

	struct mcc_symbol_table *outer_symbol_table = tmp->symbol_table;
	struct mcc_symbol_table *symbol_table = allocate_symbol_table(tmp->symbol_table, func_id, &function->node.sloc);
	add_child_symbol_table(tmp->symbol_table, symbol_table);
	enter_scope(tmp, symbol_table);

	tmp->create_inner_scope = 0;
	// tmp->current_function = lookup_function_symbol(symbol_table, function->func_identifier->identifier->name);
	if (function->parameter_list) {
		struct mcc_ast_parameter *param = function->parameter_list;
		do {
			symbol_table_declaration(param->parameter, tmp);
			param = param->next_parameter;
		} while (param);
	}

	struct argument_type_list *arg_type_list_first = create_argument_type_list();
	int numArgs = 0;
	if (function->parameter_list) {
		struct mcc_ast_parameter *param = function->parameter_list;
		struct mcc_symbol *sym =
		    lookup_symbol_in_scope(symbol_table, param->parameter->declare_id->identifier->name);
		if (sym->array_size != NULL) {
			arg_type_list_first->type = MCC_AST_TYPE_ARRAY;
			arg_type_list_first->array_type = sym->type;
		} else {
			arg_type_list_first->type = sym->type;
		}
		struct argument_type_list *arg_type_list_temp = arg_type_list_first;
		do {
			numArgs++;
			if (param->next_parameter) {
				struct argument_type_list *argument_type_list_next = create_argument_type_list();
				sym = lookup_symbol_in_scope(
				    symbol_table, param->next_parameter->parameter->declare_id->identifier->name);
				if (sym->array_size != NULL) {
					argument_type_list_next->type = MCC_AST_TYPE_ARRAY;
					argument_type_list_next->array_type = sym->type;
				} else {
					argument_type_list_next->type = sym->type;
				}
				arg_type_list_temp->next_type = argument_type_list_next;
				arg_type_list_temp = argument_type_list_next;
			}
			param = param->next_parameter;
		} while (param);
	}

	// set_semantic_annotation_function_duplicate(function, 0);
	insert_symbol_function(outer_symbol_table, function, numArgs, arg_type_list_first);
}

void insert_symbol_function(struct mcc_symbol_table *st,
                            struct mcc_ast_func_definition *function_def,
                            int numArgs,
                            struct argument_type_list *argument_type_list)
{
	assert(function_def);
	struct mcc_symbol *sym = create_symbol(function_def->func_type, function_def->func_identifier->identifier, NULL,
	                                       numArgs, argument_type_list);

	// function_def->func_identifier->identifier->sym_declaration = sym;

	add_symbol_to_list(st->symbols, sym);
}

static void post_function_def(struct mcc_ast_func_definition *function, void *data)
{
	assert(data);
	assert(function);

	// struct userdata_create_symbol_table *userdata = data;
	// struct mCc_sc_if_else_stmt *if_stmt_res = userdata->check_return;

	// int return_found = delete_and_return_found(if_stmt_res);
	// set_semantic_annotation_function_return(function, return_found);

	// if (!return_found && function->fun_type != MCC_AST_TYPE_VOID) {
	// 	struct mCc_sc_error *error = mCc_error_get_semantic_error_struct(MCC_SC_ERROR_TYPE_MISSING_RETURN);
	// 	error->identifier = function->fun_identifier->identifier;
	// 	error->sloc = &error->identifier->node.sloc;
	// 	ARRAY_ADD(userdata->error_list, error);
	// }
}

static void symbol_table_function_call(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	struct temp_create_symbol_table *tmp = data;

	struct mcc_symbol *sym =
	    lookup_symbol(tmp->symbol_table, expression->function_call_identifier->identifier->name);

	struct mcc_semantic_error *error;
	if (!sym) {
		tmp->error_found = true;
		error = get_mcc_semantic_error_struct(MCC_SC_ERROR_FUNCTION_NOT_DECLARED);
		error->sloc = &expression->node.sloc;
		error->identifier = expression->function_call_identifier->identifier;
		print_semantic_error(error, tmp->out, tmp->is_quiet);
		return;
	}
	expression->expression_type = sym->type;

	int numArgs = 0;
	if (expression->function_call_arguments) {
		numArgs++;
		struct mcc_ast_function_arguments *tmp = expression->function_call_arguments;
		while (tmp->next_argument) {
			numArgs++;
			tmp = tmp->next_argument;
		}
	}
	if (numArgs != sym->numArgs) {
		tmp->error_found = true;
		error = get_mcc_semantic_error_struct(MCC_SC_ERROR_NUMBER_ARGUMENTS);
		error->sloc = &expression->node.sloc;
		error->func_identifier = expression->function_call_identifier->identifier;
		error->expArgs = sym->numArgs;
		error->gotArgs = numArgs;
		print_semantic_error(error, tmp->out, tmp->is_quiet);
	}
}

static struct mcc_symbol *check_identifier(struct mcc_ast_source_location *sloc,
                                           struct temp_create_symbol_table *temp,
                                           struct mcc_ast_identifier *id,
                                           bool print_error)
{
	assert(sloc);
	assert(temp);
	assert(id);

	struct mcc_symbol_table *symbol_table = temp->symbol_table;
	struct mcc_symbol *previous_declaration = lookup_symbol(symbol_table, id->name);
	if (previous_declaration == NULL) {
		temp->error_found = true;
		if (print_error) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_UNDEFINED_IDENTIFIER);
			error->sloc = sloc;
			error->identifier = id;
			print_semantic_error(error, temp->out, temp->is_quiet);
		}
		return NULL;
	}
	return previous_declaration;
}

static void set_expression_type(struct mcc_ast_expression *expr, struct mcc_symbol *sym)
{
	if (sym == NULL) {
		return;
	}
	if (sym->array_size != NULL && expr->array_access_exp == NULL) {
		expr->expression_type = MCC_AST_TYPE_ARRAY;
		expr->expression_array_type = sym->type;
	} else {
		expr->expression_type = sym->type;
	}
}

static void
set_arr_expression_type(struct mcc_ast_expression *expr, struct mcc_symbol *sym, struct temp_create_symbol_table *temp)
{
	if (sym == NULL) {
		return;
	}
	if (sym->array_size == NULL) {
		temp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_ARRAY_ACCESS);
		error->sloc = &expr->node.sloc;
		error->lhs_type = sym->type;
		print_semantic_error(error, temp->out, temp->is_quiet);
	} else {
		expr->expression_type = sym->type;
	}
}

static void symbol_table_expression(struct mcc_ast_expression *expr, void *data)
{
	assert(expr);
	assert(data);
	struct temp_create_symbol_table *temp = data;
	struct mcc_symbol *sym;

	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		sym = check_identifier(&expr->node.sloc, temp, expr->identifier, true);
		set_expression_type(expr, sym);
		break;
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		symbol_table_expression(expr->rhs, data);
		break;
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		symbol_table_expression(expr->lhs, data);
		symbol_table_expression(expr->rhs, data);
		break;
	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		symbol_table_expression(expr->expression, data);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		sym =
		    check_identifier(&expr->array_access_id->node.sloc, temp, expr->array_access_id->identifier, true);
		set_arr_expression_type(expr, sym, temp);
		break;
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		break;
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		sym = check_identifier(&expr->node.sloc, temp, expr->function_call_identifier->identifier, true);
		set_expression_type(expr, sym);
		break;
	}
}

static void symbol_table_statement_expression(struct mcc_ast_statement *expr, void *data)
{
	assert(expr);
	assert(data);

	symbol_table_expression(expr->expression, data);
}

static void symbol_table_assignment(struct mcc_ast_declare_assign *assignment, void *data)
{
	assert(assignment);
	assert(data);

	struct temp_create_symbol_table *temp = data;
	struct mcc_ast_identifier *id;
	if (assignment->assign_lhs->type != MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		id = assignment->assign_lhs->identifier;
	} else {
		id = assignment->assign_lhs->array_access_id->identifier;
	}

	struct mcc_symbol *previous_declaration = check_identifier(&assignment->node.sloc, temp, id, false);
	if (previous_declaration != NULL) {
		assignment->assign_lhs->expression_type = previous_declaration->type;
	}

	symbol_table_expression(assignment->assign_rhs, data);
}

static void symbol_table_if_statement(struct mcc_ast_statement *if_stmt, void *data)
{
	assert(if_stmt);
	assert(data);

	// check if condition
	symbol_table_expression(if_stmt->if_cond, data);
}

static void symbol_table_while_statement(struct mcc_ast_statement *if_stmt, void *data)
{
	assert(if_stmt);
	assert(data);
	// check if condition
	symbol_table_expression(if_stmt->while_cond, data);
}

static void symbol_table_return_statement(struct mcc_ast_statement *ret, void *data)
{
	assert(ret);
	assert(data);

	if (ret->expression != NULL)
		symbol_table_expression(ret->expression, data);
}

struct mcc_ast_visitor generate_symbol_table_visitor(struct temp_create_symbol_table *temp_st)
{
	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_PRE_ORDER,

	    .userdata = temp_st,
	    // .pre_visit_function = 1,

	    .declaration = symbol_table_declaration,
	    .assignment = symbol_table_assignment,

	    .statement_compound = symbol_table_compound_pre,
	    .statement_compound_post = symbol_table_compound_post,

	    .function = symbol_table_function_def,
	    .function_compound = post_function_def,

	    .expression_call = symbol_table_function_call,
	    .expression_identifier = symbol_table_expression,
	    .expression_array_access = symbol_table_expression,

	    .statement_if = symbol_table_if_statement,
	    .statement_while = symbol_table_while_statement,
	    .statement_return = symbol_table_return_statement,
	    .statement_expression = symbol_table_statement_expression,
	};
}

void mcc_delete_symbol_table(struct mcc_symbol_table *symbol_table)
{
	if (symbol_table == NULL) {
		return;
	}

	if (symbol_table->next != NULL) {
		mcc_delete_symbol_table(symbol_table->next);
	}

	if (symbol_table->sub_tables != NULL && symbol_table->sub_tables->head != NULL) {
		mcc_delete_symbol_table(symbol_table->sub_tables->head);
	}
	mcc_delete_symbol(symbol_table->symbols->head);
	free(symbol_table->symbols);
	free(symbol_table->sub_tables);
	free(symbol_table);
}

void mcc_delete_symbol(struct mcc_symbol *symbol)
{
	if (symbol == NULL) {
		return;
	}

	if (symbol->next_symbol != NULL) {
		mcc_delete_symbol(symbol->next_symbol);
	}

	mcc_delete_argument_typelist(symbol->argument_type_list);

	free(symbol->identifier->name);
	free(symbol->identifier);
	free(symbol);
}

void mcc_delete_argument_typelist(struct argument_type_list *list)
{
	if (list == NULL) {
		return;
	}
	mcc_delete_argument_typelist(list->next_type);
	free(list);
}

char *string_repeat(int n, const char *s)
{
	size_t slen = strlen(s);
	char *dest = malloc(n * slen + 1);

	int i;
	char *p;
	for (i = 0, p = dest; i < n; ++i, p += slen) {
		memcpy(p, s, slen);
	}
	*p = '\0';
	return dest;
}

void mcc_print_symbol_table(FILE *out, struct mcc_symbol_table *symbol_table, int indent)
{
	assert(out);

	if (!symbol_table) {
		return;
	}

	char *indention = string_repeat(indent, "\t");

	fprintf(out, "\n%s[symbol_table ", indention);
	if (symbol_table->sloc != NULL) {
		fprintf(out, "%s %d:%d", symbol_table->label, symbol_table->sloc->start_line,
		        symbol_table->sloc->end_col);
	} else {
		fprintf(out, "%s", symbol_table->label);
	}

	fprintf(out, "]\n%sname\t\t|\ttype\n%s-----------------------------\n", indention, indention);

	if (symbol_table->symbols != NULL) {
		struct mcc_symbol *current_symbol = symbol_table->symbols->head;
		while (current_symbol != NULL) {
			fprintf(out, "%s%s", indention, current_symbol->identifier->name);
			fprintf(out, "\t\t|\t");
			fprintf(out, "%s", get_type_string(current_symbol->type));
			fprintf(out, "\n");

			current_symbol = current_symbol->next_symbol;
		}
	}

	if (symbol_table->sub_tables != NULL && symbol_table->sub_tables->head != NULL) {
		mcc_print_symbol_table(out, symbol_table->sub_tables->head, indent + 1);
	}

	if (symbol_table->next != NULL) {
		mcc_print_symbol_table(out, symbol_table->next, indent);
	}

	free(indention);
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

void add_child_symbol_table(struct mcc_symbol_table *parent, struct mcc_symbol_table *table)
{
	if (parent->sub_tables == NULL) {
		struct mcc_symbol_table_list *symbol_table_list = malloc(sizeof(*symbol_table_list));
		if (!symbol_table_list) {
			return;
		}
		symbol_table_list->head = table;
		parent->sub_tables = symbol_table_list;
		return;
	}
	struct mcc_symbol_table *current = parent->sub_tables->head;

	while (current->next != NULL) {
		current = current->next;
	}

	current->next = table;
}