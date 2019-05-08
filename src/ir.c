#include "mcc/ir.h"
#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "mcc/print_ir.h"

static struct mcc_ir_table *create_new_ir_table(){
    struct mcc_ir_table *table = malloc(sizeof(*table));
    if(!table){
        return NULL;
    }
    return table;
}

static struct mcc_ir_entity *create_new_ir_entity(){
    struct mcc_ir_entity *entity = malloc(sizeof(*entity));
    if(!entity){
        return NULL;
    }
    return entity;
}

static char *lookup_table_args(struct mcc_ir_head *head, char *arg1, char *arg2)
{
    assert(head);

    struct mcc_ir_table *table = head->root;
    while(table != NULL){
        if (strcmp(arg1, table->arg1->lit) == 0 && strcmp(arg2, table->arg2->lit)){
            char value[12] = {0};
            sprintf(value, "(%d)", table->index);
            return strdup(value);
        }
        table = table->next_table;
    }
    return NULL;
}

static struct mcc_ir_entity *generate_ir_identifier_entity(struct mcc_ast_identifier *id_expr)
{
    assert(id_expr);
    struct mcc_ir_entity *entity = create_new_ir_entity();
    entity->lit = strdup(id_expr->name);
    return entity;
}

static struct mcc_ir_entity *generate_ir_literal_entity(struct mcc_ast_literal *lit)
{
    assert(lit);
    struct mcc_ir_entity *entity = create_new_ir_entity();
    char value[12] = {0};
    switch (lit->type)
    {
    case MCC_AST_LITERAL_TYPE_BOOL:
        sprintf(value, "%d", lit->b_value);
        entity->lit = strdup(value);
        break;
    case MCC_AST_LITERAL_TYPE_INT:
        sprintf(value, "%ld", lit->i_value);
        entity->lit = strdup(value);
        break;
    case MCC_AST_LITERAL_TYPE_FLOAT:
        sprintf(value, "%f", lit->f_value);
        entity->lit = value;    
        break;
    case MCC_AST_LITERAL_TYPE_STRING:
        entity->lit = strdup(lit->str_value);
        break;
    }
    
    return entity;
}

static struct mcc_ir_entity *generate_ir_entity(struct mcc_ast_expression *expr){
    struct mcc_ir_entity *entity = NULL;
    switch (expr->type)
    {
    case MCC_AST_EXPRESSION_TYPE_LITERAL:
        entity = generate_ir_literal_entity(expr->literal);
        break;
    case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
        entity = generate_ir_identifier_entity(expr->identifier);
        break;
    default:
        break;
    }
    return entity;
}

static void generate_ir_binary_expression(struct mcc_ast_expression *bin_expr, struct mcc_ir_head *head)
{
    assert(bin_expr);
    assert(head);

    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = generate_ir_entity(bin_expr->lhs);
    struct mcc_ir_entity *entity2 = generate_ir_entity(bin_expr->rhs);

    new_table->arg1 = entity1;
    new_table->arg2 = entity2;
    new_table->operator.bin_op = bin_expr->op;
    new_table->node = bin_expr->node;
    new_table->index = head->index;
    

    head->current->next_table = new_table;    
    head->current = new_table;

    print_table(MCC_IR_TABLE_BINARY_OP, head->index, entity1->lit, entity2->lit, bin_expr);
}

static void generate_ir_unary_expression(struct mcc_ast_expression *un_expr, struct mcc_ir_head *head)
{
    assert(un_expr);
    assert(head);

    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = generate_ir_entity(un_expr->rhs);

    new_table->arg1 = entity1;
    new_table->operator.un_op = un_expr->u_op;
    new_table->node = un_expr->node;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_UNARY_OP, head->index, entity1->lit, NULL, un_expr);
}

static void generate_ir_literal_expression(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
    assert(expr);
    assert(head);

    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();
    struct mcc_ir_entity *entity = generate_ir_literal_entity(expr->literal);
    new_table->arg1 = entity;
    new_table->operator.ir_op = MCC_IR_TABLE_COPY;
    new_table->index = head->index;

    head->current->next_table = new_table;
    head->current = new_table;
    print_table(MCC_IR_TABLE_COPY, head->index, entity->lit, NULL, NULL);
}

static void generate_ir_identifier_expression(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
    assert(expr);
    assert(head);

    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();
    struct mcc_ir_entity *entity = generate_ir_identifier_entity(expr->identifier);
    new_table->arg1 = entity;
    new_table->operator.ir_op = MCC_IR_TABLE_COPY;
    new_table->index = head->index;

    head->current->next_table = new_table;
    head->current = new_table;
    print_table(MCC_IR_TABLE_COPY, head->index, entity->lit, NULL, NULL);
}

