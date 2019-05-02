#include "mcc/type_checking.h"
#include "mcc/print_type_log.h"
#include "mcc/symbol_table.h"
#include <stdlib.h>

static void check_expression_int(struct mcc_ast_expression *expr, struct mcc_type_checking *type_checking)
{
	assert(expr);
	assert(type_checking);

	struct mcc_ast_expression *array_access_expr = expr->array_access_exp;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);
	log->sloc = &expr->node.sloc;
	log->lhs_type = array_access_expr->expression_type;

	if (array_access_expr->expression_type != MCC_AST_TYPE_INT) {
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(ERROR_TYPE_INVALID_ARRAY_ACCESS);
		error->sloc = &expr->node.sloc;
		error->lhs_type = array_access_expr->expression_type;
		print_semantic_error(error, type_checking->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_checking->tracing) {
		mcc_print_type_log_decl(type_checking->out, log, "array access");
	}
}

static void check_assignment(struct mcc_ast_declare_assign *declare_assign, void *data)
{
	assert(declare_assign);
	assert(data);

	struct mcc_type_checking *type_checking = data;

	struct mcc_ast_expression *lhs = declare_assign->assign_lhs;
	struct mcc_ast_expression *rhs = declare_assign->assign_rhs;

	struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_ASSIGNMENT);
	error->sloc = &declare_assign->node.sloc;
	error->lhs_type = lhs->expression_type;
	error->rhs_type = rhs->expression_type;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);
	log->sloc = error->sloc;

	if (lhs->expression_type != rhs->expression_type) {
		print_semantic_error(error, type_checking->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_checking->tracing) {
		log->lhs_type = lhs->expression_type;
		log->rhs_type = rhs->expression_type;
		char label[64] = {0};
		if (lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
			check_expression_int(lhs, type_checking);
			snprintf(label, sizeof(label), "assign %s[]", lhs->expression->identifier->name);
		} else {
			snprintf(label, sizeof(label), "assign %s", declare_assign->assign_lhs->identifier->name);
		}
		mcc_print_type_log_assign(type_checking->out, log, label);
	}
}

static void check_declaration(struct mcc_ast_declare_assign *declare_assign, void *data)
{
	assert(declare_assign);
	assert(data);

	struct mcc_type_checking *type_checking = data;

	if (type_checking->tracing) {
		struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);
		log->sloc = &declare_assign->node.sloc;
		log->lhs_type = declare_assign->declare_type;
		char array_len[4] = {0};
		if (declare_assign->declare_array_size) {
			snprintf(array_len, sizeof(array_len), "[%ld]", *declare_assign->declare_array_size);
		}
		char label[64] = {0};
		snprintf(label, sizeof(label), "declare %s%s", declare_assign->declare_id->identifier->name, array_len);
		mcc_print_type_log_decl(type_checking->out, log, label);
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
		if (ret_stmt->expression->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
			ret_type = ret_stmt->expression->array_access_id->expression_type;
		} else {
			ret_type = ret_stmt->expression->expression_type;
		}
	}
	enum mcc_ast_type func_type = type_check->current_function->func_type;

	struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_RETURN_TYPE);
	error->sloc = &ret_stmt->node.sloc;
	error->ret_type = ret_type;
	error->func_type = func_type;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);

	if (ret_type != func_type) {
		print_semantic_error(error, type_check->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_check->tracing) {
		log->sloc = error->sloc;
		log->lhs_type = func_type;
		log->rhs_type = ret_type;
		mcc_print_type_log_return(type_check->out, log, "return");
	}
}

