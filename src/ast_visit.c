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

	switch (pro->type) {
	case MCC_AST_PROGRAM_TYPE_EXPRESSION:
		mcc_ast_visit_expression(pro->expression, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_DECLARATION:
		mcc_ast_visit_declare_assign(pro->declaration, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_STATEMENT:
		mcc_ast_visit_statement(pro->statement, visitor);
		break;
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		mcc_ast_visit_function(pro->function, visitor);
		break;
	}
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
		visit_if_pre_order(dec, visitor->declaration, visitor);
		mcc_ast_visit(dec->declare_id, visitor);
		visit_if_post_order(dec, visitor->declaration, visitor);
		break;
	case MCC_AST_TYPE_ASSIGNMENT:
		visit_if_pre_order(dec, visitor->assignment, visitor);
		mcc_ast_visit(dec->assign_lhs, visitor);
		mcc_ast_visit(dec->assign_rhs, visitor);
		visit_if_post_order(dec, visitor->assignment, visitor);
		break;
	}
}

void mcc_ast_visit_statement_expression(struct mcc_ast_statement *exp_stmt, struct mcc_ast_visitor *visitor)
{
	assert(exp_stmt);
	assert(visitor);

	visit_if_pre_order(exp_stmt, visitor->statement_expression, visitor);
	mcc_ast_visit_expression(exp_stmt->expression, visitor);
	visit_if_post_order(exp_stmt, visitor->statement_expression, visitor);
}

void mcc_ast_visit_statement_declaration(struct mcc_ast_statement *dec_stmt, struct mcc_ast_visitor *visitor)
{
	assert(dec_stmt);
	assert(visitor);

	visit_if_pre_order(dec_stmt, visitor->statement_declaration, visitor);
	mcc_ast_visit(dec_stmt->declare_assign, visitor);
	visit_if_post_order(dec_stmt, visitor->statement_declaration, visitor);
}

void mcc_ast_visit_statement_assignment(struct mcc_ast_statement *ass_stmt, struct mcc_ast_visitor *visitor)
{
	assert(ass_stmt);
	assert(visitor);

	visit_if_pre_order(ass_stmt, visitor->statement_assignment, visitor);

	mcc_ast_visit_declare_assign(ass_stmt->declare_assign, visitor);

	visit_if_post_order(ass_stmt, visitor->statement_assignment, visitor);
}

void mcc_ast_visit_statement_if(struct mcc_ast_statement *if_stmt, struct mcc_ast_visitor *visitor)
{
	assert(if_stmt);
	assert(visitor);

	visit_if_pre_order(if_stmt, visitor->statement_if, visitor);

	// if
	mcc_ast_visit_expression(if_stmt->if_cond, visitor);
	// then
	mcc_ast_visit_statement(if_stmt->if_stat, visitor);
	// else
	if (if_stmt->else_stat != NULL)
		mcc_ast_visit_statement(if_stmt->else_stat, visitor);

	visit_if_post_order(if_stmt, visitor->statement_if, visitor);
}

void mcc_ast_visit_statement_return(struct mcc_ast_statement *ret_stmt, struct mcc_ast_visitor *visitor)
{
	assert(ret_stmt);
	assert(visitor);

	visit_if_pre_order(ret_stmt, visitor->statement_return, visitor);
	mcc_ast_visit_expression(ret_stmt->expression, visitor);
	visit_if_post_order(ret_stmt, visitor->statement_return, visitor);
}

void mcc_ast_visit_statement_while(struct mcc_ast_statement *while_stmt, struct mcc_ast_visitor *visitor)
{
	assert(while_stmt);
	assert(visitor);

	visit_if_pre_order(while_stmt, visitor->statement_while, visitor);

	// run condition
	mcc_ast_visit_expression(while_stmt->while_cond, visitor);
	// loop body
	mcc_ast_visit_statement(while_stmt->while_stat, visitor);

	visit_if_post_order(while_stmt, visitor->statement_while, visitor);
}

void mcc_ast_visit_statement_compound(struct mcc_ast_statement *comp_stmt, struct mcc_ast_visitor *visitor)
{
	assert(comp_stmt);
	assert(visitor);

	visit_if_pre_order(comp_stmt, visitor->statement_compound, visitor);

	struct mcc_ast_statement_list *stmt = comp_stmt->compound;
	while (stmt != NULL) {
		mcc_ast_visit_statement(stmt->statement, visitor);
		stmt = stmt->next_statement;
	}

	visit_if_post_order(comp_stmt, visitor->statement_compound, visitor);
}

void mcc_ast_visit_statement(struct mcc_ast_statement *stat, struct mcc_ast_visitor *visitor)
{	
	assert(stat);
	assert(visitor);
	switch (stat->type) {
	case MCC_AST_STATEMENT_EXPRESSION:
		mcc_ast_visit_statement_expression(stat, visitor);
		break;
	case MCC_AST_STATEMENT_ASSIGNMENT:
		mcc_ast_visit_statement_assignment(stat, visitor);
		break;
	case MCC_AST_STATEMENT_DECLARATION:
		mcc_ast_visit_statement_declaration(stat, visitor);
		break;
	case MCC_AST_STATEMENT_RETURN:
		mcc_ast_visit_statement_return(stat, visitor);
		break;
	case MCC_AST_STATEMENT_IF:
		mcc_ast_visit_statement_if(stat, visitor);
		break;
	case MCC_AST_STATEMENT_WHILE:
		mcc_ast_visit_statement_while(stat, visitor);
		break;
	case MCC_AST_STATEMENT_COMPOUND:
		mcc_ast_visit_statement_compound(stat, visitor);
		break;
	}
}

void mcc_ast_visit_function(struct mcc_ast_func_definition *function, struct mcc_ast_visitor *visitor)
{
	assert(function);
	assert(visitor);

	visit_if_pre_order(function, visitor->function, visitor);

	// identifier
	mcc_ast_visit_expression(function->func_identifier, visitor);

	// compund
	mcc_ast_visit_statement_compound(function->func_compound, visitor);

	visit_if_post_order(function, visitor->function, visitor);
}
