#include "mcc/type_checking.h"
#include "mcc/symbol_table.h"
#include <stdlib.h>

static void check_assignment(struct mcc_ast_declare_assign *declare_assign, void *data)
{
	assert(declare_assign);
	assert(data);

	struct mcc_type_checking *type_checking = data;

	struct mcc_ast_expression *lhs = declare_assign->assign_lhs;
	struct mcc_ast_expression *rhs = declare_assign->assign_rhs;

	if (lhs->expression_type != rhs->expression_type) {
		// TODO Andi -> error type invalid assignment
		printf("error, expected type '%d', but got type '%d'\n", lhs->expression_type, rhs->expression_type);
	}
}

static void check_function_return(struct mcc_ast_statement *ret_stmt, void *data)
{
	assert(ret_stmt);
	assert(data);

	struct mcc_type_checking *type_check = data;

	enum mcc_ast_type ret_type;
	if (ret_stmt->expression == NULL) {
		ret_type = MCC_AST_TYPE_VOID;
	} else {
		ret_type = ret_stmt->expression->expression_type;
	}
	enum mcc_ast_type func_type = type_check->current_function->func_type;

	if (ret_type != func_type) {
		// TODO Andi -> error type invalid return type
		printf("error, expected type '%d', but got type '%d'\n", func_type, ret_type);
	}
}

