// 102-Busl Interpreter (version 0.1)
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define JIT_MEMORY_CAP (10 * 1000 * 1000)

enum operation_kind {
	OP_INCREMENT,
	OP_DECREMENT,
	OP_MOVE_LEFT,
	OP_MOVE_RIGHT,
	OP_OUTPUT,
	OP_INPUT,
	OP_JUMP_IF_ZERO,
	OP_JUMP_IF_NONZERO,
};

struct operation {
	enum operation_kind kind;
	size_t operand;
};

struct operations {
	struct operation *items;
	size_t count;
	size_t capacity;
};

struct address_stack {
	size_t *items;
	size_t count;
	size_t capacity;
};

struct lexer {
	const char *content;
	size_t position;
	size_t length;
};

struct command_mapping {
	const char *command_string;
	enum operation_kind kind;
};

static const struct command_mapping COMMAND_MAPPINGS[] = {
	{ "ROUTE", OP_INCREMENT },
	{ "102", OP_DECREMENT },
	{ "MARKHAM", OP_MOVE_LEFT },
	{ "ROAD", OP_MOVE_RIGHT },
	{ "SOUTHBOUND", OP_OUTPUT },
	{ "TOWARDS", OP_INPUT },
	{ "WARDEN", OP_JUMP_IF_ZERO },
	{ "STATION", OP_JUMP_IF_NONZERO },
	{ NULL, 0 }
};

static const char *lexer_next_command(struct lexer *lex)
{
	while (lex->position < lex->length) {
		const char *current_position = lex->content + lex->position;
		for (const struct command_mapping *mapping = COMMAND_MAPPINGS;
		     mapping->command_string; mapping++) {
			size_t command_length = strlen(mapping->command_string);
			if (strncmp(current_position, mapping->command_string,
				    command_length) == 0) {
				lex->position += command_length;
				return mapping->command_string;
			}
		}
		lex->position++;
	}
	return NULL;
}

static void operations_init(struct operations *ops)
{
	ops->items = NULL;
	ops->count = 0;
	ops->capacity = 0;
}

static void operations_append(struct operations *ops, struct operation op)
{
	if (ops->count == ops->capacity) {
		size_t new_capacity = ops->capacity ? ops->capacity * 2 : 16;
		ops->items = realloc(ops->items,
				     new_capacity * sizeof(struct operation));
		assert(ops->items != NULL);
		ops->capacity = new_capacity;
	}
	ops->items[ops->count++] = op;
}

static void operations_free(struct operations *ops)
{
	free(ops->items);
	ops->items = NULL;
	ops->count = 0;
	ops->capacity = 0;
}

static bool parse_operations(const char *source, struct operations *ops)
{
	struct lexer lex = { source, 0, strlen(source) };
	struct address_stack loop_stack = { NULL, 0, 0 };
	operations_init(ops);

	const char *command;
	while ((command = lexer_next_command(&lex)) != NULL) {
		for (const struct command_mapping *mapping = COMMAND_MAPPINGS;
		     mapping->command_string; mapping++) {
			if (strcmp(command, mapping->command_string) == 0) {
				switch (mapping->kind) {
				case OP_INCREMENT:
				case OP_DECREMENT:
				case OP_MOVE_LEFT:
				case OP_MOVE_RIGHT: {
					size_t repeat_count = 1;
					while ((command = lexer_next_command(
							&lex)) &&
					       strcmp(command,
						      mapping->command_string) ==
						       0) {
						repeat_count++;
					}
					operations_append(
						ops,
						(struct operation){
							.kind = mapping->kind,
							.operand =
								repeat_count });
					if (command)
						lex.position -= strlen(command);
					break;
				}
				case OP_OUTPUT:
				case OP_INPUT:
					operations_append(
						ops,
						(struct operation){
							.kind = mapping->kind,
							.operand = 1 });
					break;
				case OP_JUMP_IF_ZERO:
					operations_append(
						ops,
						(struct operation){
							.kind = mapping->kind,
							.operand = 0 });
					if (loop_stack.count ==
					    loop_stack.capacity) {
						size_t new_capacity =
							loop_stack.capacity ?
								loop_stack.capacity *
									2 :
								16;
						loop_stack.items = realloc(
							loop_stack.items,
							new_capacity *
								sizeof(size_t));
						assert(loop_stack.items !=
						       NULL);
						loop_stack.capacity =
							new_capacity;
					}
					loop_stack.items[loop_stack.count++] =
						ops->count - 1;
					break;
				case OP_JUMP_IF_NONZERO:
					if (loop_stack.count == 0) {
						fprintf(stderr,
							"err: unmatched 'STATION'.\n");
						operations_free(ops);
						free(loop_stack.items);
						return false;
					}
					size_t matching_jump_index =
						loop_stack.items
							[--loop_stack.count];
					operations_append(
						ops,
						(struct operation){
							.kind = mapping->kind,
							.operand =
								matching_jump_index });
					ops->items[matching_jump_index].operand =
						ops->count;
					break;
				}
				break;
			}
		}
	}

	if (loop_stack.count > 0) {
		fprintf(stderr, "err: unmatched 'WARDEN'.\n");
		operations_free(ops);
		free(loop_stack.items);
		return false;
	}

	free(loop_stack.items);
	return true;
}

