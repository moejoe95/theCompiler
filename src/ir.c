#include "mcc/ir.h"
#include "assert.h"
#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

// forward declarations
static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_head *head);
static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_head *head);
static void
generate_ir_expression(struct mcc_ast_expression *e, struct mcc_ir_head *head, enum ir_table_operation_type t);

static struct mcc_ir_table *create_new_ir_table()
{
	struct mcc_ir_table *table = malloc(sizeof(*table));
	if (!table) {
		return NULL;
	}
	return table;
}

static char *lookup_table_args(struct mcc_ir_head *head, char *arg1, char *arg2)
{
	assert(head);

	struct mcc_ir_table *table = head->root->next_table;
	while (table != NULL) {
		int arg2eq = 1;
		if (arg2 != NULL) {
			arg2eq = strcmp(arg2, table->arg2);
		}
		if (strcmp(arg1, table->arg1) == 0 && arg2eq) {
			char value[12] = {0};
			sprintf(value, "(%d)", table->index);
			return strdup(value);
		}
		table = table->next_table;
	}
	return NULL;
}

static char *generate_ir_literal_entity(struct mcc_ast_literal *lit)
{
	assert(lit);
	char *entity;
	char value[12] = {0};
	switch (lit->type) {
	case MCC_AST_LITERAL_TYPE_BOOL:
		if (lit->b_value == 1)
			sprintf(value, "%s", "true");
		else
			sprintf(value, "%s", "false");
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
	}

	return entity;
}

static void
generate_ir_identifier(struct mcc_ast_identifier *id, struct mcc_ir_head *head, enum ir_table_operation_type type)
{
	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity = strdup(id->name);
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

static char *generate_ir_entity(struct mcc_ast_expression *expr)
{
	char *entity;
	char value[64] = {0};
	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		entity = generate_ir_literal_entity(expr->literal);
		break;
	case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
		entity = strdup(expr->identifier->name);
		break;
	case MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS:
		sprintf(value, "%s[]", expr->array_access_id->identifier->name);
		entity = strdup(value);
		break;
	default:
		break;
	}
	return entity;
}