static void check_eval_expression(struct mcc_ast_expression *expr)
{

	if (expr == NULL) {
		return;
	}

	printf("%d\n", expr->type);

	if (expr->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (expr->expression_type != MCC_AST_TYPE_BOOL) {
			printf("error, literal type '%d' not allowed in eval condition\n", expr->expression_type);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		if (expr->expression_type != MCC_AST_TYPE_BOOL) {
			printf("error, variable type '%d' not allowed in eval condition\n", expr->identifier->type);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_PARENTH) {
		return check_eval_expression(expr->expression);
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		// TODO check array type
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
		enum mcc_ast_binary_op op_type = expr->op;
		switch (expr->op) {
		case MCC_AST_BINARY_OP_ST:
			break;
		case MCC_AST_BINARY_OP_GT:
			break;
		case MCC_AST_BINARY_OP_SE:
			break;
		case MCC_AST_BINARY_OP_GE:
			break;
		case MCC_AST_BINARY_OP_LAND:
			break;
		case MCC_AST_BINARY_OP_LOR:
			break;
		case MCC_AST_BINARY_OP_EQ:
			break;
		case MCC_AST_BINARY_OP_NEQ:
			break;
		default:
			printf("error, expression type '%d' not allowed in eval condition\n", expr->op);
			break;
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_UNARY_OP) {
		enum mcc_ast_unary_op u_op_type = expr->u_op;
		if (u_op_type != MCC_AST_UNARY_OP_NOT) {
			printf("error, expression type '%d' not allowed in eval condition\n", u_op_type);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL) {
		// TODO check return value
		return;
	}

	printf("error, missing eval condition\n");
}

static void check_statement_if(struct mcc_ast_statement *if_stmt, void *data)
{
	assert(if_stmt);
	assert(if_stmt->if_cond);
	assert(data);

	check_eval_expression(if_stmt->if_cond);
}

static void check_statement_while(struct mcc_ast_statement *while_stmt, void *data)
{
	assert(while_stmt);
	assert(while_stmt->while_cond);
	assert(data);

	check_eval_expression(while_stmt->while_cond);
}

static void set_type(struct mcc_ast_expression *expr, enum mcc_ast_type type)
{
	assert(expr);

	expr->expression_type = type;
}

static void check_expression_literal(struct mcc_ast_expression *expr, void *data)
{
	assert(expr);
	assert(data);

	switch (expr->literal->type) {

	case MCC_AST_LITERAL_TYPE_BOOL:
		set_type(expr, MCC_AST_TYPE_BOOL);
		break;

	case MCC_AST_LITERAL_TYPE_INT:
		set_type(expr, MCC_AST_TYPE_INT);
		break;

	case MCC_AST_LITERAL_TYPE_FLOAT:
		set_type(expr, MCC_AST_TYPE_FLOAT);
		break;

	case MCC_AST_LITERAL_TYPE_STRING:
		set_type(expr, MCC_AST_TYPE_STRING);
		break;

	default:
		printf("error, literal cant be type void");
		break;
	}
}

static void check_arithmetic_ops(enum mcc_ast_type type)
{
	if (type != MCC_AST_TYPE_INT && type != MCC_AST_TYPE_FLOAT) {
		// TODO Andi -> error arithmetic ops only allowed on ints and floats
		printf("arithmetic operations not allowed on type '%d'\n", type);
	}
}

static void check_logical_ops(enum mcc_ast_type type)
{
	if (type != MCC_AST_TYPE_BOOL) {
		// TODO Andi -> error logical ops only allowed on bools
		printf("logical operations not allowed on type '%d'\n", type);
	}
}

static void check_expression_binary(struct mcc_ast_expression *bin_expr, void *data)
{
	assert(bin_expr);
	assert(data);

	struct mcc_sc_type_checking *type_checking = data;
	enum mcc_ast_type lhs_type = bin_expr->lhs->expression_type;
	enum mcc_ast_type rhs_type = bin_expr->rhs->expression_type;

	if (lhs_type != rhs_type) {
		// TODO Andi binary operation between two different types
		printf("operation '%d' not allowed on type '%d' and '%d'\n", bin_expr->op, lhs_type, rhs_type);
	}

	bin_expr->expression_type = lhs_type;

	switch (bin_expr->op) {
	case MCC_AST_BINARY_OP_ADD:
	case MCC_AST_BINARY_OP_SUB:
	case MCC_AST_BINARY_OP_MUL:
	case MCC_AST_BINARY_OP_DIV:
	case MCC_AST_BINARY_OP_GE:
	case MCC_AST_BINARY_OP_SE:
	case MCC_AST_BINARY_OP_GT:
	case MCC_AST_BINARY_OP_ST:
		check_arithmetic_ops(bin_expr->expression_type);
		break;
	case MCC_AST_BINARY_OP_LAND:
	case MCC_AST_BINARY_OP_LOR:
		check_logical_ops(bin_expr->expression_type);
		break;
	default:
		// eq and neq allowed on all types
		break;
	}
}

static void check_expression_unary(struct mcc_ast_expression *expr, void *data)
{
	assert(expr);
	assert(data);

	switch (expr->u_op) {
	case MCC_AST_UNARY_OP_MINUS:
		check_arithmetic_ops(expr->expression_type);
		break;

	case MCC_AST_UNARY_OP_NOT:
		check_logical_ops(expr->expression_type);
		break;
	}
}

static struct mcc_ast_visitor type_checking_visitor(void *data)
{

	return (struct mcc_ast_visitor){.traversal = MCC_AST_VISIT_DEPTH_FIRST,
	                                .order = MCC_AST_VISIT_POST_ORDER,

	                                .userdata = data,
	                                .assignment = check_assignment,
	                                .expression_literal = check_expression_literal,
	                                .expression_binary_op = check_expression_binary,
	                                .expression_unary_op = check_expression_unary,
	                                .statement_return = check_function_return,
	                                .statement_if = check_statement_if,
	                                .statement_while = check_statement_while};
}

void mcc_check_types(struct mcc_ast_program *program, struct mcc_symbol_table *symbol_table)
{
	assert(program);
	assert(symbol_table);

	struct mcc_type_checking *type_checking = malloc(sizeof(*type_checking));
	if (!type_checking) {
		return;
	}
	type_checking->symbol_table = symbol_table;

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		type_checking->current_function = program->function;
		struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
		mcc_ast_visit_function(program->function, &visitor);
		break;

	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST: {
		struct mcc_ast_func_list *list = program->function_list;
		while (list != NULL) {
			type_checking->current_function = program->function_list->function;
			struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
			mcc_ast_visit_function(type_checking->current_function, &visitor);
			list = list->next_function;
		}
	} break;
	}

	free(type_checking);
}
