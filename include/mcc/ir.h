#ifndef THECOMPILER_IR_H
#define THECOMPILER_IR_H

#include "mcc/ast.h"

enum ir_table_operation_type {
    MCC_IR_TABLE_UNARY_OP,
    MCC_IR_TABLE_BINARY_OP,
    MCC_IR_TABLE_ASSIGNMENT,
    MCC_IR_TABLE_JUMPFALSE,
    MCC_IR_TABLE_JUMP,
    MCC_IR_TABLE_LABEL,
    MCC_IR_TABLE_COPY,
};

struct mcc_ir_table{
    enum ir_table_operation_type op_type;
    union
    {
        enum mcc_ast_binary_op bin_op;
        enum mcc_ast_unary_op un_op;
    };
    

    struct mcc_ir_entity *arg1;
    struct mcc_ir_entity *arg2;

    int index; 

    struct mcc_ast_node node;

    struct mcc_ir_table *next_table;    
};

enum mcc_ir_entity_type {
    MCC_IR_ENTITY_TYPE_LITERAL, 
    MCC_IR_ENTITY_TYPE_IDENTIFIER, 
    MCC_IR_ENTITY_TYPE_INDEX
    };

struct mcc_ir_entity{
    enum mcc_ir_entity_type type;
    
    char *lit;
};

struct mcc_ir_head{
    struct mcc_ir_table *root;
    struct mcc_ir_table *current;
    int index;
};

struct mcc_ast_visitor generate_ir_visitor(struct mcc_ir_head *head);
struct mcc_ir_table *mcc_create_ir(struct mcc_ast_program *program);

#endif