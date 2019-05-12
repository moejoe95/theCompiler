#include "mcc/ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// ------------------------------------------------------------------- Generator Utility

struct mcc_ast_expression *mcc_ast_get_new_expression_struct()
{
	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

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
	return func_def;
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

struct mcc_ast_literal *mcc_ast_get_new_literal_struct()
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	return lit;
}

void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	assert(literal);
	free(literal);
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

struct mcc_ast_expression *mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op u_op, struct mcc_ast_expression *rhs)
{
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->u_op = u_op;
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

	expr->identifier = id;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_function_call(struct mcc_ast_expression *identifier,
                                                                struct mcc_ast_function_arguments *arguments)
{
	assert(identifier);

	struct mcc_ast_expression *expr = mcc_ast_get_new_expression_struct();
	expr->type = MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL;

	expr->function_call_identifier = identifier;
	expr->function_call_arguments = arguments;

	return expr;
}

struct mcc_ast_function_arguments *mcc_ast_new_expression_argument(struct mcc_ast_expression *expression,
                                                                   struct mcc_ast_function_arguments *arguments)
{
	assert(expression);

	struct mcc_ast_function_arguments *func_arg = malloc(sizeof(*func_arg));
	if (!func_arg) {
		return NULL;
	}

	func_arg->expression = expression;
	func_arg->next_argument = arguments;

	return func_arg;
}

struct mcc_ast_expression *mcc_ast_new_expression_array_access(struct mcc_ast_expression *identifier,
                                                               struct mcc_ast_expression *expression)
{
	assert(identifier);
	assert(expression);

	struct mcc_ast_expression *expr = mcc_ast_get_new_expression_struct();
	expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS;

	expr->array_access_exp = expression;
	expr->array_access_id = identifier;

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
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		mcc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		mcc_ast_delete_expression(expression->array_access_exp);
		mcc_ast_delete_expression(expression->array_access_id);
		break;

	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		mcc_ast_delete_expression(expression->function_call_identifier);
		if (expression->function_call_arguments != NULL) {
			mcc_ast_delete_function_arguments(expression->function_call_arguments);
		}
		break;
	}

	free(expression);
}

void mcc_ast_delete_function_arguments(struct mcc_ast_function_arguments *arguments)
{
	mcc_ast_delete_expression(arguments->expression);
	if (arguments->next_argument != NULL) {
		mcc_ast_delete_function_arguments(arguments->next_argument);
	}
	free(arguments);
}

// ------------------------------------------------------------------- Identifier

void mcc_ast_delete_identifier(struct mcc_ast_identifier *id)
{
	assert(id);

	free(id);
}

// ------------------------------------------------------------------- Parameter

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

// ------------------------------------------------------------------- Declaration Assignment

struct mcc_ast_declare_assign *
mcc_ast_new_declaration(enum mcc_ast_type type, struct mcc_ast_expression *identifier, long literal, int literal_flag)
{
	assert(identifier);

	struct mcc_ast_declare_assign *decl = mcc_ast_get_new_declaration_struct();

	decl->type = MCC_AST_TYPE_DECLARATION;
	decl->declare_type = type;
	decl->declare_id = identifier;
	decl->declare_id->type = MCC_AST_EXPRESSION_TYPE_IDENTIFIER;

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

	struct mcc_ast_expression *arr_expr = identifier;
	if (array_expression) {
		arr_expr = mcc_ast_get_new_expression_struct();
		arr_expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS;
		arr_expr->array_access_exp = array_expression;
		arr_expr->array_access_id = identifier;
	}

	decl->assign_lhs = arr_expr;

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
		mcc_ast_delete_expression(declaration->assign_lhs);
		mcc_ast_delete_expression(declaration->assign_rhs);
		break;
	}
	free(declaration);
}

// ------------------------------------------------------------------- Statements

struct mcc_ast_statement *mcc_ast_new_statement_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_statement *stat = mcc_ast_get_new_statement_struct();
	stat->type = MCC_AST_STATEMENT_EXPRESSION;
	stat->expression = expression;

	return stat;
}

