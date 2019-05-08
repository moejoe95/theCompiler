#include "mcc/ir.h"
#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include "assert.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

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

int nodecompare(struct mcc_ast_node node1, struct mcc_ast_node node2){
    if (node1.sloc.start_col == node2.sloc.start_col &&
        node1.sloc.end_col == node2.sloc.end_col &&
        node1.sloc.start_line == node2.sloc.start_line &&
        node1.sloc.end_line == node2.sloc.end_line)
        return 0;
    return 1;
}

static struct mcc_ir_entity *mcc_ir_lookup_entity(struct mcc_ir_head *head, struct mcc_ast_node node)
{
    assert(head);

    struct mcc_ir_table *current_table = head->root->next_table;
    struct mcc_ir_entity *entity = create_new_ir_entity();
    while(current_table != NULL){
        if (nodecompare(current_table->node, node) == 0){
            char value[14] = {0};
            sprintf(value, "(%d)", current_table->index);
            entity->lit = strdup(value);
            break;
        }
        current_table = current_table->next_table;
    }

    return entity;
}

static struct mcc_ir_entity *generate_ir_identifier_expression(struct mcc_ast_identifier *id_expr)
{
    assert(id_expr);
    struct mcc_ir_entity *entity = create_new_ir_entity();
    entity->type = MCC_IR_TYPE_IDENTIFIER_EXPRESSION;
    entity->lit = strdup(id_expr->name);
    return entity;
}

static struct mcc_ir_entity *generate_ir_literal_expression(struct mcc_ast_literal *lit)
{
    assert(lit);
    struct mcc_ir_entity *entity = create_new_ir_entity();
    entity->type = MCC_IR_TYPE_LITERAL_EXPRESSION;
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

static struct mcc_ir_entity *generate_ir_expression(struct mcc_ast_expression *expr){
    struct mcc_ir_entity *entity = NULL;
    switch (expr->type)
    {
    case MCC_AST_EXPRESSION_TYPE_LITERAL:
        entity = generate_ir_literal_expression(expr->literal);
        break;
    case MCC_AST_EXPRESSION_TYPE_IDENTIFIER:
        entity = generate_ir_identifier_expression(expr->identifier);
        break;
    default:
        break;
    }
    return entity;
}

static void generate_ir_binary_expression(struct mcc_ast_expression *bin_expr, void *data)
{
    assert(bin_expr);
    assert(data);

    struct mcc_ir_head *head = data;
    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = generate_ir_expression(bin_expr->lhs);
    struct mcc_ir_entity *entity2 = generate_ir_expression(bin_expr->rhs);

    new_table->arg1 = entity1;
    new_table->arg2 = entity2;
    new_table->operator.bin_op = bin_expr->op;
    new_table->node = bin_expr->node;
    new_table->index = head->index;
    

    head->current->next_table = new_table;    
    head->current = new_table;

    print_table(MCC_IR_TABLE_BINARY_OP, head->index, entity1->lit, entity2->lit, bin_expr);
}

static void generate_ir_unary_expression(struct mcc_ast_expression *un_expr, void *data)
{
    assert(un_expr);
    assert(data);

    struct mcc_ir_head *head = data;
    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = generate_ir_expression(un_expr->rhs);

    new_table->arg1 = entity1;
    new_table->operator.un_op = un_expr->u_op;
    new_table->node = un_expr->node;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_UNARY_OP, head->index, entity1->lit, NULL, un_expr);
}


static void generate_ir_assignment(struct mcc_ast_declare_assign *assign, void *data)
{
    assert(assign);
    assert(data);

    struct mcc_ir_head *head = data;
    head->index++;
    struct mcc_ir_table *new_table = create_new_ir_table();

    struct mcc_ir_entity *entity1 = mcc_ir_lookup_entity(head, assign->assign_rhs->node);

    new_table->arg1 = entity1;
    new_table->operator.un_op = assign->type;
    new_table->index = head->index;
    
    head->current->next_table = new_table;
    head->current = new_table;

    print_table(MCC_IR_TABLE_ASSIGNMENT, head->index, entity1->lit, NULL, NULL);
}


struct mcc_ast_visitor generate_ir_visitor(struct mcc_ir_head *head)
{
	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_POST_ORDER,

        .userdata = head,

        .expression_binary_op = generate_ir_binary_expression,
        .expression_unary_op = generate_ir_unary_expression,
        .assignment = generate_ir_assignment,
	};
}

struct mcc_ir_table *mcc_create_ir(struct mcc_ast_program *program)
{
    assert(program);

	struct mcc_ir_table *table = create_new_ir_table();
    struct mcc_ir_head *head = malloc(sizeof(*head));
    if(!head) return NULL;
    table->index = 0;
    head->root = table;
    head->current = table;
    head->index = 0;

    print_table_legend();

	struct mcc_ast_visitor visitor = generate_ir_visitor(head);
	mcc_ast_visit(program, &visitor);

    free(head);
    free(table);
	return table;
}

