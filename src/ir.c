#include "mcc/ir.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "mcc/print_ir.h"

// forward declarations
static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_line_head *head);
static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_line_head *head);
static void generate_ir_function_call(struct mcc_ast_expression *e, struct mcc_ir_line_head *head);
static void
generate_ir_expression(struct mcc_ast_expression *e, struct mcc_ir_line_head *head, enum ir_table_operation_type t);
static int hasStatementReturn(struct mcc_ast_statement *stmt);
static char *generate_ir_entity(struct mcc_ir_line_head *head, struct mcc_ast_expression *expr);

static int get_memory_size_literal_type(enum mcc_ast_literal_type type)
{
	int mem_size = 0;
	switch (type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
	case MCC_AST_LITERAL_TYPE_INT:
	case MCC_AST_LITERAL_TYPE_STRING:
		mem_size = 1;
		break;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		mem_size = 2;
		break;
	default:
		mem_size = 0;
		break;
	}
	return mem_size;
}

static int get_memory_size_type(enum mcc_ast_type type)
{
	int mem_size = 0;
	switch (type) {
	case MCC_AST_TYPE_BOOL:
	case MCC_AST_TYPE_INT:
	case MCC_AST_TYPE_STRING:
		mem_size = 1;
		break;
	case MCC_AST_TYPE_FLOAT:
		mem_size = 2;

	default:
		break;
	}
	return mem_size;
}

static struct mcc_ir_line *create_new_ir_line()
{
	struct mcc_ir_line *table = malloc(sizeof(*table));
	if (!table) {
		return NULL;
	}
	table->index = 0;
	table->arg1 = NULL;
	table->arg2 = NULL;
	table->next_line = NULL;
	table->bin_op = MCC_AST_BINARY_OP_NULL;
	table->op_type = MCC_IR_TABLE_NULL;
	table->jump_target = 0;
	table->memory_size = 0;
	return table;
}

static char *lookup_table_args(struct mcc_ir_line_head *head, char *arg1, char *arg2, enum mcc_ast_type type)
{
	assert(head);
	struct mcc_ir_line *table = head->root->next_line;

	char lookup_arg[128] = {0};
	if (type == MCC_AST_TYPE_ARRAY) {
		sprintf(lookup_arg, "%s[%s]", arg1, arg2);
	} else {
		sprintf(lookup_arg, "%s", arg1);
	}

	int found = 0;
	char value[128] = {0};

	while (table != NULL) {
		int arg2eq = 1;
		if (arg2 != NULL && table->arg2 != NULL && type != MCC_AST_TYPE_ARRAY)
			arg2eq = strcmp(arg2, table->arg2);

		if (table->arg1 != NULL) {
			if (strcmp(lookup_arg, table->arg1) == 0 && arg2eq) {
				sprintf(value, "(%d)", table->index);
				found = 1;
			}
		}
		table = table->next_line;
	}

	if (found == 0) {
		switch (type) {
		case MCC_AST_TYPE_STRING:
			sprintf(value, "\"\"");
			break;
		default:
			sprintf(value, "%d", 0);
			break;
		}
	}

	return strdup(value);
}

static int get_memory_size_line(struct mcc_ir_line_head *head, char *arg1)
{
	assert(head);
	struct mcc_ir_line *table = head->root->next_line;

	while (table != NULL) {
		char value[128] = {0};
		sprintf(value, "(%d)", table->index);
		if (strcmp(value, arg1) == 0) {
			return table->memory_size;
		}

		table = table->next_line;
	}

	return 0;
}

static char *generate_ir_literal_entity(struct mcc_ast_literal *lit)
{
	assert(lit);
	char *entity;
	char value[12] = {0};
	switch (lit->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		sprintf(value, "%d", lit->b_value);
		entity = strdup(value);
		break;
	case MCC_AST_LITERAL_TYPE_INT:
		sprintf(value, "%ld", lit->i_value);
		entity = strdup(value);
		break;
	case MCC_AST_LITERAL_TYPE_FLOAT:
		sprintf(value, "%f", lit->f_value);
		entity = strdup(value);
		break;
	case MCC_AST_LITERAL_TYPE_STRING:
		entity = strdup(lit->str_value);
		break;
	default:
		entity = strdup("-");
	}

	return entity;
}

