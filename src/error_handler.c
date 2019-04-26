#include "mcc/error_handler.h"

#include <stdio.h>
#include <stdlib.h>

void print_lexer_error(char* filename, int last_line, int last_column, const char *msg){
    fprintf(stderr, "%s:%d:%d: error: %s \n", filename, last_line, last_column, msg);
}

void print_scanner_error(char* filename, int last_line, int last_column, char error_char){
	fprintf(stderr, "%s:%d:%d: unknown character '%c'\n", filename, last_line, last_column, error_char);
}

void print_semantic_error(struct mcc_semantic_error *semantic_error){
    switch (semantic_error->error_type) {

		case MCC_SC_ERROR_NO_MAIN:
			fprintf(stderr, "no main\n");
			/*fprintf(stderr, "%s:%d:%d: error: %s \n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line, 
			semantic_error->sloc->end_col, "no main function found");*/
			break;
	}
}

struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type, struct mcc_ast_source_location *sloc){
	struct mcc_semantic_error *semantic_error = malloc(sizeof(*semantic_error));
	if (!semantic_error) {
		return NULL;
	}
	semantic_error->error_type = error_type;
	semantic_error->sloc = sloc;

	return semantic_error;
}


