#include "mcc/symbol_table.h"
#include "mcc/ast.h"
#include "mcc/error_handler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct argument_type_list *create_argument_type_list()
{
	struct argument_type_list *argument_type_list = malloc(sizeof(*argument_type_list));
	if (!argument_type_list) {
		return NULL;
	}
	return argument_type_list;
}

static struct mcc_symbol_table *allocate_symbol_table(struct mcc_symbol_table *symbol_table_parent, char *label)
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
	struct mcc_symbol *symbol = create_symbol_built_in(return_type, id, NULL, NULL, NULL);

	if (parameter_type != MCC_AST_TYPE_VOID) {
		symbol->numArgs = 1;
		// TODO Andreas add argument types for check
		struct argument_type_list *argument_type_list = create_argument_type_list();
		argument_type_list->type = parameter_type;
		symbol->argument_type_list;
	}

	add_symbol_to_list(temp_st->symbol_table->symbols, symbol);
}

struct mcc_symbol *create_symbol_built_in(enum mcc_ast_type type,
                                          struct mcc_ast_identifier *identifier,
                                          long *arr_size,
                                          int numArgs,
                                          struct argument_type_list *argument_type_list)
{
	struct mcc_symbol *sym = malloc(sizeof(*sym));
	if (!sym) {
		return NULL;
	}
	sym->type = type;
	sym->identifier = identifier;
	sym->array_size = arr_size;
	sym->numArgs = numArgs;
	sym->index = -1;
	sym->next_symbol = NULL;
	sym->argument_type_list = argument_type_list;

	return sym;
}

struct mcc_symbol_table *mcc_create_symbol_table(struct mcc_ast_program *program, FILE *out)
{
	if (program == NULL) {
		return NULL;
	}

	struct temp_create_symbol_table *temp_st = malloc(sizeof(*temp_st));
	temp_st->create_inner_scope = 1;
	temp_st->main_found = 0;
	temp_st->symbol_table = allocate_symbol_table(NULL, "global");
	temp_st->is_returned = 0;
	temp_st->out = out;
	temp_st->error_found = false;

	add_built_ins(temp_st);

	struct mcc_ast_visitor visitor = generate_symbol_table_visitor(temp_st);
	mcc_ast_visit(program, &visitor);

	if (!temp_st->main_found) {
		temp_st->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_NO_MAIN);
		error->sloc = &program->function_list->node.sloc;
		print_semantic_error(error, temp_st->out);
	}

	struct mcc_symbol_table *symbol_table = temp_st->symbol_table;
	if (temp_st->error_found) {
		symbol_table = NULL;
		mcc_delete_symbol_table(temp_st->symbol_table);
	}

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
		temp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_DUPLICATE_DECLARATION);
		error->sloc = &declaration->node.sloc;
		error->identifier = declaration->declare_id->identifier;
		print_semantic_error(error, temp->out);
		return;
	}

	struct mcc_symbol *symbol =
	    create_symbol_built_in(declaration->declare_type, declaration->declare_id->identifier,
	                           declaration->declare_array_size, NULL, NULL);

	add_symbol_to_list(temp->symbol_table->symbols, symbol);

	// if (!userdata->create_inner_scope && !userdata->is_duplicate) {
	// 	assert(userdata->current_function->parameter_declaration);
	// 	ARRAY_ADD(userdata->current_function->parameter_declaration, tmp);
	// }
}

static void symbol_table_compound(struct mcc_ast_statement __attribute__((unused)) * statement,
                                  void *data,
                                  enum mcc_ast_visit_order order)
{
	struct mcc_ast_symbol_table *symbol_table;
	struct temp_create_symbol_table *tmp = data;

	switch (order) {
	case MCC_AST_VISIT_PRE_ORDER:
		symbol_table = allocate_symbol_table(tmp->symbol_table, NULL);
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

	struct mcc_ast_symbol_declaration *previous_declaration = lookup_symbol_in_scope(tmp->symbol_table, func_id);
	if (previous_declaration != NULL) {
		tmp->error_found = true;
		struct mcc_semantic_error *error =
		    get_mcc_semantic_error_struct(MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION);
		error->sloc = &function->node.sloc;
		error->identifier = function->func_identifier->identifier;
		print_semantic_error(error, tmp->out);
		return;
	}

	struct argument_type_list *argument_type_list = create_argument_type_list();

	// check if non-void function returns value
	check_return(tmp, function);
	if (!tmp->is_returned) {
		tmp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_NO_RETURN);
		error->sloc = &function->node.sloc;
		error->identifier = function->func_identifier->identifier;
		print_semantic_error(error, tmp->out);
	}
	tmp->is_returned = 0;

	struct mcc_symbol_table *outer_symbol_table = tmp->symbol_table;
	struct mcc_symbol_table *symbol_table = allocate_symbol_table(tmp->symbol_table, func_id);
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