static void
generate_ir_literal(struct mcc_ast_literal *lit, struct mcc_ir_line_head *head, enum ir_table_operation_type type)
{
	head->index++;
	struct mcc_ir_line *new_table = create_new_ir_line();
	char *entity = generate_ir_literal_entity(lit);
	new_table->arg1 = entity;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->index = head->index;
	new_table->memory_size = get_memory_size_literal_type(lit->type);

	head->current->next_line = new_table;
	head->current = new_table;
}

char *generate_ir_array_access(struct mcc_ir_line_head *head, struct mcc_ast_expression *expr)
{
	char value[64] = {0};
	char *id = expr->array_access_id->identifier->name;
	char *pos;

	if (expr->array_access_exp->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		pos = generate_ir_entity(head, expr->array_access_exp);
		sprintf(value, "%s[%s]", id, pos);

	} else if (expr->array_access_exp->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		pos = lookup_table_args(head, expr->array_access_exp->identifier->name, NULL,
		                        expr->array_access_exp->type);
		sprintf(value, "%s[%s]", id, pos);
	} else {
		generate_ir_expression(expr->array_access_exp, head, MCC_IR_TABLE_NULL);
		sprintf(value, "(%d)", head->index);
		pos = strdup(value);
		sprintf(value, "%s[%s]", id, pos);
		free(pos);
	}

	return strdup(value);
}

static char *generate_ir_entity(struct mcc_ir_line_head *head, struct mcc_ast_expression *expr)
{
	char *entity = NULL;
	char value[64] = {0};
	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		entity = generate_ir_literal_entity(expr->literal);
		break;
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		entity = !entity ? strdup(expr->identifier->name)
		                 : lookup_table_args(head, expr->identifier->name, NULL, MCC_AST_TYPE_STRING);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		entity = generate_ir_array_access(head, expr);
		break;
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		sprintf(value, "(%d)", head->index);
		entity = strdup(value);
		break;
	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		sprintf(value, "(%d)", head->index - 1);
		entity = strdup(value);
		break;
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		sprintf(value, "(%d)", head->index);
		entity = strdup(value);
		break;
	default:
		break;
	}
	return entity;
}

static void generate_ir_table_line(struct mcc_ir_line_head *head,
                                   char *arg1,
                                   char *arg2,
                                   enum ir_table_operation_type type,
                                   int jump_loc,
                                   enum mcc_ast_literal_type lit_type,
                                   int mem_size)
{
	assert(head);

	head->index++;
	struct mcc_ir_line *new_table = create_new_ir_line();

	new_table->arg1 = arg1;
	if (arg2)
		new_table->arg2 = arg2;
	new_table->op_type = type;
	new_table->index = head->index;
	if (jump_loc >= 0) {
		new_table->jump_target = jump_loc;
	}

	if (arg1[0] == '(') {
		new_table->memory_size = get_memory_size_line(head, arg1);
	} else if (arg2 != NULL && arg2[0] == '(') {
		new_table->memory_size = get_memory_size_line(head, arg2);
	} else {
		new_table->memory_size = get_memory_size_literal_type(lit_type);
	}

	if (mem_size > 0)
		new_table->memory_size = mem_size;

	head->current->next_line = new_table;
	head->current = new_table;
}

static void generate_ir_binary_expression(struct mcc_ast_expression *bin_expr,
                                          struct mcc_ir_line_head *head,
                                          enum ir_table_operation_type type)
{
	assert(bin_expr);
	assert(head);

	struct mcc_ast_expression *lhs = bin_expr->lhs;
	struct mcc_ast_expression *rhs = bin_expr->rhs;

	while (rhs->type == MCC_AST_EXPRESSION_TYPE_PARENTH) {
		rhs = rhs->expression;
	}

	while (lhs->type == MCC_AST_EXPRESSION_TYPE_PARENTH) {
		lhs = lhs->expression;
	}

	if (lhs->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER && lhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(lhs, head, type);
	}

	char *entity1;
	if (lhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (lhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			entity1 = lookup_table_args(head, lhs->identifier->name, NULL, lhs->expression_type);
		} else if (lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
			char *arg2 = generate_ir_entity(head, lhs->array_access_exp);
			entity1 =
			    lookup_table_args(head, lhs->array_access_id->identifier->name, arg2, MCC_AST_TYPE_ARRAY);
			free(arg2);
		} else {
			char value[14];
			sprintf(value, "(%d)", head->index);
			entity1 = strdup(value);
		}

	} else {
		entity1 = generate_ir_entity(head, lhs);
	}

