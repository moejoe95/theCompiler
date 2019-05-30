#include "mcc/ir.h"

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "mcc/print_ir.h"

// forward declarations
static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_head *head);
static void generate_built_in_function_call(struct mcc_ast_expression *expr_call, struct mcc_ir_head *head, char *b);
static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_head *head);
static void generate_ir_function_call(struct mcc_ast_expression *e, struct mcc_ir_head *head);
static void
generate_ir_expression(struct mcc_ast_expression *e, struct mcc_ir_head *head, enum ir_table_operation_type t);

static struct mcc_ir_table *create_new_ir_table()
{
	struct mcc_ir_table *table = malloc(sizeof(*table));
	if (!table) {
		return NULL;
	}
	table->index = 0;
	table->arg1 = NULL;
	table->arg2 = NULL;
	table->next_table = NULL;
	table->bin_op = MCC_AST_BINARY_OP_NULL;
	table->op_type = MCC_IR_TABLE_NULL;
	table->jump_target = 0;
	return table;
}

static char *lookup_table_args(struct mcc_ir_head *head, char *arg1, char *arg2)
{
	assert(head);
	char *result = NULL;
	struct mcc_ir_table *table = head->root->next_table;
	while (table != NULL) {
		int arg2eq = 1;
		if (arg2 != NULL) {
			arg2eq = strcmp(arg2, table->arg2);
		}
		if (table->arg1 != NULL) {
			char *table_arg = strdup(table->arg1);
			if (strcmp(arg1, strtok(table_arg, "[")) == 0 && arg2eq) {
				char value[12] = {0};
				sprintf(value, "(%d)", table->index);
				result = strdup(value);
				free(table_arg);
				break;
			}
			free(table_arg);
		}
		table = table->next_table;
	}
	return result;
}