static bool interpret_operations(const struct operations *ops)
{
	uint8_t *memory = calloc(JIT_MEMORY_CAP, sizeof(uint8_t));
	if (!memory) {
		fprintf(stderr, "err: mem allocation failed :(.\n");
		return false;
	}

	size_t memory_pointer = 0;
	for (size_t instruction_pointer = 0;
	     instruction_pointer < ops->count;) {
		struct operation op = ops->items[instruction_pointer];
		switch (op.kind) {
		case OP_INCREMENT:
			memory[memory_pointer] += (uint8_t)op.operand;
			instruction_pointer++;
			break;
		case OP_DECREMENT:
			memory[memory_pointer] -= (uint8_t)op.operand;
			instruction_pointer++;
			break;
		case OP_MOVE_LEFT:
			if (memory_pointer < op.operand) {
				fprintf(stderr,
					"err: mem ptr out of bounds.\n");
				free(memory);
				return false;
			}
			memory_pointer -= op.operand;
			instruction_pointer++;
			break;
		case OP_MOVE_RIGHT:
			if (memory_pointer + op.operand >= JIT_MEMORY_CAP) {
				fprintf(stderr,
					"err: mem ptr out of bounds.\n");
				free(memory);
				return false;
			}
			memory_pointer += op.operand;
			instruction_pointer++;
			break;
		case OP_OUTPUT:
			fwrite(&memory[memory_pointer], 1, 1, stdout);
			instruction_pointer++;
			break;
		case OP_INPUT:
			fread(&memory[memory_pointer], 1, 1, stdin);
			instruction_pointer++;
			break;
		case OP_JUMP_IF_ZERO:
			instruction_pointer = memory[memory_pointer] ?
						      instruction_pointer + 1 :
						      op.operand;
			break;
		case OP_JUMP_IF_NONZERO:
			instruction_pointer = memory[memory_pointer] ?
						      op.operand :
						      instruction_pointer + 1;
			break;
		}
	}

	free(memory);
	return true;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "use: %s./busl-jit [program.bus]\n", argv[0]);
		return 1;
	}

	const char *file_path = argv[1];
	FILE *file = fopen(file_path, "r");
	if (!file) {
		fprintf(stderr, "err: cant open file :( %s\n", file_path);
		return 1;
	}

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *source_code = malloc(file_size + 1);
	if (!source_code) {
		fprintf(stderr, "err: mem allocation failed for src :( .\n");
		fclose(file);
		return 1;
	}

	fread(source_code, 1, file_size, file);
	source_code[file_size] = '\0';
	fclose(file);

	struct operations ops;
	if (!parse_operations(source_code, &ops)) {
		free(source_code);
		return 1;
	}

	free(source_code);

	if (!interpret_operations(&ops)) {
		operations_free(&ops);
		return 1;
	}

	operations_free(&ops);
	return 0;
}