	if (rhs->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER && rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(rhs, head, type);
	}

	char *entity2;
	if (rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (rhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			entity2 = lookup_table_args(head, rhs->identifier->name, NULL, rhs->expression_type);
		} else if (rhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
			char *arg2 = generate_ir_entity(head, rhs->array_access_exp);
			entity2 =
			    lookup_table_args(head, rhs->array_access_id->identifier->name, arg2, MCC_AST_TYPE_ARRAY);
			free(arg2);
		} else {
			char value[14];
			sprintf(value, "(%d)", head->index);
			entity2 = strdup(value);
		}
	} else {
		entity2 = generate_ir_entity(head, rhs);
	}

	head->index++;
	struct mcc_ir_line *new_table = create_new_ir_line();

	new_table->arg1 = entity1;
	new_table->arg2 = entity2;
	new_table->op_type = type;
	new_table->bin_op = bin_expr->op;
	new_table->index = head->index;
	new_table->memory_size = get_memory_size_type(bin_expr->rhs->expression_type);

	head->current->next_line = new_table;
	head->current = new_table;
}

static void generate_ir_unary_expression(struct mcc_ast_expression *un_expr,
                                         struct mcc_ir_line_head *head,
                                         enum ir_table_operation_type type)
{
	assert(un_expr);
	assert(head);

	struct mcc_ir_line *new_table = create_new_ir_line();

	char *entity1;
	char value[64] = {0};

	if (un_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		entity1 = generate_ir_entity(head, un_expr->rhs);
	} else if (un_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL) {
		generate_ir_function_call(un_expr->rhs, head);
		sprintf(value, "(%d)", head->index);
		entity1 = strdup(value);
	} else {
		char *value;
		if (un_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
			value = generate_ir_array_access(head, un_expr->rhs);
		} else {
			value = lookup_table_args(head, un_expr->rhs->identifier->name, NULL, un_expr->expression_type);
		}
		entity1 = strdup(value);
	}

	head->index++;
	new_table->arg1 = entity1;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->un_op = un_expr->u_op;
	new_table->index = head->index;
	new_table->memory_size = get_memory_size_type(un_expr->expression_type);

	head->current->next_line = new_table;
	head->current = new_table;
}

static struct mcc_ast_function_arguments *reverse_recursive(struct mcc_ast_function_arguments *args)
{
	return args;

	if (args == NULL || args->next_argument == NULL) {
		return args;
	}

	struct mcc_ast_function_arguments *reversed_list = reverse_recursive(args->next_argument);

	args->next_argument->next_argument = args;
	args->next_argument = NULL;
	return reversed_list;
}

static void generate_function_arguments(struct mcc_ast_expression *expr, struct mcc_ir_line_head *head)
{
	assert(head);

	struct mcc_ast_function_arguments *list = reverse_recursive(expr->function_call_arguments);
	expr->function_call_arguments = list;
	while (list != NULL) {
		generate_ir_expression(list->expression, head, MCC_IR_TABLE_PUSH);
		// insert additional line in IR table

		if (list->expression->type != MCC_AST_EXPRESSION_TYPE_LITERAL &&
		    list->expression->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER &&
		    head->current->op_type != MCC_IR_TABLE_PUSH) {
			char *value = generate_ir_entity(head, list->expression);

			int mem_size = list->expression->expression_type == MCC_AST_TYPE_FLOAT ? 2 : 0;

			generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_PUSH, -1, -1, mem_size);
			free(value);
		}
		list = list->next_argument;
	}
}

static void generate_ir_function_call(struct mcc_ast_expression *expr_call, struct mcc_ir_line_head *head)
{
	assert(expr_call);
	assert(head);

	char *func_id = expr_call->function_call_identifier->identifier->name;

	// normal functions
	generate_function_arguments(expr_call, head);

	// call function line
	generate_ir_table_line(head, strdup(func_id), NULL, MCC_IR_TABLE_CALL, -1, -1,
	                       get_memory_size_type(expr_call->expression_type));
}

