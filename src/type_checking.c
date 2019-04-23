#include "mcc/type_checking.h"

static void check_assignment(struct mcc_ast_declare_assign *declare_assign,
                             void *data)
{
	assert(declare_assign);
	assert(data);

	struct mcc_type_checking *type_checking = data;

	struct mcc_ast_expression *lhs = declare_assign->assign_lhs;
    struct mcc_ast_expression *rhs = declare_assign->assign_rhs;
    
    if(lhs->expression_type != rhs->expression_type){
        printf("error, expected type '%d', but got type '%d'\n", lhs->expression_type, rhs->expression_type);
    }

}

static void set_type(struct mcc_ast_expression *expr, enum mcc_ast_type type){
    assert(expr);

    expr->expression_type = type;
}

static void check_expression_literal(struct mcc_ast_expression *expr, void *data){
    assert(expr);
    assert(data);

    switch (expr->literal->type) {

	case MCC_AST_TYPE_BOOL:
		set_type(expr, MCC_AST_TYPE_BOOL);
		break;

	case MCC_AST_TYPE_INT:
		set_type(expr, MCC_AST_TYPE_INT);
		break;

	case MCC_AST_TYPE_FLOAT:
		set_type(expr, MCC_AST_TYPE_FLOAT);
		break;

	case MCC_AST_TYPE_STRING:
		set_type(expr, MCC_AST_TYPE_STRING);
		break;

	case MCC_AST_TYPE_VOID:
		set_type(expr, MCC_AST_TYPE_VOID);
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
	};
}


void mcc_check_types(struct mcc_ast_program *program,
                        struct mcc_symbol_table *symbol_table)
{
	assert(program);
	assert(symbol_table);

	struct mcc_type_checking *type_checking = malloc(sizeof(*type_checking));
	if (!type_checking) {
		return;
	}
	type_checking->symbol_table = symbol_table;

	struct mcc_ast_visitor visitor = type_checking_visitor(type_checking);
	mcc_ast_visit_program(program, &visitor);
	free(type_checking);
}