static char *generate_ir_literal_entity(struct mcc_ast_literal *lit)
{
	assert(lit);
	char *entity;
	char value[12] = {0};
	switch (lit->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		sprintf(value, "%s", lit->b_value == 1 ? "true" : "false");
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
generate_ir_identifier(struct mcc_ast_identifier *id, struct mcc_ir_head *head, enum ir_table_operation_type type)
{
	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity;
	if (id->type == MCC_AST_TYPE_ARRAY) {
		char value[64] = {0};
		sprintf(value, "%s[]", id->name);
		entity = strdup(value);
	} else {
		entity = strdup(id->name);
	}
	new_table->arg1 = entity;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void
generate_ir_literal(struct mcc_ast_literal *lit, struct mcc_ir_head *head, enum ir_table_operation_type type)
{
	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity = generate_ir_literal_entity(lit);
	new_table->arg1 = entity;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static char *generate_ir_entity(struct mcc_ir_head *head, struct mcc_ast_expression *expr)
{
	char *entity = NULL;
	char value[64] = {0};
	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		entity = generate_ir_literal_entity(expr->literal);
		break;
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		entity =
		    !entity ? strdup(expr->identifier->name) : lookup_table_args(head, expr->identifier->name, NULL);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
	case MCC_AST_EXPRESSION_TYPE_PARENTH:
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		sprintf(value, "(%d)", head->index - 1);
		entity = strdup(value);
		break;
	default:
		break;
	}
	return entity;
}

static void generate_ir_table_line(
    struct mcc_ir_head *head, char *arg1, char *arg2, enum ir_table_operation_type type, int jump_loc)
{
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	new_table->arg1 = arg1;
	if (arg2)
		new_table->arg2 = arg2;
	new_table->op_type = type;
	new_table->index = head->index;
	if (jump_loc >= 0) {
		new_table->jump_target = jump_loc;
	}

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_ir_binary_expression(struct mcc_ast_expression *bin_expr,
                                          struct mcc_ir_head *head,
                                          enum ir_table_operation_type type)
{
	assert(bin_expr);
	assert(head);

	if (bin_expr->lhs->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER &&
	    bin_expr->lhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(bin_expr->lhs, head, type);
	}
	if (bin_expr->rhs->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER &&
	    bin_expr->rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(bin_expr->rhs, head, type);
	}

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	char *entity1;
	if (bin_expr->lhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		if (bin_expr->lhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			entity1 = lookup_table_args(head, bin_expr->lhs->identifier->name, NULL);
		} else {
			char value[14];
			sprintf(value, "(%d)", head->index - 1);
			entity1 = strdup(value);
		}

	} else {
		entity1 = generate_ir_entity(head, bin_expr->lhs);
	}
	char *entity2 = generate_ir_entity(head, bin_expr->rhs);

	new_table->arg1 = entity1;
	new_table->arg2 = entity2;
	new_table->op_type = type;
	new_table->bin_op = bin_expr->op;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_ir_unary_expression(struct mcc_ast_expression *un_expr,
                                         struct mcc_ir_head *head,
                                         enum ir_table_operation_type type)
{
	assert(un_expr);
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	char *entity1 = generate_ir_entity(head, un_expr->rhs);

	new_table->arg1 = entity1;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->un_op = un_expr->u_op;
	new_table->index = head->index;

	head->current->next_table = new_table;
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

static void generate_function_arguments(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
	assert(head);

	struct mcc_ast_function_arguments *list = reverse_recursive(expr->function_call_arguments);
	expr->function_call_arguments = list;

	while (list != NULL) {
		generate_ir_expression(list->expression, head, MCC_IR_TABLE_PUSH);
		list = list->next_argument;
	}
}

static void generate_ir_function_call(struct mcc_ast_expression *expr_call, struct mcc_ir_head *head)
{
	assert(expr_call);
	assert(head);

	char *func_id = expr_call->function_call_identifier->identifier->name;

	// built in functions
	int i = 0;
	static char *built_ins[6] = {"print_nl", "read_int", "print", "print_int", "print_float", "read_float"};
	while (built_ins[i]) {
		if (strcmp(built_ins[i], func_id) == 0) {
			generate_built_in_function_call(expr_call, head, built_ins[i]);
			return;
		}
		i++;
	}

	// normal functions
	generate_function_arguments(expr_call, head);

	char *label = lookup_table_args(head, func_id, NULL);
	if (label) {
		generate_ir_table_line(head, label, NULL, MCC_IR_TABLE_JUMP, -1);
		return;
	}

	struct mcc_ast_func_list *list = head->program->function_list;
	while (list != NULL) {
		if (strcmp(list->function->func_identifier->identifier->name, func_id) == 0) {
			generate_function_definition(list->function, head);
			break;
		}
		list = list->next_function;
	}
}

static void
generate_built_in_function_call(struct mcc_ast_expression *expr_call, struct mcc_ir_head *head, char *built_in)
{
	assert(expr_call);
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity = NULL;

	if (expr_call->function_call_arguments) {
		struct mcc_ast_literal *lit = expr_call->function_call_arguments->expression->literal;
		entity = generate_ir_literal_entity(lit);
	} else {
		entity = strdup("-");
	}
	new_table->arg1 = entity;
	new_table->arg2 = NULL;
	new_table->op_type = MCC_IR_TABLE_BUILT_IN;
	new_table->index = head->index;
	new_table->built_in = built_in;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_ir_array_access(struct mcc_ast_expression *id_expr,
                                     struct mcc_ast_expression *array_expr,
                                     struct mcc_ir_head *head)
{
	assert(id_expr);
	assert(array_expr);
	assert(head);

	head->index++;
	char value[128] = {0};

	char *id = id_expr->identifier->name;
	if (array_expr->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(array_expr, head, -1);
		sprintf(value, "%s[(%d)]", id, head->index);
	} else {
		char *lit = generate_ir_literal_entity(array_expr->literal);
		sprintf(value, "%s[%s]", id, lit);
	}

	generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_LOAD, -1);
}

static void
generate_ir_expression(struct mcc_ast_expression *expr, struct mcc_ir_head *head, enum ir_table_operation_type type)
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
		generate_ir_identifier(expr->identifier, head, type);
		break;
	case MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL:
		generate_ir_function_call(expr, head);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		generate_ir_array_access(expr->array_access_id, expr->array_access_exp, head);
		break;
	}
}

static void generate_ir_assignment(struct mcc_ast_declare_assign *assign, struct mcc_ir_head *head)
{
	assert(assign);
	assert(head);

	char *entity2;
	char value[64] = {0};
	if (assign->assign_rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(assign->assign_rhs, head, -1);
		sprintf(value, "(%d)", head->index);
		entity2 = strdup(value);
	} else {
		entity2 = generate_ir_literal_entity(assign->assign_rhs->literal);
	}

	enum ir_table_operation_type type;
	char *entity1;
	if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		entity1 = generate_ir_entity(head, assign->assign_lhs);
		type = MCC_IR_TABLE_ASSIGNMENT;
	} else if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		char *id = assign->assign_lhs->array_access_id->identifier->name;
		if (assign->assign_lhs->array_access_exp->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
			generate_ir_expression(assign->assign_lhs->array_access_exp, head, -1);
			sprintf(value, "%s[(%d)]", id, head->index);
		} else {
			char *lit = generate_ir_literal_entity(assign->assign_lhs->array_access_exp->literal);
			sprintf(value, "%s[%s]", id, lit);
			free(lit);
		}
		entity1 = strdup(value);
		type = MCC_IR_TABLE_STORE;
	}

	generate_ir_table_line(head, entity1, entity2, type, -1);
}

static void generate_ir_return(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
	assert(head);

	char value[14] = {0};

	// push
	if (expr) {
		generate_ir_expression(expr, head, MCC_IR_TABLE_PUSH);

		// insert additional line in IR table
		if (expr->type != MCC_AST_EXPRESSION_TYPE_LITERAL && expr->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
			sprintf(value, "(%d)", head->index - 1);
			generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_PUSH, -1);
		}
	}
	// jump
	sprintf(value, "(%d)", head->index + 2);
	generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_JUMP, head->index + 2);

	// pop
	if (expr) {
		sprintf(value, "(%d)", head->index);
		generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_POP, -1);
	}
}