struct mcc_ast_statement *mcc_ast_new_statement_return_expression(struct mcc_ast_expression *expression)
{
	struct mcc_ast_statement *stat = mcc_ast_get_new_statement_struct();
	stat->type = MCC_AST_STATEMENT_RETURN;
	stat->expression = expression;

	return stat;
}

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_declare_assign *assignment)
{
	assert(assignment);

	struct mcc_ast_statement *assign = mcc_ast_get_new_statement_struct();
	assign->type = MCC_AST_STATEMENT_ASSIGNMENT;
	assign->declare_assign = assignment;

	return assign;
}

struct mcc_ast_statement *mcc_ast_new_statement_declaration(struct mcc_ast_declare_assign *declaration)
{
	assert(declaration);

	struct mcc_ast_statement *dec = mcc_ast_get_new_statement_struct();
	dec->type = MCC_AST_STATEMENT_DECLARATION;
	dec->declare_assign = declaration;

	return dec;
}

struct mcc_ast_statement *mcc_ast_new_if_stmt(struct mcc_ast_expression *expr,
                                              struct mcc_ast_statement *if_body,
                                              struct mcc_ast_statement *else_body)
{
	assert(expr);
	assert(if_body);

	struct mcc_ast_statement *if_stmt = mcc_ast_get_new_statement_struct();
	if_stmt->type = MCC_AST_STATEMENT_IF;
	if_stmt->if_cond = expr;
	if_stmt->if_stat = if_body;
	if (else_body != NULL) {
		if_stmt->else_stat = else_body;
	} else {
		if_stmt->else_stat = NULL;
	}

	return if_stmt;
}

struct mcc_ast_statement *mcc_ast_new_while_stmt(struct mcc_ast_expression *expr, struct mcc_ast_statement *while_body)
{
	assert(expr);
	assert(while_body);

	struct mcc_ast_statement *while_stmt = mcc_ast_get_new_statement_struct();
	while_stmt->type = MCC_AST_STATEMENT_WHILE;
	while_stmt->while_cond = expr;
	while_stmt->while_stat = while_body;

	return while_stmt;
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
		mcc_ast_delete_statement(statement->while_stat);
		break;

	case MCC_AST_STATEMENT_COMPOUND:
		if (statement->compound != NULL) {
			mcc_ast_delete_statement_list(statement->compound);
		}
		break;
	}

	free(statement);
}

struct mcc_ast_statement *mcc_ast_new_statement_compound(struct mcc_ast_statement_list *statement_list)
{
	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if (!statement) {
		return NULL;
	}
	statement->type = MCC_AST_STATEMENT_COMPOUND;
	statement->compound = statement_list;

	return statement;
}

