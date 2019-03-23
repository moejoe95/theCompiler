#include "mcc/ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------- Generator
void mcc_ast_add_sloc(struct mcc_ast_node *node, struct mcc_ast_source_location *location)
{
	node->sloc.start_line = location->start_line;
	node->sloc.start_col = location->start_col;
	node->sloc.end_line = location->end_line;
	node->sloc.end_col = location->end_col;
	// node->sloc.filename = location->filename;
}

void mcc_ast_set_sloc(struct mcc_ast_source_location *location, struct mcc_ast_source_location *set_location)
{
	location->start_line = set_location->start_line;
	location->start_col = set_location->start_col;
	location->end_line = set_location->end_line;
	location->end_col = set_location->end_col;
	// location->filename = set_location->filename;
}

struct mcc_ast_parameter *mcc_ast_get_new_parameter_struct()
{

	struct mcc_ast_parameter *param = malloc(sizeof(*param));
	if (!param) {
		return NULL;
	}

	return param;
}

struct mcc_ast_declare_assign *mcc_ast_get_new_declaration_struct()
{
	struct mcc_ast_declare_assign *decl = malloc(sizeof(*decl));
	if (!decl) {
		return NULL;
	}

	return decl;
}

struct mcc_ast_source_location *mcc_ast_get_new_source_location_struct()
{
	struct mcc_ast_source_location *sloc = malloc(sizeof(*sloc));
	if (!sloc) {
		return NULL;
	}
	return sloc;
}

struct mcc_ast_expression *mcc_ast_get_new_expression_struct()
{
	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}
	// expr->semantic_annotation = NULL;

	return expr;
}

struct mcc_ast_identifier *mcc_ast_get_new_identifier_struct()
{
	struct mcc_ast_identifier *id = malloc(sizeof(*id));
	if (!id) {
		return NULL;
	}

	return id;
}

struct mcc_ast_literal *mcc_ast_get_new_literal_struct()
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	return lit;
}

struct mcc_ast_statement *mcc_ast_get_new_statement_struct()
{
	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if (!statement) {
		return NULL;
	}

	return statement;
}

struct mcc_ast_func_definition *mcc_ast_get_new_function_def_struct()
{
	struct mcc_ast_func_definition *func_def = malloc(sizeof(*func_def));
	if (!func_def) {
		return NULL;
	}
	func_def->semantic_annotation = NULL;
	return func_def;
}

// ---------------------------------------------------------------- Expressions

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal)
{
	assert(literal);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY_OP;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op op, struct mcc_ast_expression *rhs)
{
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->op = op;
	expr->rhs = rhs;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_identifier(char *identifier)
{
	assert(identifier);

	struct mcc_ast_expression *expr = mcc_ast_get_new_expression_struct();
	expr->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;

	struct mcc_ast_identifier *id = mcc_ast_get_new_identifier_struct();
	id->name = identifier;
	id->sym_declaration = NULL;

	expr->identifier = id;
	return expr;
}

void mcc_ast_delete_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mcc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mcc_ast_delete_expression(expression->lhs);
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mcc_ast_delete_expression(expression->expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		mcc_ast_delete_expression(expression->expression);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		mcc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		free(expression->array_sloc);
		mcc_ast_delete_expression(expression->array_access_exp);
		mcc_ast_delete_expression(expression->array_access_id);
		break;
	}

	free(expression);
}

// ------------------------------------------------------------------- Literals