static void generate_ir_declaration(struct mcc_ast_declare_assign *decl, struct mcc_ir_head *head)
{
    assert(decl);
    assert(head);

    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = create_new_ir_entity();
    entity1->lit = decl->declare_id->identifier->name;

    new_table->arg1 = entity1;
    new_table->operator.un_op = decl->type;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_DECLARATION, head->index, entity1->lit, NULL, NULL);
}


static void generate_ir_expression(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
    assert(expr);
    assert(head);

    switch (expr->type)
    {
    case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
        generate_ir_binary_expression(expr, head);
        break;
    case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
        generate_ir_unary_expression(expr, head);
        break;
    case MCC_AST_EXPRESSION_TYPE_LITERAL:
        generate_ir_literal_expression(expr, head);
        break;
    case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
        generate_ir_identifier_expression(expr, head);
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

    // RHS
    generate_ir_expression(assign->assign_rhs, head);

    // LHS
    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();
    struct mcc_ir_entity *entity = create_new_ir_entity();
    char value[14] = {0};
    sprintf(value, "(%d)", head->current->index);
    entity->lit = strdup(value);

    new_table->arg1 = entity;
    new_table->operator.un_op = assign->type;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_ASSIGNMENT, head->index, entity->lit, NULL, NULL);
}

static void generate_ir_return(struct mcc_ast_expression *expr, struct mcc_ir_head *head)
{
    assert(expr);
    assert(head);

    struct mcc_ir_table *new_table = NULL;
    struct mcc_ir_entity *entity = NULL;

    switch (expr->type)
    {
    case MCC_AST_EXPRESSION_TYPE_LITERAL:
        head->index++;
        new_table = create_new_ir_table();
        entity = generate_ir_literal_entity(expr->literal);
        new_table->arg1 = entity;
        new_table->operator.ir_op = MCC_IR_TABLE_JUMP;
        new_table->index = head->index;

        head->current->next_table = new_table;
        head->current = new_table;
        print_table(MCC_IR_TABLE_JUMP, head->index, entity->lit, NULL, NULL);
        break;
    case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
        head->index++;
        new_table = create_new_ir_table();
        entity = generate_ir_identifier_entity(expr->identifier);
        new_table->arg1 = entity;
        new_table->operator.ir_op = MCC_IR_TABLE_JUMP;
        new_table->index = head->index;

        head->current->next_table = new_table;
        head->current = new_table;
        print_table(MCC_IR_TABLE_JUMP, head->index, entity->lit, NULL, NULL);    
        break;
    default:
        printf("todo\n");
        break;
    }
}

static void generate_ir_statement(struct mcc_ast_statement *stmt, struct mcc_ir_head *head)
{
    assert(stmt);
    assert(head);

    switch (stmt->type)
    {
    case MCC_AST_STATEMENT_EXPRESSION:
        generate_ir_expression(stmt->expression, head);
        break;
    case MCC_AST_STATEMENT_DECLARATION:
        generate_ir_declaration(stmt->declare_assign, head);
        break;
    case MCC_AST_STATEMENT_ASSIGNMENT:
        generate_ir_assignment(stmt->declare_assign, head);
        break;
    case MCC_AST_STATEMENT_RETURN:
        generate_ir_return(stmt->expression, head);
        break;
    default:
        printf("todo\n");
        break;
    }
}

static void generate_function_definition(struct mcc_ast_func_definition *func, struct mcc_ir_head *head)
{
    assert(func);
    assert(head);

    // func identifier
    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();
    struct mcc_ir_entity *id_entity = generate_ir_entity(func->func_identifier);

    new_table->arg1 = id_entity;
    new_table->operator.da_op = MCC_IR_TABLE_LABEL;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_LABEL, head->index, id_entity->lit, NULL, NULL);

    // func compound
    struct mcc_ast_statement_list *list = func->func_compound->compound;
    while(list != NULL){
        generate_ir_statement(list->statement, head);
        list = list->next_statement;
    }

    // TODO parameter list
}

struct mcc_ir_table *mcc_create_ir(struct mcc_ast_program *program)
{
    assert(program);

    struct mcc_ir_head *head = malloc(sizeof(*head));
    if(!head) return NULL;

    struct mcc_ir_table *table = create_new_ir_table();
    table->index = 0;

    head->root = table;
    head->current = table;
    head->index = 0;

    print_table_legend();

	switch (program->type) {
	case MCC_AST_PROGRAM_TYPE_FUNCTION:
        generate_function_definition(program->function, head);
		break;

	case MCC_AST_PROGRAM_TYPE_FUNCTION_LIST: {
		struct mcc_ast_func_list *list = program->function_list;
		while (list != NULL) {
            generate_function_definition(list->function, head);
			list = list->next_function;
		}
	} break;
	}

    free(head);
    free(table);
	return table;
}

