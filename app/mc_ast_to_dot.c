#include <stdio.h>
#include <stdlib.h>

#include "mcc/ast.h"
#include "mcc/ast_print.h"
#include "mcc/parser.h"
#include <getopt.h>
#include <string.h>

void print_help(const char *prg_name)
{
	printf("usage: %s [OPTIONS] file...\n", prg_name);
	printf("\nUtility for printing an abstract syntax tree in the DOT format. The output\
can be visualised using graphviz. Errors are reported on invalid inputs. \
\n");
	printf("\nUse '-' as input file to read from stdin.\n");
	printf("OPTIONS:\n");
	printf("\t-h,--help \tdisplays this help message\n");
	printf("\t-o,--output <file> \twrite the output to <file> (defaults to stdout)\n");
	printf("\t-f, --function <name> \tlimit scope to the given function\n");
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stdout, "Invalid number of arguments. Try -h or --help for more informatiotion.");
		return EXIT_FAILURE;
	}
	static struct option long_options[] = {{"help", no_argument, NULL, 'h'},
	                                       {"output", optional_argument, NULL, 'o'},
	                                       {"function", optional_argument, NULL, 'f'}};

	char output[64] = {0};
	snprintf(output, sizeof(output), "%s", "-");
	int c;
	while ((c = getopt_long(argc, argv, "ho:f:", long_options, NULL)) != -1)
		switch (c) {
		case 'h':
			print_help(argv[0]);
			return EXIT_SUCCESS;
			break;
		case 'o':
			snprintf(output, sizeof(output), "%s", optarg);
			break;
		case 'f':
			break;
		default:
			return EXIT_FAILURE;
		}

	FILE *f = fopen(argv[optind], "r");

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

	FILE *out;
	if (strcmp("-", output) == 0) {
		out = stdout;
	} else {
		out = fopen(output, "w");
		if (out == NULL) {
			fprintf(stdout, "failed to open file: %s\n", output);
		}
	}
	mcc_ast_print_dot(out, pro);

	// cleanup
	mcc_ast_delete(pro);

	return EXIT_SUCCESS;
}