static void generate_ir_if(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
	assert(stmt);
	assert(head);

	char value[14] = {0};

	char *jump_loc = NULL;
	char *jump_false_loc = NULL;
	struct mcc_ir_table *jumpfalse_table = create_new_ir_table();
	char *entity1;

	// if condition
	if (stmt->if_cond->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER ||
	    stmt->if_cond->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
		entity1 = generate_ir_entity(head, stmt->if_cond);
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

	head->current->next_table = jumpfalse_table;
	head->current = jumpfalse_table;

	// if body
	generate_ir_statement(stmt->if_stat, head);

	// generate jump table
	struct mcc_ir_table *jump_table = create_new_ir_table();
	head->index++;
	jump_table->arg1 = jump_loc;
	jump_table->arg2 = NULL;
	jump_table->op_type = MCC_IR_TABLE_JUMP;
	jump_table->index = head->index;

	head->current->next_table = jump_table;
	head->current = jump_table;

	// set jumpfalse
	sprintf(value, "(%d)", head->current->index + 1);
	jump_false_loc = strdup(value);
	jumpfalse_table->arg2 = jump_false_loc;
	jumpfalse_table->jump_target = head->current->index + 1;

	// else body
	if (stmt->else_stat) {
		generate_ir_statement(stmt->else_stat, head);
		sprintf(value, "(%d)", head->current->index + 2);
		generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_JUMP, head->current->index + 2);
	}

	// set jump loc
	sprintf(value, "(%d)", head->current->index + 1);
	jump_loc = strdup(value);
	jump_table->arg1 = jump_loc;
	jump_table->jump_target = head->current->index + 1;
}

