#include "mcc/type_checking.h"
#include "mcc/symbol_table.h"
#include <stdlib.h>

static void check_assignment(struct mcc_ast_declare_assign *declare_assign, void *data)
{
	assert(declare_assign);
	assert(data);

	struct mcc_type_checking *type_checking = data;
	char *id = declare_assign->assign_lhs->identifier->name;
	struct mcc_symbol *symbol = lookup_symbol_in_scope(type_checking->symbol_table, id); // TODO: returns null
	
	struct mcc_ast_expression *rhs = declare_assign->assign_rhs;

	if (symbol->type != rhs->expression_type) {
		// TODO Andi -> error type invalid assignment...type is there, only check print and implement
		printf("error, expected type '%d', but got type '%d'\n", symbol->type, rhs->expression_type);
	}
}

static void check_function_return(struct mcc_ast_statement *ret_stmt, void *data)
{
	assert(ret_stmt);
	assert(data);

	struct mcc_type_checking *type_check = data;

	enum mcc_ast_type ret_type = ret_stmt->expression->expression_type;
	enum mcc_ast_type func_type = type_check->current_function->func_type;

	if (ret_type != func_type) {
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_RETURN_TYPE);
		error->sloc = &ret_stmt->node.sloc;
		error->ret_type = ret_type;
		error->func_type = func_type;
		print_semantic_error(error);
	}
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

	default: {
			//todo test
			struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_LITERAL_VOID);
			error->sloc = &expr->node.sloc;
			error->identifier = expr->identifier;
			break;
		}
	}
}

static void check_arithmetic_ops(struct mcc_ast_expression *bin_expr){
	if (bin_expr->expression_type != MCC_AST_TYPE_INT && bin_expr->expression_type != MCC_AST_TYPE_FLOAT){
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_AR_OPERATION);
		error->sloc = &bin_expr->node.sloc;
		error->expr_type = bin_expr->expression_type;
		print_semantic_error(error);
	}
}

static void check_logical_ops(struct mcc_ast_expression *bin_expr){
	if (bin_expr->expression_type != MCC_AST_TYPE_BOOL){
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_LOG_OPERATION);
		error->sloc = &bin_expr->node.sloc;
		error->expr_type = bin_expr->expression_type;
		print_semantic_error(error);
	}
}

static void check_expression_binary(struct mcc_ast_expression *bin_expr, void *data){
	assert(bin_expr);
	assert(data);

	struct mcc_sc_type_checking *type_checking = data;
	enum mcc_ast_type lhs_type = bin_expr->lhs->expression_type;
	enum mcc_ast_type rhs_type = bin_expr->rhs->expression_type;

	if (lhs_type != rhs_type){
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_BIN_OPERATION);
		error->sloc = &bin_expr->node.sloc;
		error->bin_expr = bin_expr;
		print_semantic_error(error);
	}

	bin_expr->expression_type = lhs_type;

	switch (bin_expr->op)
	{
		case MCC_AST_BINARY_OP_ADD:
		case MCC_AST_BINARY_OP_SUB:
		case MCC_AST_BINARY_OP_MUL:
		case MCC_AST_BINARY_OP_DIV:
		case MCC_AST_BINARY_OP_GE:
		case MCC_AST_BINARY_OP_SE:
		case MCC_AST_BINARY_OP_GT:
		case MCC_AST_BINARY_OP_ST:
			check_arithmetic_ops(bin_expr);
			break;
		case MCC_AST_BINARY_OP_LAND:
		case MCC_AST_BINARY_OP_LOR:
			check_logical_ops(bin_expr);
			break;
		default:
			// eq and neq allowed on all types
			break;
	}

}

static struct mcc_ast_visitor type_checking_visitor(void *data)
{

	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_POST_ORDER,

	    .userdata = data,

	    .assignment = check_assignment,
	    .expression_literal = check_expression_literal,
		.expression_binary_op = check_expression_binary,

	    .statement_return = check_function_return,
	};
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

	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST:
		{
			struct mcc_ast_func_list *list = program->function_list;
			while(list != NULL){
				type_checking->current_function = program->function_list->function;
				struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
				mcc_ast_visit_function(program->function, &visitor);
				list = list->next_function;
			}
		}
		break;
	}
	
	free(type_checking);
}
