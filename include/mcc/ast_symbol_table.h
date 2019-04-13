// Symbol Table Infrastructure
//
// This module provides basic symbol table infrastructure for the symbol table data structure.

#ifndef MCC_AST_SYMBOL_TABLE_H
#define MCC_AST_SYMBOL_TABLE_H

#include "mcc/ast.h"
#include "mcc/ast_visit.h"
#include <assert.h>
#include <mcc/ast_print.h>
#include <stdio.h>

struct mcc_ast_symbol_table {
};

// struct mcc_ast_symbol_declaration {
// };

struct mcc_ast_symbol_table *mcc_ast_create_symbol_table(struct mcc_ast_program *program);

// struct mcc_ast_symbol_declaration *mcc_ast_lookup_function_symbol(struct mcc_ast_symbol_table *symbol_table,
//                                                                   char *symbol);

// struct mcc_ast_symbol_declaration *mcc_ast_lookup_symbol(struct mcc_ast_symbol_table *symbol_table, char *symbol);

void mcc_ast_delete_symbol_table(struct mcc_ast_symbol_table *symbol_table);

#endif // MCC_AST_SYMBOL_TABLE_H