static void check_eval_expression(struct mcc_ast_expression *expr, struct mcc_symbol_table *symbol_table, void *data)
{
	assert(data);
	struct mcc_type_checking *type_check = data;
	if (expr->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (expr->expression_type != MCC_AST_TYPE_BOOL) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_TYPE);
			error->sloc = &expr->node.sloc;
			error->expr_type = expr->expression_type;
			print_semantic_error(error, type_check->out);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		if (expr->expression_type != MCC_AST_TYPE_BOOL) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_TYPE);
			error->sloc = &expr->node.sloc;
			error->expr_type = expr->expression_type;
			print_semantic_error(error, type_check->out);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_PARENTH) {
		return check_eval_expression(expr->expression, symbol_table, data);
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		if (expr->array_access_id->expression_type != MCC_AST_TYPE_BOOL) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_TYPE);
			error->sloc = &expr->node.sloc;
			error->expr_type = expr->array_access_id->expression_type;
			print_semantic_error(error, type_check->out);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
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
		default: {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_BIN_EXPR);
			error->sloc = &expr->node.sloc;
			error->bin_expr = expr;
			print_semantic_error(error, type_check->out);
			break;
		}
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_UNARY_OP) {
		enum mcc_ast_unary_op u_op_type = expr->u_op;
		if (u_op_type != MCC_AST_UNARY_OP_NOT) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_UN_EXPR);
			error->sloc = &expr->node.sloc;
			error->un_expr = expr;
			print_semantic_error(error, type_check->out);
		}
		return;
	}

	if (expr->type == MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL) {
		struct mcc_symbol *symbol =
		    lookup_symbol_in_scope(symbol_table, expr->function_call_identifier->identifier->name);
		if (symbol != NULL && symbol->type != MCC_AST_TYPE_BOOL) {
			struct mcc_semantic_error *error =
			    get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_CONDITION_TYPE);
			error->sloc = &expr->node.sloc;
			error->expr_type = symbol->type;
			print_semantic_error(error, type_check->out);
		}
		return;
	}

	struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_TYPE_NO_CONDITION);
	error->sloc = &expr->node.sloc;
	print_semantic_error(error, type_check->out);
}

static void check_statement_if(struct mcc_ast_statement *if_stmt, void *data)
{
	assert(if_stmt);
	assert(if_stmt->if_cond);
	assert(data);

	struct mcc_type_checking *type_check = data;
	check_eval_expression(if_stmt->if_cond, type_check->symbol_table, data);
}

static void check_statement_while(struct mcc_ast_statement *while_stmt, void *data)
{
	assert(while_stmt);
	assert(while_stmt->while_cond);
	assert(data);

	struct mcc_type_checking *type_check = data;
	check_eval_expression(while_stmt->while_cond, type_check->symbol_table, data);
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

	struct mcc_type_checking *type_check = data;
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

	default: { // TODO test?
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_LITERAL_VOID);
		error->sloc = &expr->node.sloc;
		error->identifier = expr->identifier;
		print_semantic_error(error, type_check->out);
		break;
	}
	}
}

static void check_arithmetic_ops(struct mcc_ast_expression *bin_expr, void *data)
{
	assert(data);
	struct mcc_type_checking *type_check = data;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);

	if (bin_expr->lhs->expression_type != MCC_AST_TYPE_INT &&
	    bin_expr->lhs->expression_type != MCC_AST_TYPE_FLOAT) {
		struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_AR_OPERATION);
		error->sloc = &bin_expr->node.sloc;
		error->expr_type = bin_expr->expression_type;
		print_semantic_error(error, type_check->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_check->tracing) {
		log->sloc = &bin_expr->node.sloc;
		log->lhs_type = bin_expr->expression_type;
		if (bin_expr->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
			log->rhs_type = bin_expr->expression_type;
			mcc_print_type_log_op(type_check->out, log, "bin op");
		} else {
			mcc_print_type_log_u_op(type_check->out, log, "un op");
		}
	}
}