struct mcc_ast_literal *mcc_ast_new_literal_int(long value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_float(double value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_bool(int value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->b_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_string(char *value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->str_value = value;
	return lit;
}

void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	assert(literal);
	free(literal);
}

// ------------------------------------------------------------------- Identifier

void mcc_ast_delete_identifier(struct mcc_ast_identifier *id)
{
	assert(id);
	free(id->name);
	free(id);
}

// ------------------------------------------------------------------- Declaration

struct mcc_ast_declare_assign *
mcc_ast_new_declaration(enum mcc_ast_type type, struct mcc_ast_expression *identifier, long literal, int literal_flag)
{
	assert(identifier);

	struct mcc_ast_declare_assign *decl = mcc_ast_get_new_declaration_struct();

	decl->type = MCC_AST_TYPE_DECLARATION;
	decl->declare_type = type;
	decl->declare_id = identifier;
	decl->declare_id->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;
	decl->sym_declaration = NULL;

	if (literal_flag) {
		long *temp = malloc(sizeof(long));
		*temp = literal;
		decl->declare_array_size = temp;
	} else {
		decl->declare_array_size = NULL;
	}

	return decl;
}

struct mcc_ast_declare_assign *mcc_ast_new_assignment(struct mcc_ast_expression *identifier,
                                                      struct mcc_ast_expression *expression,
                                                      struct mcc_ast_expression *array_expression)
{

	assert(identifier);
	assert(expression);

	struct mcc_ast_declare_assign *decl = mcc_ast_get_new_declaration_struct();
	decl->type = MCC_AST_TYPE_ASSIGNMENT;
	decl->assign_rhs = expression;

	// struct mcc_ast_source_location *sloc_op = mcc_ast_get_new_source_location_struct();
	// mcc_ast_set_sloc(sloc_op, op_location);
	// decl->assign_sloc = sloc_op;

	struct mcc_ast_expression *arr_expr = identifier;
	if (array_expression) {
		arr_expr = mcc_ast_get_new_expression_struct();
		arr_expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS;
		arr_expr->array_access_exp = array_expression;
		arr_expr->array_access_id = identifier;
		mcc_ast_add_sloc(&arr_expr->node, &identifier->node.sloc);

		// struct mcc_ast_source_location *sloc_array = mcc_ast_get_new_source_location_struct();
		// mcc_ast_set_sloc(sloc_array, array_location);
		// arr_expr->array_sloc = sloc_array;
	}

	decl->assign_lhs = arr_expr;
	// mcc_ast_add_sloc(&decl->node, location);

	return decl;
}

void mcc_ast_delete_declare_assign(struct mcc_ast_declare_assign *declaration)
{
	assert(declaration);

	switch (declaration->type) {

	case MCC_AST_TYPE_DECLARATION:
		mcc_ast_delete_expression(declaration->declare_id);
		free(declaration->declare_array_size);
		break;

	case MCC_AST_TYPE_ASSIGNMENT:
		// TODO
		break;
	}
	free(declaration);
}

// ------------------------------------------------------------------- Toplevel

struct mcc_ast_program *mcc_ast_new_program(void *program, enum mcc_ast_program_type type)
{

	struct mcc_ast_program *pro = malloc(sizeof(*pro));
	if (!pro) {
		return NULL;
	}

	pro->type = type;

	switch (type) {
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		pro->expression = (struct mcc_ast_expression *)program;
		break;

	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		pro->declaration = (struct mcc_ast_declare_assign *)program;
		break;

	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		pro->statement = (struct mcc_ast_statement *)program;
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		pro->function = (struct mcc_ast_func_definition *)program;
		break;

		// TODO
	}

	return pro;
}

void mcc_ast_delete_program(struct mcc_ast_program *program)
{
	assert(program);

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		// TODO
		break;

	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		// TODO
		break;

	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		// TODO
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		// TODO
		break;

		// TODO
	}
}

// ------------------------------------------------------------------- Statements

struct mcc_ast_statement *
mcc_ast_new_statement_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_statement *stat = mcc_ast_get_new_statement_struct();
	stat->type = MCC_AST_STATEMENT_EXPRESSION;
	stat->expression = expression;

	// mcc_ast_add_sloc(&stat->node, location);

	return stat;
}

struct mcc_ast_statement *
mcc_ast_new_statement_assignment(struct mcc_ast_declare_assign *assignment)
{
	assert(assignment);

	struct mcc_ast_statement *assign = mcc_ast_get_new_statement_struct();
	assign->type = MCC_AST_STATEMENT_ASSIGNMENT;
	assign->declare_assign = assignment;

	// mcc_ast_add_sloc(&assign->node, location);

	return assign;
}

struct mcc_ast_statement *
mcc_ast_new_statement_declaration(struct mcc_ast_declare_assign *declaration)
{
	assert(declaration);

	struct mcc_ast_statement *dec = mcc_ast_get_new_statement_struct();
	dec->type = MCC_AST_STATEMENT_DECLARATION;
	dec->declare_assign = declaration;

	// mcc_ast_add_sloc(&dec->node, location);

	return dec;
}

