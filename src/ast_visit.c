#include "mcc/ast_visit.h"

#include <assert.h>
#include <stdio.h>

#define visit(node, callback, visitor) \
	do { \
		if (callback) { \
			(callback)(node, (visitor)->userdata); \
		} \
	} while (0)

#define visit_if(cond, node, callback, visitor) \
	do { \
		if (cond) { \
			visit(node, callback, visitor); \
		} \
	} while (0)

#define visit_if_pre_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_PRE_ORDER, node, callback, visitor)

#define visit_if_post_order(node, callback, visitor) \
	visit_if((visitor)->order == MCC_AST_VISIT_POST_ORDER, node, callback, visitor)

void mcc_ast_visit_program(struct mcc_ast_program *pro, struct mcc_ast_visitor *visitor)
{
	assert(pro);
	assert(visitor);

	//visit_if_pre_order(pro, visitor->program, visitor);

	switch (pro->type) {
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		visit_if_post_order(pro, visitor->expression, visitor);
		mcc_ast_visit(pro->expression, visitor);
		visit_if_post_order(pro, visitor->program, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		visit_if_post_order(pro, visitor->declaration, visitor);
		mcc_ast_visit(pro->declaration, visitor);
		visit_if_post_order(pro, visitor->declaration, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		visit_if_post_order(pro, visitor->statement, visitor);
		mcc_ast_visit(pro->statement, visitor);
		visit_if_post_order(pro, visitor->statement, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		// TODO
		break;
	}

	visit_if_post_order(pro, visitor->program, visitor);
}

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor)
{
	assert(expression);
	assert(visitor);

	visit_if_pre_order(expression, visitor->expression, visitor);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		visit_if_pre_order(expression, visitor->expression_literal, visitor);
		mcc_ast_visit(expression->literal, visitor);
		visit_if_post_order(expression, visitor->expression_literal, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		visit_if_pre_order(expression, visitor->expression_binary_op, visitor);
		mcc_ast_visit(expression->lhs, visitor);
		mcc_ast_visit(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_binary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		visit_if_pre_order(expression, visitor->expression_unary_op, visitor);
		mcc_ast_visit(expression->rhs, visitor);
		visit_if_post_order(expression, visitor->expression_unary_op, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		visit_if_pre_order(expression, visitor->expression_parenth, visitor);
		mcc_ast_visit(expression->expression, visitor);
		visit_if_post_order(expression, visitor->expression_parenth, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		visit(expression, visitor->expression_identifier, visitor);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		visit_if_pre_order(expression, visitor->expression_array_access, visitor);
		mcc_ast_visit_expression(expression->array_access_id, visitor);
		mcc_ast_visit_expression(expression->array_access_exp, visitor);
		visit_if_post_order(expression, visitor->expression_array_access, visitor);
		break;
	}

	visit_if_post_order(expression, visitor->expression, visitor);
}

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor)
{
	assert(literal);
	assert(visitor);

	visit_if_pre_order(literal, visitor->literal, visitor);

	switch (literal->type) {
	case MCC_AST_LITERAL_TYPE_INT:
		visit(literal, visitor->literal_int, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_FLOAT:
		visit(literal, visitor->literal_float, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_BOOL:
		visit(literal, visitor->literal_bool, visitor);
		break;

	case MCC_AST_LITERAL_TYPE_STRING:
		visit(literal, visitor->literal_string, visitor);
		break;
	}

	visit_if_post_order(literal, visitor->literal, visitor);
}

void mcc_ast_visit_declare_assign(struct mcc_ast_declare_assign *dec, struct mcc_ast_visitor *visitor)
{
	assert(dec);
	assert(visitor);

	switch (dec->type) {
	case MCC_AST_TYPE_DECLARATION:
		visit(dec, visitor->declaration, visitor);
		break;
	case MCC_AST_TYPE_ASSIGNMENT:
		visit(dec, visitor->assignment, visitor);
		break;
	}
}

void mcc_ast_visit_statement(struct mcc_ast_statement *stat, struct mcc_ast_visitor *visitor)
{
	assert(stat);
	assert(visitor);

	visit_if_pre_order(stat, visitor->statement, visitor);

	switch (stat->type) {
	case MCC_AST_STATEMENT_EXPRESSION:
		visit(stat, visitor->expression, visitor);
		break;
	case MCC_AST_STATEMENT_ASSIGNMENT:
		visit(stat, visitor->assignment, visitor);
		break;
	case MCC_AST_STATEMENT_DECLARATION:
		visit(stat, visitor-> declaration, visitor);
		break;
	case MCC_AST_STATEMENT_RETURN:
		visit(stat, visitor-> statement_return, visitor);
		break;
	case MCC_AST_STATEMENT_IF:
		visit(stat, visitor->statement_if, visitor);
		break;
	case MCC_AST_STATEMENT_WHILE:
		visit(stat, visitor->statement_while, visitor);
		break;
	case MCC_AST_STATEMENT_COMPOUND:
		visit(stat, visitor->statement_compound, visitor);
		break;
	}

	visit_if_post_order(stat, visitor->statement, visitor);
}

