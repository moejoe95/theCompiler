#ifndef MCC_TYPE_CHECKING_H
#define MCC_TYPE_CHECKING_H

#include "mcc/ast_print.h"
#include "mcc/symbol_table.h"
#include "mcc/ast.h"


struct mcc_type_checking {
	struct mcc_symbol_table *symbol_table;
	struct mcc_ast_func_definition *current_function;
};


void mcc_check_types(struct mcc_ast_program *program,
                        struct mcc_symbol_table *symbol_table);

#endif