static void generate_ir_table_line(struct mcc_ir_head *head, char *arg1, char *arg2, enum ir_table_operation_type type)
{
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	new_table->arg1 = strdup(arg1);
	if (arg2)
		new_table->arg2 = strdup(arg2);
	new_table->op_type = type;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_ir_binary_expression(struct mcc_ast_expression *bin_expr,
                                          struct mcc_ir_head *head,
                                          enum ir_table_operation_type type)
{
	assert(bin_expr);
	assert(head);

	if (bin_expr->lhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
		generate_ir_binary_expression(bin_expr->lhs, head, type);
	}
	if (bin_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP) {
		generate_ir_binary_expression(bin_expr->rhs, head, type);
	}

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	char *entity1;
	if (bin_expr->lhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		char value[14];
		sprintf(value, "(%d)", head->index - 1);
		entity1 = strdup(value);
	} else {
		entity1 = generate_ir_entity(bin_expr->lhs);
	}
	char *entity2 = generate_ir_entity(bin_expr->rhs);

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

	char *entity1 = generate_ir_entity(un_expr->rhs);

	new_table->arg1 = entity1;
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->un_op = un_expr->rhs->type;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_function_arguments(struct mcc_ast_function_arguments *args, struct mcc_ir_head *head)
{
	assert(head);

	struct mcc_ast_function_arguments *list = args;
	while (list != NULL) {
		generate_ir_expression(list->expression, head, MCC_IR_TABLE_PUSH);
		list = list->next_argument;
	}
}

static void generate_ir_function_call(struct mcc_ast_expression *expr_call, struct mcc_ir_head *head)
{
	assert(expr_call);
	assert(head);

	// built in functions
	char *x[] = {"print_nl", "read_int", 0}; // TODO rest of built in functions
	int i = 0;
	while (x[i]) {
		if (strcmp(x[i], expr_call->function_call_identifier->identifier->name) == 0) {
			generate_ir_identifier(expr_call->function_call_identifier->identifier, head,
			                       MCC_IR_TABLE_BUILT_IN);
			break;
		}
		i++;
	}

	// normal functions
	generate_function_arguments(expr_call->function_call_arguments, head);

	struct mcc_ast_func_list *list = head->program->function_list;
	// search for main function
	while (list != NULL) {
		if (strcmp(list->function->func_identifier->identifier->name,
		           expr_call->function_call_identifier->identifier->name) == 0) {
			generate_function_definition(list->function, head);
			break;
		}
		list = list->next_function;
	}
}

static void generate_ir_array_access(struct mcc_ast_expression *id_expr,
                                     struct mcc_ast_expression *array_expr,
                                     struct mcc_ir_head *head,
                                     enum ir_table_operation_type type)
{
	assert(id_expr);
	assert(array_expr);
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();

	char *entity1 = generate_ir_entity(id_expr);
	char value[14] = {0};
	sprintf(value, "%s[]", entity1);

	new_table->arg1 = strdup(value);
	new_table->arg2 = NULL;
	new_table->op_type = type;
	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void
generate_ir_expression(struct mcc_ast_expression *expr, struct mcc_ir_head *head, enum ir_table_operation_type type)
{
	assert(expr);
	assert(head);

	switch (expr->type) {
	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		generate_ir_binary_expression(expr, head, MCC_IR_TABLE_BINARY_OP);
		break;
	case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
		generate_ir_unary_expression(expr, head, MCC_IR_TABLE_UNARY_OP);
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
		generate_ir_array_access(expr->array_access_id, expr->array_access_exp, head, MCC_IR_TABLE_LOAD);
		break;
	default:
		printf("todo\n");
		break;
	}
}

static void generate_ir_assignment(struct mcc_ast_declare_assign *assign, struct mcc_ir_head *head)
{
	assert(assign);
	assert(head);

	char *entity2;
	char value[14] = {0};
	if (assign->assign_rhs->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
		generate_ir_expression(assign->assign_rhs, head, -1);
		sprintf(value, "(%d)", head->index);
		entity2 = strdup(value);
	} else {
		entity2 = generate_ir_literal_entity(assign->assign_rhs->literal);
	}

	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity1;
	if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		entity1 = generate_ir_entity(assign->assign_lhs);
		new_table->op_type = MCC_IR_TABLE_ASSIGNMENT;
	} else if (assign->assign_lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS) {
		char *id = assign->assign_lhs->array_access_id->identifier->name;
		if (assign->assign_lhs->array_access_exp->type != MCC_AST_EXPRESSION_TYPE_LITERAL) {
			generate_ir_expression(assign->assign_lhs->array_access_exp, head, -1);
			sprintf(value, "%s[(%d)]", id, head->index);
		} else {
			char *lit = generate_ir_literal_entity(assign->assign_lhs->array_access_exp->literal);
			sprintf(value, "%s[%s]", id, lit);
		}
		entity1 = strdup(value);
		new_table->op_type = MCC_IR_TABLE_STORE;
	}

	head->index++;

	new_table->arg1 = entity1;
	new_table->arg2 = entity2;

	new_table->index = head->index;

	head->current->next_table = new_table;
	head->current = new_table;
}

static void generate_ir_return(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
	assert(expr);
	assert(head);

	// push
	generate_ir_expression(expr, head, MCC_IR_TABLE_PUSH);

	char value[14] = {0};

	// insert additional line in IR table
	if (expr->type != MCC_AST_EXPRESSION_TYPE_LITERAL && expr->type != MCC_AST_EXPRESSION_TYPE_IDENTIFIER) {
		sprintf(value, "(%d)", head->index - 1);
		generate_ir_table_line(head, value, NULL, MCC_IR_TABLE_PUSH);
	}

	// jump
	sprintf(value, "(%d)", head->index + 2);
	generate_ir_table_line(head, value, NULL, MCC_IR_TABLE_JUMP);

	// pop
	sprintf(value, "(%d)", head->index);
	generate_ir_table_line(head, value, NULL, MCC_IR_TABLE_POP);
}

static void generate_ir_if(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
	assert(stmt);
	assert(head);

	char value[12] = {0};

	char *jump_loc;
	char *jump_false_loc;

	// if condition
	generate_ir_expression(stmt->if_cond, head, -1);

	struct mcc_ir_table *jumpfalse_table = create_new_ir_table();
	char *entity1;

	sprintf(value, "(%d)", head->current->index);
	entity1 = strdup(value);

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

	// set jump false
	sprintf(value, "(%d)", head->current->index + 1);
	jump_false_loc = strdup(value);
	jumpfalse_table->arg2 = jump_false_loc;

	// else body
	generate_ir_statement(stmt->else_stat, head);

	// set jump loc
	sprintf(value, "(%d)", head->current->index + 1);
	jump_loc = strdup(value);
	jump_table->arg1 = jump_loc;
}

static void generate_ir_while(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
	assert(stmt);
	assert(head);

	char value[12] = {0};

	char *jump_loc;
	char *jump_false_loc;

	// while condition
	generate_ir_expression(stmt->while_cond, head, -1);

	struct mcc_ir_table *jumpfalse_table = create_new_ir_table();
	char *entity1;

	sprintf(value, "(%d)", head->current->index);
	entity1 = strdup(value);

	head->index++;
	jumpfalse_table->arg1 = entity1;
	jumpfalse_table->arg2 = jump_false_loc;
	jumpfalse_table->op_type = MCC_IR_TABLE_JUMPFALSE;
	jumpfalse_table->index = head->index;

	head->current->next_table = jumpfalse_table;
	head->current = jumpfalse_table;

	// set jump loc
	sprintf(value, "(%d)", head->current->index);
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

	head->current->next_table = jump_table;
	head->current = jump_table;

	// set jump false
	sprintf(value, "(%d)", head->current->index + 1);
	jump_false_loc = strdup(value);
	jumpfalse_table->arg2 = jump_false_loc;
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
	case MCC_AST_STATEMENT_COMPOUND: {
		struct mcc_ast_statement_list *list = stmt->compound;
		while (list != NULL) {
			generate_ir_statement(list->statement, head);
			list = list->next_statement;
		}
	} break;
	default:
		printf("todo\n");
		break;
	}
}

static void generate_ir_param(struct mcc_ast_parameter *param, struct mcc_ir_head *head)
{
	assert(param);
	assert(head);

	head->index++;
	struct mcc_ir_table *new_table = create_new_ir_table();
	char *entity = generate_ir_entity(param->parameter->declare_id);

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
	char *id_entity = generate_ir_entity(func->func_identifier);

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

struct mcc_ir_table *mcc_create_ir(struct mcc_ast_program *program)
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
	head->labelIndex = 1;
	head->program = program;

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
		generate_function_definition(program->function, head);
		break;

	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST: {
		struct mcc_ast_func_list *list = program->function_list;
		// search for main function
		while (list != NULL) {
			if (strcmp(list->function->func_identifier->identifier->name, "main") == 0) {
				generate_function_definition(list->function, head);
				break;
			}
			list = list->next_function;
		}
	} break;
	default:
		break;
	}

	return table;
}