static void generate_ir_identifier(struct mcc_ast_expression *expr,
                                   struct mcc_ir_line_head *head,
                                   enum ir_table_operation_type type)
{
	char *value = lookup_table_args(head, expr->identifier->name, NULL, expr->expression_type);
	if (expr->expression_type == MCC_AST_TYPE_BOOL)
		generate_ir_table_line(head, value, NULL, MCC_IR_TABLE_BOOL, -1, -1, -1);
	else
		generate_ir_table_line(head, value, NULL, type, -1, -1, -1); // todo error from here!!
}

static void generate_ir_expression(struct mcc_ast_expression *expr,
                                   struct mcc_ir_line_head *head,
                                   enum ir_table_operation_type type)
{
	assert(expr);
	assert(head);

	if (expr->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER && expr->expression_type == MCC_AST_TYPE_ARRAY)
		expr->identifier->type = expr->expression_type;

	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		generate_ir_binary_expression(expr, head, MCC_IR_TABLE_BINARY_OP);
		break;
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		generate_ir_unary_expression(expr, head, MCC_IR_TABLE_UNARY_OP);
		break;
	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		generate_ir_expression(expr->expression, head, type);
		break;
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		generate_ir_literal(expr->literal, head, type);
		break;
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		generate_ir_identifier(expr, head, type);
		break;
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		generate_ir_function_call(expr, head);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		break;
	}
}

static void generate_ir_declaration_array(struct mcc_ast_declare_assign *decl, struct mcc_ir_line_head *head)
{
	assert(decl);
	assert(head);

	if (decl->declare_array_size == NULL || *decl->declare_array_size <= 0)
		return;

	int mem_size = decl->declare_type == MCC_AST_TYPE_FLOAT ? 2 : 0;

	char value[64] = {0};
	sprintf(value, "%ld", *decl->declare_array_size);
	generate_ir_table_line(head, strdup(decl->declare_id->identifier->name), strdup(value), MCC_IR_TABLE_ARRAY, -1,
	                       -1, mem_size);
}

static void generate_ir_assignment(struct mcc_ast_declare_assign *assign, struct mcc_ir_line_head *head)
{
	assert(assign);
	assert(head);

	enum mcc_ast_literal_type lit_type = -1;
	char *entity2;
	char value[64] = {0};

	struct mcc_ast_expression *assign_rhs = assign->assign_rhs;

	while (assign_rhs->type == MCC_AST_EXPRESSION_TYPE_PARENTH) {
		assign_rhs = assign_rhs->expression;
	}

	if (assign_rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (assign_rhs->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			generate_ir_expression(assign_rhs, head, -1);
			sprintf(value, "(%d)", head->index);
		} else {
			char *result = lookup_table_args(head, assign_rhs->identifier->name, NULL, MCC_AST_TYPE_STRING);
			sprintf(value, "%s", result);
			free(result);
		}
		entity2 = strdup(value);
	} else {
		entity2 = generate_ir_literal_entity(assign_rhs->literal);
		lit_type = assign_rhs->literal->type;
	}

	enum ir_table_operation_type type;
	char *entity1;
	if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		entity1 = generate_ir_entity(head, assign->assign_lhs);
		type = MCC_IR_TABLE_ASSIGNMENT;
	} else if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		char *id = assign->assign_lhs->array_access_id->identifier->name;
		if (assign->assign_lhs->array_access_exp->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
			if (assign->assign_lhs->array_access_exp->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
				generate_ir_expression(assign->assign_lhs->array_access_exp, head, -1);
				sprintf(value, "%s[(%d)]", id, head->index);
			} else {
				char *id_index =
				    lookup_table_args(head, assign->assign_lhs->array_access_exp->identifier->name,

				                      NULL, assign->assign_lhs->expression_type);

				sprintf(value, "%s[%s]", id, id_index);
				free(id_index);
			}
		} else {
			char *lit = generate_ir_literal_entity(assign->assign_lhs->array_access_exp->literal);
			sprintf(value, "%s[%s]", id, lit);
			free(lit);
			lit_type = assign->assign_lhs->array_access_exp->literal->type;
		}
		entity1 = strdup(value);
		type = MCC_IR_TABLE_STORE;
	}

	int mem_size = assign->assign_lhs->expression_type == MCC_AST_TYPE_FLOAT ? 2 : 0;

	generate_ir_table_line(head, entity1, entity2, type, -1, lit_type, mem_size);
}

