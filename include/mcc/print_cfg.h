#ifndef MCC_PRINT_CFG_H
#define MCC_PRINT_CFG_H

#include "mcc/cfg.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mcc/ast.h>

void print_cfg(struct mcc_ir_line *ir, struct mcc_cfg *cfg, FILE *out);

#endif // MCC_PRINT_CFG_H