static void generate_ir_while(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
	assert(stmt);
	assert(head);

	char value[12] = {0};

	char *jump_loc = NULL;
	char *jump_false_loc = NULL;

	struct mcc_ir_table *cond_table = create_new_ir_table();
	char *entity1;

	sprintf(value, "(%d)", head->index + 2);
	generate_ir_table_line(head, strdup(value), NULL, MCC_IR_TABLE_JUMP, head->index + 2);

	int index = head->index + 1;
	int jump_target = head->current->index + 1;

	// while condition
	if (stmt->while_cond->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER ||
	    stmt->while_cond->type == MCC_AST_EXPRESSION_TYPE_LITERAL) {
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

	head->current->next_table = cond_table;
	head->current = cond_table;

	// set jump loc
	sprintf(value, "(%d)", index);
	jump_loc = strdup(value);

	// while body
	generate_ir_statement(stmt->while_stat, head);

	// generate jump table
	struct mcc_ir_table *jump_table = create_new_ir_table();
	head->index++;
	jump_table->arg1 = jump_loc;
	jump_table->arg2 = NULL;
	jump_table->op_type = MCC_IR_TABLE_JUMP;
	jump_table->index = head->index;
	jump_table->jump_target = jump_target;

	head->current->next_table = jump_table;
	head->current = jump_table;

	// set jump false
	sprintf(value, "(%d)", head->current->index + 1);
	jump_false_loc = strdup(value);
	cond_table->arg2 = jump_false_loc;
	cond_table->jump_target = head->current->index + 1;
	jump_table->arg2 = strdup(jump_false_loc);
}

static void generate_ir_compound(struct mcc_ast_statement_list *list, struct mcc_ir_head *head)
{
	while (list != NULL) {
		generate_ir_statement(list->statement, head);
		list = list->next_statement;
	}
}

static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
	assert(stmt);
	assert(head);

	switch (stmt->type) {
	case MCC_AST_STATEMENT_EXPRESSION:
		generate_ir_expression(stmt->expression, head, MCC_IR_TABLE_COPY);
		break;
	case MCC_AST_STATEMENT_DECLARATION:
		break;
	case MCC_AST_STATEMENT_ASSIGNMENT:
		generate_ir_assignment(stmt->declare_assign, head);
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

static void generate_ir_param(struct mcc_ast_parameter *param, struct mcc_ir_head *head)
{
	assert(param);
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity = generate_ir_entity(head, param->parameter->declare_id);

	new_table->arg1 = entity;
	new_table->op_type = MCC_IR_TABLE_POP;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_head *head)
{
	assert(func);
	assert(head);

	// func identifier
	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *id_entity = generate_ir_entity(head, func->func_identifier);

	new_table->arg1 = id_entity;
	new_table->arg2 = NULL;
	new_table->op_type = MCC_IR_TABLE_LABEL;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;

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
}

struct mcc_ir_table *mcc_create_ir(struct mcc_ast_program *program, FILE *out, int log_level)
{
	assert(program);

	struct mcc_ir_head *head = malloc(sizeof(*head));
	if (!head)
		return NULL;

	struct mcc_ir_table *table = create_new_ir_table();
	table->index = 0;

	head->root = table;
	head->current = table;
	head->index = 0;
	head->program = program;

	if (program->type == MCC_AST_PROGRAM_TYPE_FUNCTION) {
		generate_function_definition(program->function, head);
	}

	if (program->type == MCC_AST_PROGRAM_TYPE_FUNCTION_LIST) {
		struct mcc_ast_func_list *list = program->function_list;
		// search for main function
		while (list != NULL) {
			if (strcmp(list->function->func_identifier->identifier->name, "main") == 0) {
				generate_function_definition(list->function, head);
				break;
			}
			list = list->next_function;
		}
	}

	if (log_level > 0)
		mcc_print_ir_table(table, out);

	free(head);
	return table;
}

void mcc_delete_ir(struct mcc_ir_table *table)
{
	if (table->next_table != NULL) {
		mcc_delete_ir(table->next_table);
	}

	free(table->arg1);
	free(table->arg2);
	free(table);
}
