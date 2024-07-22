// Basic interpreter written in C
// rest of this repo is this but fancier.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 65536

typedef struct {
	unsigned char memory[MEMORY_SIZE];
	size_t data_pointer;
	char **source_code;
	size_t source_code_len;
} Interpreter;

Interpreter new_interpreter(char **source_code, size_t source_code_len)
{
	Interpreter interp;
	interp.data_pointer = 0;
	interp.source_code = source_code;
	interp.source_code_len = source_code_len;
	memset(interp.memory, 0, MEMORY_SIZE);
	return interp;
}

void increment_data_pointer(Interpreter *interp)
{
	if (interp->data_pointer < MEMORY_SIZE - 1) {
		interp->data_pointer++;
	}
}

void decrement_data_pointer(Interpreter *interp)
{
	if (interp->data_pointer > 0) {
		interp->data_pointer--;
	}
}

void increment_value(Interpreter *interp)
{
	interp->memory[interp->data_pointer]++;
}

void decrement_value(Interpreter *interp)
{
	interp->memory[interp->data_pointer]--;
}

void write_to_output(Interpreter *interp)
{
	putchar(interp->memory[interp->data_pointer]);
}

void read_from_input(Interpreter *interp)
{
	interp->memory[interp->data_pointer] = getchar();
}

void debug_handler(Interpreter *interp)
{
	size_t data_pointer_addr =
		(size_t)&interp->source_code + interp->data_pointer;
	size_t memory_addr = (size_t)&interp->memory + interp->data_pointer;
	unsigned char memory_value = interp->memory[interp->data_pointer];

	printf("\x1b[1;34mdebug: \x1b[0mip \x1b[36m=\x1b[0m %zx \x1b[90m(\x1b[0mprogram "
	       "\x1b[36m+\x1b[0m %zx\x1b[90m)\x1b[0m\x1b[36m;\x1b[0m dp\x1b[36m\x1b[0m "
	       "\x1b[36m=\x1b[0m %zx \x1b[90m(\x1b[0mmemory \x1b[36m+\x1b[0m "
	       "%zx\x1b[90m)\x1b[0m\x1b[36m;\x1b[0m \x1b[36m*\x1b[0mdp \x1b[36m=\x1b[0m "
	       "%u\n",
	       data_pointer_addr, interp->data_pointer, memory_addr,
	       interp->data_pointer, memory_value);
}

void execute(Interpreter *interp)
{
	size_t i = 0;

	while (i < interp->source_code_len) {
		if (strcmp(interp->source_code[i], "ROUTE") == 0)
			increment_data_pointer(interp);
		else if (strcmp(interp->source_code[i], "102") == 0)
			decrement_data_pointer(interp);
		else if (strcmp(interp->source_code[i], "MARKHAM") == 0)
			increment_value(interp);
		else if (strcmp(interp->source_code[i], "ROAD") == 0)
			decrement_value(interp);
		else if (strcmp(interp->source_code[i], "SOUTHBOUND") == 0)
			write_to_output(interp);
		else if (strcmp(interp->source_code[i], "TOWARDS") == 0)
			read_from_input(interp);
		else if (strcmp(interp->source_code[i], "WARDEN") == 0) {
			if (interp->memory[interp->data_pointer] == 0) {
				size_t count = 1;
				while (count > 0) {
					i++;
					if (strcmp(interp->source_code[i],
						   "WARDEN") == 0)
						count++;
					else if (strcmp(interp->source_code[i],
							"STATION") == 0)
						count--;
				}
			}
		} else if (strcmp(interp->source_code[i], "STATION") == 0) {
			if (interp->memory[interp->data_pointer] != 0) {
				size_t count = 1;
				while (count > 0) {
					i--;
					if (strcmp(interp->source_code[i],
						   "STATION") == 0)
						count++;
					else if (strcmp(interp->source_code[i],
							"WARDEN") == 0)
						count--;
				}
			}
		} else if (strcmp(interp->source_code[i], "PRESTO") == 0) {
			debug_handler(interp);
		} else {
			fprintf(stderr, "unknown yap detected\n");
		}
		i++;
	}
}

void cleanup_interpreter(Interpreter *interp)
{
	// Free DMA'd src memory if need
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
		return 1;
	}

	FILE *input_file = fopen(argv[1], "r");
	if (!input_file) {
		fprintf(stderr, "Error opening the file: %s\n", argv[1]);
		return 1;
	}

	char source_code[1024]; // Adjust size as needed
	char *tokens[1024]; // Adjust size as needed
	size_t token_count = 0;

	while (fgets(source_code, sizeof(source_code), input_file)) {
		// Remove comments starting with ';'
		char *comment_start = strchr(source_code, ';');
		if (comment_start)
			*comment_start = '\0';

		// Split by whitespace
		char *token = strtok(source_code, " \t\n\r");
		while (token) {
			tokens[token_count++] = strdup(token);
			token = strtok(NULL, " \t\n\r");
		}
	}

	Interpreter interp = new_interpreter(tokens, token_count);
	execute(&interp);
	cleanup_interpreter(&interp);

	return 0;
}