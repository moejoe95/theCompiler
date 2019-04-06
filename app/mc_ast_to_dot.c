#include <stdio.h>
#include <stdlib.h>

#include "mcc/ast.h"
#include "mcc/ast_print.h"
#include "mcc/parser.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stdout, "invalid number of arguments");
		return EXIT_FAILURE;
	}
	FILE *f = fopen(argv[1], "r");

	struct mcc_ast_program *pro = NULL;

	// parsing phase
	{
		struct mcc_parser_result result = mcc_parse_file(f);

		if (result.status != MCC_PARSER_STATUS_OK) {
			fprintf(stdout, "...parsing failed...\n");
			return EXIT_FAILURE;
		}
		pro = result.program;
	}

	mcc_ast_print_dot(stdout, pro);

	// cleanup
	mcc_ast_delete(pro);

	return EXIT_SUCCESS;
}
