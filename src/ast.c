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

struct mcc_ast_parameter *mCC_ast_get_new_parameter_struct()
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

struct mcc_ast_expression *mcc_ast_new_expression_identifier(char *identifier, struct mcc_ast_source_location *location)
{
	assert(identifier);
	assert(location);

	struct mcc_ast_expression *expr = mcc_ast_get_new_expression_struct();
	expr->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;

	struct mcc_ast_identifier *id = mcc_ast_get_new_identifier_struct();
	id->name = identifier;
	id->sym_declaration = NULL;

	expr->identifier = id;
	mcc_ast_add_sloc(&expr->node, location);
	mcc_ast_add_sloc(&expr->identifier->node, location);

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

// ------------------------------------------------------------------- Functions

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declare_assign *declaration,
                                                struct mcc_ast_parameter *parameter,
                                                struct mcc_ast_source_location *location)
{
	assert(declaration);
	assert(location);

	struct mcc_ast_parameter *param = mCC_ast_get_new_parameter_struct();
	param->parameter = declaration;
	param->next_parameter = parameter;

	mcc_ast_add_sloc(&param->node, location);

	return param;
}

// ------------------------------------------------------------------- Identifier

void mcc_ast_delete_identifier(struct mcc_ast_identifier *id)
{
	assert(id);
	free(id->name);
	free(id);
}

// ------------------------------------------------------------------- Declaration

struct mcc_ast_declare_assign *mcc_ast_new_declaration(enum mcc_ast_type type,
                                                       struct mcc_ast_expression *identifier,
                                                       long literal,
                                                       int literal_flag,
                                                       struct mcc_ast_source_location *location)
{
	assert(identifier);

	struct mcc_ast_declare_assign *decl = mcc_ast_get_new_declaration_struct();

	decl->type = MCC_AST_TYPE_DECLARATION;
	decl->declare_type = type;
	decl->declare_id = identifier;
	decl->sym_declaration = NULL;

	if (literal_flag) {
		long *temp = malloc(sizeof(long));
		*temp = literal;
		decl->declare_array_size = temp;
	} else {
		decl->declare_array_size = NULL;
	}

	mcc_ast_add_sloc(&decl->node, location);

	return decl;
}

struct mcc_ast_declare_assign *mcc_ast_new_assignment(struct mcc_ast_expression *identifier,
                                                      struct mcc_ast_expression *expression,
                                                      struct mcc_ast_expression *array_expression,
                                                      struct mcc_ast_source_location *location,
                                                      struct mcc_ast_source_location *op_location,
                                                      struct mcc_ast_source_location *array_location)
{

	assert(identifier);
	assert(expression);

	struct mcc_ast_declare_assign *decl = mcc_ast_get_new_declaration_struct();
	decl->type = MCC_AST_TYPE_ASSIGNMENT;
	decl->assign_rhs = expression;

	struct mcc_ast_source_location *sloc_op = mcc_ast_get_new_source_location_struct();
	mcc_ast_set_sloc(sloc_op, op_location);
	decl->assign_sloc = sloc_op;

	struct mcc_ast_expression *arr_expr = identifier;
	if (array_expression) {
		arr_expr = mcc_ast_get_new_expression_struct();
		arr_expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS;
		arr_expr->array_access_exp = array_expression;
		arr_expr->array_access_id = identifier;
		mcc_ast_add_sloc(&arr_expr->node, &identifier->node.sloc);

		struct mcc_ast_source_location *sloc_array = mcc_ast_get_new_source_location_struct();
		mcc_ast_set_sloc(sloc_array, array_location);
		arr_expr->array_sloc = sloc_array;
	}

	decl->assign_lhs = arr_expr;
	mcc_ast_add_sloc(&decl->node, location);

	return decl;
}