static void generate_ir_return(struct mcc_ast_expression *expr, struct mcc_ir_line_head *head)
{
	assert(head);

	char value[14] = {0};

	if (expr) { // needed if function returns void (nothing)
		generate_ir_expression(expr, head, MCC_IR_TABLE_RETURN);

		// insert additional line in IR table
		if (expr->type != MCC_AST_EXPRESSION_TYPE_LITERAL && expr->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			sprintf(value, "(%d)", head->index);
			generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_RETURN, -1, -1, -1);
		}
	} else {
		sprintf(value, "-");
		generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_RETURN, -1, -1, -1);
	}
}

static void generate_ir_if(struct mcc_ast_statement *stmt, struct mcc_ir_line_head *head)
{
	assert(stmt);
	assert(head);

	char value[64] = {0};

	char *jump_loc = NULL;
	char *jump_false_loc = NULL;
	struct mcc_ir_line *jumpfalse_table = create_new_ir_line();
	char *entity1;

	// if condition
	if (stmt->if_cond->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		entity1 = generate_ir_entity(head, stmt->if_cond);
	} else if (stmt->if_cond->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		entity1 = generate_ir_array_access(head, stmt->if_cond);
	} else {
		generate_ir_expression(stmt->if_cond, head, -1);
		sprintf(value, "(%d)", head->current->index);
		entity1 = strdup(value);
	}
	head->index++;
	jumpfalse_table->arg1 = entity1;
	jumpfalse_table->arg2 = jump_false_loc;
	jumpfalse_table->op_type = MCC_IR_TABLE_JUMPFALSE;
	jumpfalse_table->index = head->index;

	head->current->next_line = jumpfalse_table;
	head->current = jumpfalse_table;

	// if body
	generate_ir_statement(stmt->if_stat, head);

	// generate jump table
	struct mcc_ir_line *jump_table = NULL;
	if (stmt->else_stat) {
		jump_table = create_new_ir_line();
		head->index++;
		jump_table->arg1 = jump_loc;
		jump_table->arg2 = NULL;
		jump_table->op_type = MCC_IR_TABLE_JUMP;
		jump_table->index = head->index;

		head->current->next_line = jump_table;
		head->current = jump_table;
	}

	sprintf(value, "L_%s_%d", head->func_name, head->current->index);

	// set jumpfalse
	jump_false_loc = strdup(value);
	jumpfalse_table->arg2 = jump_false_loc;
	jumpfalse_table->jump_target = head->current->index + 1;

	// create label for jumpfalse
	char *label_entity = strdup(value);
	generate_ir_table_line(head, label_entity, NULL, MCC_IR_TABLE_BR_LABEL, -1, -1, -1);

	// else body
	if (stmt->else_stat) {
		generate_ir_statement(stmt->else_stat, head);

		// set jump loc
		if (jump_table != NULL) {
			sprintf(value, "L_%s_%d", head->func_name, head->current->index + 1);
			jump_loc = strdup(value);
			jump_table->arg1 = jump_loc;
			jump_table->jump_target = head->current->index + 1;
		}
		// create label for jump
		char *label_entity2 = strdup(value);
		generate_ir_table_line(head, label_entity2, NULL, MCC_IR_TABLE_BR_LABEL, -1, -1, -1);
	}
}

