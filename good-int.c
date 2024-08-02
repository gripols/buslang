// the actually good buslang interpreter

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 65536

typedef struct {
    unsigned char mem[MEMORY_SIZE];
    size_t data_pointer;
    char **src;
    size_t src_len;
} Interpreter;

Interpreter new_interpreter(char **source_code, size_t source_code_len)
{
    Interpreter interp;
    interp.data_pointer = 0;
    interp.src = source_code;
    interp.src_len = source_code_len;
    memset(interp.mem, 0, MEMORY_SIZE);
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
    interp->mem[interp->data_pointer]++;
}

void decrement_value(Interpreter *interp)
{
    interp->mem[interp->data_pointer]--;
}

void write_to_output(Interpreter *interp)
{
    putchar(interp->mem[interp->data_pointer]);
}

void read_from_input(Interpreter *interp)
{
    int ch = getchar();
    if (ch != EOF) {
        interp->mem[interp->data_pointer] = (unsigned char)ch;
    }
}

void execute(Interpreter *interp)
{
    size_t i = 0;

    while (i < interp->src_len) {
        if (strcmp(interp->src[i], "ROUTE") == 0)
            increment_data_pointer(interp);
        else if (strcmp(interp->src[i], "102") == 0)
            decrement_data_pointer(interp);
        else if (strcmp(interp->src[i], "MARKHAM") == 0)
            increment_value(interp);
        else if (strcmp(interp->src[i], "ROAD") == 0)
            decrement_value(interp);
        else if (strcmp(interp->src[i], "SOUTHBOUND") == 0)
            write_to_output(interp);
        else if (strcmp(interp->src[i], "TOWARDS") == 0)
            read_from_input(interp);
        else if (strcmp(interp->src[i], "WARDEN") == 0) {
            if (interp->mem[interp->data_pointer] == 0) {
                size_t count = 1;
                while (count > 0 && i < interp->src_len - 1) {
                    i++;
                    if (strcmp(interp->src[i], "WARDEN") == 0)
                        count++;
                    else if (strcmp(interp->src[i], "STATION") == 0)
                        count--;
                }
            }
        } else if (strcmp(interp->src[i], "STATION") == 0) {
            if (interp->mem[interp->data_pointer] != 0) {
                size_t count = 1;
                while (count > 0 && i > 0) {
                    i--;
                    if (strcmp(interp->src[i], "STATION") == 0)
                        count++;
                    else if (strcmp(interp->src[i], "WARDEN") == 0)
                        count--;
                }
            }
        } else {
            fprintf(stderr, "Unknown command detected\n");
        }
        i++;
    }
}

void cleanup_interpreter(Interpreter *interp)
{
    for (size_t i = 0; i < interp->src_len; i++) {
        free(interp->src[i]);
    }
    free(interp->src);
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
    size_t token_capacity = 1024;
    char **tokens = malloc(token_capacity * sizeof(char *));
    if (!tokens) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(input_file);
        return 1;
    }
    size_t token_count = 0;

    while (fgets(source_code, sizeof(source_code), input_file) != NULL) {
        char *comment_start = strchr(source_code, ';');
        if (comment_start)
            *comment_start = '\0';

        // Split by whitespace
        char *token = strtok(source_code, " \t\n\r");
        while (token) {
            if (token_count >= token_capacity) {
                token_capacity *= 2;
                tokens = realloc(tokens, token_capacity * sizeof(char *));
                if (!tokens) {
                    fprintf(stderr, "Memory allocation error\n");
                    fclose(input_file);
                    return 1;
                }
            }
            tokens[token_count] = strdup(token);
            if (!tokens[token_count]) {
                fprintf(stderr, "Memory allocation error\n");
                fclose(input_file);
                return 1;
            }
            token_count++;
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(input_file);

    Interpreter interp = new_interpreter(tokens, token_count);
    execute(&interp);
    cleanup_interpreter(&interp);

    return 0;
}