struct mcc_ast_statement_list *mcc_ast_new_statement_compound_stmt(struct mcc_ast_statement *statement,
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

void mcc_ast_delete_statement_list(struct mcc_ast_statement_list *statement_list)
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

struct mcc_ast_func_definition *mcc_ast_new_function(enum mcc_ast_type type,
                                                     struct mcc_ast_expression *identifier,
                                                     struct mcc_ast_statement *compound,
                                                     struct mcc_ast_parameter *parameter)
{
	assert(identifier);
	assert(compound);
	assert(identifier);

	struct mcc_ast_func_definition *func_def = mcc_ast_get_new_function_def_struct();

	identifier->identifier->type = type;

	func_def->func_type = type;
	func_def->func_identifier = identifier;
	func_def->func_compound = compound;
	func_def->parameter_list = parameter;

	return func_def;
}

struct mcc_ast_func_list *mcc_ast_new_function_list(struct mcc_ast_func_definition *function,
                                                    struct mcc_ast_func_list *function_list)
{
	struct mcc_ast_func_list *list = malloc(sizeof(*list));
	if (!list) {
		return NULL;
	}
	list->function = function;
	list->next_function = function_list;

	return list;
}

void mcc_ast_delete_func_definition(struct mcc_ast_func_definition *func_def)
{
	assert(func_def);
	mcc_ast_delete_expression(func_def->func_identifier);
	if (func_def->parameter_list != NULL) {
		mcc_ast_delete_parameter(func_def->parameter_list);
	}
	mcc_ast_delete_statement(func_def->func_compound);
	free(func_def);
}

void mcc_ast_delete_func_list(struct mcc_ast_func_list *func_list)
{
	if (func_list->next_function != NULL) {
		mcc_ast_delete_func_list(func_list->next_function);
	}
	if (func_list->function != NULL) {
		mcc_ast_delete_func_definition(func_list->function);
	}

	free(func_list);
}

// ------------------------------------------------------------------- Program

struct mcc_ast_program *mcc_ast_new_program(void *program, enum mcc_ast_program_type type)
{
	assert(program);

	struct mcc_ast_program *pro = malloc(sizeof(*pro));
	if (!pro) {
		return NULL;
	}

	pro->type = type;

	switch (type) {
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		pro->expression = program;
		break;
	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		pro->declaration = program;
		break;
	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		pro->statement = program;
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		pro->function = program;
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST:
		pro->function_list = program;
		break;
	case MCC_AST_PROGRAM_TYPE_EMPTY:
		break;
	}

	return pro;
}

struct mcc_ast_program *mcc_ast_new_empty_program(char *filename)
{
	struct mcc_ast_program *pro = malloc(sizeof(*pro));
	if (!pro) {
		return NULL;
	}

	pro->type = MCC_AST_PROGRAM_TYPE_EMPTY;

	pro->node.sloc.end_line = 1;
	pro->node.sloc.end_col = 1;
	pro->node.sloc.filename = filename;

	return pro;
}

void mcc_ast_delete_program(struct mcc_ast_program *program)
{
	assert(program);

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		mcc_ast_delete_func_definition(program->function);
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST:
		mcc_ast_delete_func_list(program->function_list);
		break;
	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		mcc_ast_delete_declare_assign(program->declaration);
		break;
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		mcc_ast_delete_expression(program->expression);
		break;
	case MCC_AST_PROGRAM_TYPE_EMPTY:
		break;
	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		mcc_ast_delete_statement(program->statement);
		break;
	}

	free(program);
}

// ------------------------------------------------------------------- to String

char *get_bin_op_string(enum mcc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD:
		return "+";
	case MCC_AST_BINARY_OP_SUB:
		return "-";
	case MCC_AST_BINARY_OP_MUL:
		return "*";
	case MCC_AST_BINARY_OP_DIV:
		return "/";
	case MCC_AST_BINARY_OP_ST:
		return "<";
	case MCC_AST_BINARY_OP_GT:
		return ">";
	case MCC_AST_BINARY_OP_SE:
		return "<=";
	case MCC_AST_BINARY_OP_GE:
		return ">=";
	case MCC_AST_BINARY_OP_LAND:
		return "&&";
	case MCC_AST_BINARY_OP_LOR:
		return "||";
	case MCC_AST_BINARY_OP_EQ:
		return "==";
	case MCC_AST_BINARY_OP_NEQ:
		return "!=";
	}

	return "";
}

char *get_un_op_string(enum mcc_ast_unary_op op)
{
	switch (op) {
	case MCC_AST_UNARY_OP_MINUS:
		return "-";
	case MCC_AST_UNARY_OP_NOT:
		return "!";
	}

	return "";
}

char *get_type_string(enum mcc_ast_type type)
{
	switch (type) {
	case MCC_AST_TYPE_BOOL:
		return "BOOL";
	case MCC_AST_TYPE_INT:
		return "INT";
	case MCC_AST_TYPE_FLOAT:
		return "FLOAT";
	case MCC_AST_TYPE_STRING:
		return "STRING";
	case MCC_AST_TYPE_VOID:
		return "VOID";
	case MCC_AST_TYPE_ARRAY:
		return "ARRAY";
	}

	return "";
}

char *get_literal_type_string(enum mcc_ast_literal_type type)
{
	switch (type) {
	case MCC_AST_LITERAL_TYPE_INT:
		return "INT";
	case MCC_AST_LITERAL_TYPE_FLOAT:
		return "FLOAT";
	case MCC_AST_LITERAL_TYPE_BOOL:
		return "BOOL";
	case MCC_AST_LITERAL_TYPE_STRING:
		return "STRING";
	}

	return "";
}