static void generate_ir_while(struct mcc_ast_statement *stmt, struct mcc_ir_line_head *head)
{
	assert(stmt);
	assert(head);

	char value[64] = {0};

	char *jump_loc = NULL;
	char *jump_false_loc = NULL;

	struct mcc_ir_line *cond_table = create_new_ir_line();
	char *entity1;

	sprintf(value, "L_%s_%d", head->func_name, head->index + 1);
	generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_BR_LABEL, head->index + 1, -1, -1);

	// set jump loc
	jump_loc = strdup(value);

	int jump_target = head->current->index + 1;

	// while condition
	if (stmt->while_cond->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		entity1 = generate_ir_entity(head, stmt->while_cond);
	} else {
		generate_ir_expression(stmt->while_cond, head, -1);
		sprintf(value, "(%d)", head->current->index);
		entity1 = strdup(value);
	}

	head->index++;
	cond_table->arg1 = entity1;
	cond_table->op_type = MCC_IR_TABLE_JUMPFALSE;
	cond_table->index = head->index;

	head->current->next_line = cond_table;
	head->current = cond_table;

	// while body
	generate_ir_statement(stmt->while_stat, head);

	// generate jump table
	struct mcc_ir_line *jump_table = create_new_ir_line();
	if (!hasStatementReturn(stmt->while_stat)) {
		head->index++;
		jump_table->arg1 = jump_loc;
		jump_table->arg2 = NULL;
		jump_table->op_type = MCC_IR_TABLE_JUMP;
		jump_table->index = head->index;
		jump_table->jump_target = jump_target;

		head->current->next_line = jump_table;
		head->current = jump_table;
	}

	sprintf(value, "L_%s_%d", head->func_name, head->current->index);

	// set jump false
	jump_false_loc = strdup(value);
	cond_table->arg2 = jump_false_loc;
	cond_table->jump_target = head->current->index + 1;

	// create label for jumpfalse
	char *label_entity = strdup(value);
	generate_ir_table_line(head, label_entity, NULL, MCC_IR_TABLE_BR_LABEL, -1, -1, -1);
}

static void generate_ir_compound(struct mcc_ast_statement_list *list, struct mcc_ir_line_head *head)
{
	struct mcc_ast_statement_list *temp_list = list;
	while (temp_list != NULL) {
		generate_ir_statement(temp_list->statement, head);
		if (temp_list->statement->type == MCC_AST_STATEMENT_RETURN)
			break;
		temp_list = temp_list->next_statement;
	}
}

static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_line_head *head)
{
	assert(stmt);
	assert(head);

	switch (stmt->type) {
	case MCC_AST_STATEMENT_EXPRESSION:
		generate_ir_expression(stmt->expression, head, MCC_IR_TABLE_COPY);
		break;
	case MCC_AST_STATEMENT_ASSIGNMENT:
		generate_ir_assignment(stmt->declare_assign, head);
		break;
	case MCC_AST_STATEMENT_DECLARATION:
		generate_ir_declaration_array(stmt->declare_assign, head);
		break;
	case MCC_AST_STATEMENT_RETURN:
		generate_ir_return(stmt->expression, head);
		break;
	case MCC_AST_STATEMENT_IF:
		generate_ir_if(stmt, head);
		break;
	case MCC_AST_STATEMENT_WHILE:
		generate_ir_while(stmt, head);
		break;
	case MCC_AST_STATEMENT_COMPOUND:
		generate_ir_compound(stmt->compound, head);
		break;
	}
}

static int hasStatementReturn(struct mcc_ast_statement *stmt)
{
	struct mcc_ast_statement_list *tmp_list = stmt->compound;

	switch (stmt->type) {
	case MCC_AST_STATEMENT_RETURN:
		return 1;
	case MCC_AST_STATEMENT_COMPOUND: {
		int result;
		while (tmp_list != NULL) {
			result = hasStatementReturn(tmp_list->statement);
			tmp_list = tmp_list->next_statement;
			if (result > 0)
				return result;
		}
	}

	default:
		break;
	}
	return 0;
}

static void generate_ir_param(struct mcc_ast_parameter *param, struct mcc_ir_line_head *head)
{
	assert(param);
	assert(head);

	head->index++;
	struct mcc_ir_line *new_table = create_new_ir_line();
	char *entity = generate_ir_entity(head, param->parameter->declare_id);

	new_table->arg1 = entity;
	new_table->op_type = MCC_IR_TABLE_POP;
	new_table->index = head->index;

	struct mcc_ir_function_signature_parameters *params = head->parameters;
	while (params != NULL) {
		if (strcmp(params->arg_name, param->parameter->declare_id->identifier->name) == 0) {
			new_table->memory_size = head->parameters->size;
		}
		params = params->next_parameter;
	}

	head->current->next_line = new_table;
	head->current = new_table;
}

static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_line_head *head)
{
	assert(func);
	assert(head);

	// func identifier
	char *id_entity = generate_ir_entity(head, func->func_identifier);
	if (head->index == 0) {
		generate_ir_table_line(head, id_entity, NULL, MCC_IR_TABLE_LABEL, -1, -1, -1);
	} else {
		generate_ir_table_line(head, id_entity, NULL, MCC_IR_TABLE_CALL, -1, -1, -1);
	}

	// func parameter list
	struct mcc_ast_parameter *param = func->parameter_list;
	while (param != NULL) {
		generate_ir_param(param, head);
		param = param->next_parameter;
	}

	// func compound
	struct mcc_ast_statement_list *list = func->func_compound->compound;
	while (list != NULL) {
		generate_ir_statement(list->statement, head);
		list = list->next_statement;
	}

	if (!hasStatementReturn(func->func_compound)) {
		generate_ir_return(NULL, head);
	}
}