void mcc_ast_delete_statement(struct mcc_ast_statement *statement)
{
	assert(statement);

	switch (statement->type) {

	case MCC_AST_STATEMENT_EXPRESSION:
		mcc_ast_delete_expression(statement->expression);
		break;
	case MCC_AST_STATEMENT_RETURN:
		if (statement->expression != NULL) {
			mcc_ast_delete_expression(statement->expression);
		}
		break;

	case MCC_AST_STATEMENT_DECLARATION:
	case MCC_AST_STATEMENT_ASSIGNMENT:
		mcc_ast_delete_declare_assign(statement->declare_assign);
		break;

	case MCC_AST_STATEMENT_IF:
		mcc_ast_delete_expression(statement->if_cond);
		mcc_ast_delete_statement(statement->if_stat);
		if (statement->else_stat != NULL) {
			mcc_ast_delete_statement(statement->else_stat);
		}
		break;

	case MCC_AST_STATEMENT_WHILE:
		mcc_ast_delete_expression(statement->while_cond);
		mcc_ast_delete_statement(statement->while_sta);
		break;

	case MCC_AST_STATEMENT_COMPOUND:
		if (statement->compound != NULL) {
			mcc_ast_delete_statement_list(statement->compound);
		}
		break;
	}

	free(statement);
}

struct mcc_ast_statement *
mcc_ast_new_statement_compound(struct mcc_ast_statement_list *statement_list)
{
	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if (!statement) {
		return NULL;
	}
	statement->type = MCC_AST_STATEMENT_COMPOUND;
	statement->compound = statement_list;

	return statement;
}

struct mcc_ast_statement_list *mcc_ast_new_statement_compound_stmt(
    struct mcc_ast_statement *statement,
    struct mcc_ast_statement_list *statement_list)
{
	struct mcc_ast_statement_list *stmt = malloc(sizeof(*stmt));
	if (!stmt) {
		return NULL;
	}
	stmt->statement = statement;
	stmt->next_statement = statement_list;

	return stmt;
}

void mcc_ast_delete_statement_list(
    struct mcc_ast_statement_list *statement_list)
{
	assert(statement_list);
	if (statement_list->statement != NULL) {
		mcc_ast_delete_statement(statement_list->statement);
	}
	if (statement_list->next_statement != NULL) {
		mcc_ast_delete_statement_list(statement_list->next_statement);
	}
	free(statement_list);
}

// ------------------------------------------------------------------- Functions

struct mcc_ast_func_definition *mcc_ast_new_function(
    enum mcc_ast_type type, struct mcc_ast_expression *identifier,
    struct mcc_ast_statement *compound, struct mcc_ast_parameter *parameter)
{
	assert(identifier);
	assert(compound);

	struct mcc_ast_func_definition *func_def =
	    mcc_ast_get_new_function_def_struct();

	func_def->func_type = type;
	func_def->func_identifier = identifier;
	func_def->func_compound = compound;
	func_def->parameter_list = parameter;
	func_def->semantic_annotation = NULL;

	return func_def;
}

void mcc_ast_delete_function_def(struct mcc_ast_func_definition *func_def)
{
	assert(func_def);

	// TODO
	mcc_ast_delete_expression(func_def->func_identifier);
	if (func_def->parameter_list != NULL) {
		mcc_ast_delete_parameter(func_def->parameter_list);
	}
	if (func_def->semantic_annotation) {
		free(func_def->semantic_annotation);
	}
	mcc_ast_delete_statement(func_def->func_compound);
	free(func_def);
}

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declare_assign *declaration,
                                                struct mcc_ast_parameter *parameter)
{
	assert(declaration);

	struct mcc_ast_parameter *param = mcc_ast_get_new_parameter_struct();
	param->parameter = declaration;
	param->next_parameter = parameter;

	return param;
}

void mcc_ast_delete_parameter(struct mcc_ast_parameter *parameter)
{
	if (parameter != NULL) {
		if (parameter->next_parameter != NULL) {
			mcc_ast_delete_parameter(parameter->next_parameter);
		}
		if (parameter->parameter != NULL) {
			mcc_ast_delete_declare_assign(parameter->parameter);
		}
	}
	free(parameter);
}