static void check_logical_ops(struct mcc_ast_expression *bin_expr, void *data)
{

	assert(data);
	struct mcc_type_checking *type_check = data;

	struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_LOG_OPERATION);
	error->sloc = &bin_expr->node.sloc;
	error->expr_type = bin_expr->expression_type;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);

	if (bin_expr->expression_type != MCC_AST_TYPE_BOOL) {
		print_semantic_error(error, type_check->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_check->tracing) {
		log->sloc = error->sloc;
		log->lhs_type = bin_expr->expression_type;
		if (bin_expr->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
			log->rhs_type = bin_expr->expression_type;
			mcc_print_type_log_op(type_check->out, log, "bin op");
		} else {
			mcc_print_type_log_u_op(type_check->out, log, "u op");
		}
	}
}

static void check_expression_binary(struct mcc_ast_expression *bin_expr, void *data)
{
	assert(bin_expr);
	assert(data);

	struct mcc_type_checking *type_check = data;
	enum mcc_ast_type lhs_type = bin_expr->lhs->expression_type;
	enum mcc_ast_type rhs_type = bin_expr->rhs->expression_type;

	if (bin_expr->lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS)
		lhs_type = bin_expr->lhs->array_access_id->expression_type;
	if (bin_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS)
		rhs_type = bin_expr->rhs->array_access_id->expression_type;

	struct mcc_semantic_error *error = get_mcc_semantic_error_struct(MCC_SC_ERROR_INVALID_BIN_OPERATION);
	error->sloc = &bin_expr->node.sloc;
	error->bin_expr = bin_expr;

	struct mcc_type_log *log = get_mcc_type_log_struct(MCC_TYPE_VALID);

	if (lhs_type != rhs_type) {
		print_semantic_error(error, type_check->out);
		log->status = MCC_TYPE_INVALID;
	}

	if (type_check->tracing) {
		log->sloc = error->sloc;
		log->lhs_type = lhs_type;
		log->rhs_type = rhs_type;
		mcc_print_type_log_bin(type_check->out, log, "bin op");
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
		check_arithmetic_ops(bin_expr, data);
		break;
	case MCC_AST_BINARY_OP_LAND:
	case MCC_AST_BINARY_OP_LOR:
		check_logical_ops(bin_expr, data);
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

	expr->expression_type = expr->rhs->expression_type;

	switch (expr->u_op) {
	case MCC_AST_UNARY_OP_MINUS:
		check_arithmetic_ops(expr, data);
		break;

	case MCC_AST_UNARY_OP_NOT:
		check_logical_ops(expr, data);
		break;
	}
}

static struct mcc_ast_visitor type_checking_visitor(void *data)
{

	return (struct mcc_ast_visitor){.traversal = MCC_AST_VISIT_DEPTH_FIRST,
	                                .order = MCC_AST_VISIT_POST_ORDER,

	                                .userdata = data,
	                                .declaration = check_declaration,
	                                .assignment = check_assignment,
	                                .expression_literal = check_expression_literal,
	                                .expression_binary_op = check_expression_binary,
	                                .expression_unary_op = check_expression_unary,
	                                .statement_return = check_function_return,
	                                .statement_if = check_statement_if,
	                                .statement_while = check_statement_while};
}

void mcc_check_types(struct mcc_ast_program *program, struct mcc_symbol_table *symbol_table, FILE *out, int tracing)
{
	assert(program);
	assert(symbol_table);

	struct mcc_type_checking *type_checking = malloc(sizeof(*type_checking));
	if (!type_checking) {
		return;
	}
	type_checking->symbol_table = symbol_table;
	type_checking->out = out;
	type_checking->tracing = tracing;

	if (tracing)
		mcc_print_type_log_header(out); // print logging header

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		type_checking->current_function = program->function;
		struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
		mcc_ast_visit_function(program->function, &visitor);
		break;

	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST: {
		struct mcc_ast_func_list *list = program->function_list;
		while (list != NULL) {
			type_checking->current_function = list->function;
			struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
			mcc_ast_visit_function(type_checking->current_function, &visitor);
			list = list->next_function;
		}
	} break;
	}
	mcc_print_type_log_footer(out);
	free(type_checking);
}
