#include "mcc/ast.h"
#include "mcc/print_ir.h"

struct mcc_ir_table{
    /* operation */
    union{
        enum mcc_ast_binary_op bin_op;
        enum mcc_ast_unary_op un_op;
        enum ir_table_operation_type ir_op;
        enum mcc_ast_declare_assign_type da_op;
    } operator;
    
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
