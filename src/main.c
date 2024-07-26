#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

bool help;

void help_man(FILE *file, const char *argv0)
{
	fprintf(file, "Usage: %s [options] <.bus file>\n", argv0);
}

int opts_init(int argc, char *argv[])
{
	help = false;

	int opt;

	while ((opt = getopt(argc, argv, "h?")) != -1) {
		switch (opt) {
		case 'h':
		case '?':
			help = true;
			help_man(stdout, argv[0]);
			return EXIT_SUCCESS;
		default:
			help_man(stderr, argv[0]);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int status = opts_init(argc, argv);

	if (argc < 2) {
		help_man(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	if (status != EXIT_SUCCESS)
		return status;

	if (help)
		return EXIT_SUCCESS;

	return 0;
}
