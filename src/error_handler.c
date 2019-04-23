#include "mcc/error_handler.h"

#include <stdio.h>
#include <stdlib.h>

void print_lexer_error(char* filename, struct MCC_PARSER_LTYPE *yylloc, const char *msg){
    fprintf(stderr, "%s:%d:%d: error: %s \n", filename, yylloc->last_line, yylloc->last_column, msg);
}

void print_scanner_error(char error_char){
    fprintf(stderr, "Scanner Error: unknown character '%c'\n", error_char); 
}

