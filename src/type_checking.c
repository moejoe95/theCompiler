#include "mcc/type_checking.h"

static struct mcc_ast_visitor type_checking_visitor(void *data)
{

	return (struct mcc_ast_visitor){
		.traversal = MCC_AST_VISIT_DEPTH_FIRST,
		.order = MCC_AST_VISIT_POST_ORDER,

		.userdata = data,

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