	int numArgs = 0;
	if (function->parameter_list) { // todo andi
		struct mcc_ast_parameter *param = function->parameter_list;

		do {
			numArgs++;

			printf("type %s\n",
			       get_type_string(
			           lookup_symbol_in_scope(symbol_table, param->parameter->declare_id->identifier->name)
			               ->type));

			param = param->next_parameter;
		} while (param);
	}

	// set_semantic_annotation_function_duplicate(function, 0);
	insert_symbol_function(outer_symbol_table, function, numArgs, argument_type_list);
}

void insert_symbol_function(struct mcc_symbol_table *st,
                            struct mcc_ast_func_definition *function_def,
                            int numArgs,
                            struct argument_type_list *argument_type_list)
{
	assert(function_def);
	struct mcc_symbol *sym = create_symbol_built_in(
	    function_def->func_type, function_def->func_identifier->identifier, NULL, numArgs, argument_type_list);

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
		print_semantic_error(error, tmp->out);
		return;
	}

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
		print_semantic_error(error, tmp->out);
	}
}

static struct mcc_symbol *check_identifier(struct mcc_ast_source_location *sloc,
                                           struct temp_create_symbol_table *temp,
                                           struct mcc_ast_identifier *id)
{
	assert(sloc);
	assert(temp);
	assert(id);

	struct mcc_symbol_table *symbol_table = temp->symbol_table;
	struct mcc_symbol *previous_declaration = lookup_symbol(symbol_table, id->name);
	if (previous_declaration == NULL) {
		temp->error_found = true;
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_UNDEFINED_IDENTIFIER);
		error->sloc = sloc;
		error->identifier = id;
		print_semantic_error(error, temp->out);
		return NULL;
	}
	return previous_declaration;
}

static void symbol_table_expression(struct mcc_ast_expression *expr, void *data)
{
	assert(expr);
	assert(data);
	struct temp_create_symbol_table *temp = data;

	struct mcc_symbol *sym;

	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER: {
		sym = check_identifier(&expr->node.sloc, temp, expr->identifier);
		if (sym) {
			expr->expression_type = sym->type;
		}
	} break;
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
		symbol_table_expression(expr->array_access_id, data);
		break;
	case MCC_AST_EXPRESSION_TYPE_LITERAL:

		break;
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		sym = check_identifier(&expr->node.sloc, temp, expr->function_call_identifier->identifier);
		if (sym) {
			expr->expression_type = sym->type;
		}
		break;
	}
}

static void symbol_table_statement_expression(struct mcc_ast_expression *expr, void *data)
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

	struct mcc_symbol *previous_declaration = check_identifier(&assignment->node.sloc, temp, id);
	if (previous_declaration != NULL)
		assignment->assign_lhs->expression_type = previous_declaration->type;

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

	    .statement_if = symbol_table_if_statement,
	    .statement_while = symbol_table_while_statement,
	    .statement_return = symbol_table_return_statement,
	    .statement_expression = symbol_table_statement_expression,
	};
}

void mcc_delete_symbol_table(struct mcc_symbol_table *symbol_table)
{
	if (symbol_table->next != NULL) {
		mcc_delete_symbol_table(symbol_table->next);
	}

	if (symbol_table->sub_tables != NULL && symbol_table->sub_tables->head != NULL) {
		mcc_delete_symbol_table(symbol_table->sub_tables->head);
	}
	free(symbol_table->symbols);
	free(symbol_table);
}

void mcc_print_symbol_table(FILE *out, struct mcc_symbol_table *symbol_table)
{
	assert(out);

	if (!symbol_table) {
		return;
	}

	fprintf(out, "\n[symbol_table ");
	fprintf(out, symbol_table->label);
	fprintf(out, "]\nname\t\t|\ttype\n-----------------------------\n");

	if (symbol_table->symbols != NULL) {
		struct mcc_symbol *current_symbol = symbol_table->symbols->head;
		while (current_symbol != NULL) {
			fprintf(out, current_symbol->identifier->name);
			fprintf(out, "\t\t|\t");
			fprintf(out, get_type_string(current_symbol->type));
			fprintf(out, "\n");

			current_symbol = current_symbol->next_symbol;
		}
	}

	if (symbol_table->next != NULL) {
		mcc_print_symbol_table(out, symbol_table->next);
	}

	if (symbol_table->sub_tables != NULL && symbol_table->sub_tables->head != NULL) {
		mcc_print_symbol_table(out, symbol_table->sub_tables->head);
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

void enter_scope(struct temp_create_symbol_table *tmp, struct mcc_symbol_table *symbol_table)
{
	tmp->symbol_table = symbol_table;
}

void exit_scope(struct temp_create_symbol_table *tmp)
{
	tmp->symbol_table = tmp->symbol_table->parent;
}