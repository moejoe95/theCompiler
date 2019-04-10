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
	static struct option long_options[] = {{"help", no_argument, NULL, 'h'},
	                                       {"output", optional_argument, NULL, 'o'},
	                                       {"function", optional_argument, NULL, 'f'}};

	char outfile[64] = {0};
	snprintf(outfile, sizeof(outfile), "%s", "-");
	int c;
	while ((c = getopt_long(argc, argv, "ho:f:", long_options, NULL)) != -1)
		switch (c) {
		case 'h':
			print_help(argv[0]);
			return EXIT_SUCCESS;
			break;
		case 'o':
			snprintf(outfile, sizeof(outfile), "%s", optarg);
			break;
		case 'f':
			break;
		default:
			return EXIT_FAILURE;
		}

	FILE *in;
	if (strcmp("-", argv[optind]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[optind], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mcc_ast_program *pro = NULL;

	// parsing phase
	{
		struct mcc_parser_result result = mcc_parse_file(in);

		if (result.status != MCC_PARSER_STATUS_OK) {
			fprintf(stdout, "...parsing failed...\n");
			return EXIT_FAILURE;
		}
		pro = result.program;
	}

	FILE *out;
	if (strcmp("-", outfile) == 0) {
		out = stdout;
	} else {
		out = fopen(outfile, "w");
		if (!out) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}
	mcc_ast_print_dot(out, pro);

	// cleanup
	mcc_ast_delete(pro);

	return EXIT_SUCCESS;
}