struct mcc_ir_function_signature_parameters *get_function_parameter_size(struct mcc_ast_parameter *parameter_list)
{
	assert(parameter_list);

	struct mcc_ir_function_signature_parameters *root;
	struct mcc_ir_function_signature_parameters *current;

	int counter = 0;
	int total_size = 0;
	while (parameter_list != NULL) {
		struct mcc_ir_function_signature_parameters *new_params = malloc(sizeof(*new_params));
		if (!new_params)
			return NULL;

		new_params->arg_name = strdup(parameter_list->parameter->declare_id->identifier->name);
		new_params->size = get_memory_size_type(parameter_list->parameter->declare_type);
		new_params->next_parameter = NULL;
		new_params->index = counter;
		total_size = total_size + 1; // new_params->size;

		if (counter == 0) {
			root = new_params;
			current = root;
		} else {
			current->next_parameter = new_params;
			current = new_params;
		}

		parameter_list = parameter_list->next_parameter;
		counter++;
	}

	root->total_size = total_size;
	return root;
}

struct mcc_ir_line_head *create_line_head(struct mcc_ast_program *program)
{
	struct mcc_ir_line_head *head = malloc(sizeof(*head));
	if (!head)
		return NULL;

	struct mcc_ir_line *line = create_new_ir_line();
	line->index = 0;

	head->root = line;
	head->current = line;
	head->index = 0;
	head->program = program;

	return head;
}

struct mcc_ir_table_head *mcc_create_ir(struct mcc_ast_program *program, FILE *out, int log_level)
{
	assert(program);

	struct mcc_ir_table_head *table_head = malloc(sizeof(*table_head));

	int i = 0;
	struct mcc_ast_func_list *list = program->function_list;
	while (list != NULL) {
		char *func_id = list->function->func_identifier->identifier->name;

		struct mcc_ir_line_head *line_head = create_line_head(program);
		line_head->func_name = strdup(func_id);
		if (list->function->parameter_list) {
			line_head->parameters = get_function_parameter_size(list->function->parameter_list);
		} else {
			line_head->parameters = NULL;
		}
		generate_function_definition(list->function, line_head);
		line_head->current->next_line = NULL;

		struct mcc_ir_table *table = malloc(sizeof(*table));
		table->line_head = line_head;
		table->func_name = func_id;
		table->next_table = NULL;
		table->id = i;

		if (i == 0) {
			table_head->root = table;
			table_head->current = table;
		} else {
			table_head->current->next_table = table;
			table_head->current = table;
		}

		if (log_level > 0)
			mcc_print_ir_table(table->line_head->root, func_id, out);

		list = list->next_function;
		i++;
	}
	return table_head;
}

void mcc_delete_line(struct mcc_ir_line *line)
{
	if (line->next_line != NULL) {
		mcc_delete_line(line->next_line);
	}

	free(line->arg1);
	free(line->arg2);
	free(line);
}

void mcc_delete_func_sign_parameters(struct mcc_ir_function_signature_parameters *param)
{
	if (param->next_parameter != NULL) {
		mcc_delete_func_sign_parameters(param->next_parameter);
	}
	free(param->arg_name);
	free(param);
}

void mcc_delete_line_head(struct mcc_ir_line_head *line_head)
{
	mcc_delete_line(line_head->root);

	if (line_head->parameters != NULL)
		mcc_delete_func_sign_parameters(line_head->parameters);

	free(line_head->func_name);
	free(line_head);
}

void mcc_delete_table(struct mcc_ir_table *table)
{
	if (table->next_table != NULL) {
		mcc_delete_table(table->next_table);
	}

	mcc_delete_line_head(table->line_head);
	free(table);
}

void mcc_delete_ir(struct mcc_ir_table_head *table_head)
{
	if (table_head->root != NULL)
		mcc_delete_table(table_head->root);
	free(table_head);
}
