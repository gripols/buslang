# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Ilib -Wall -Wpedantic -Wextra -g

# Source files
SRC = src/buffer.c src/context.c src/error.c src/jit_comp.c src/jit_run.c src/main.c src/lexer.c src/token.c cmd/hit-and-run.c

# Header files
HEADERS = cmd/log.h lib/buffer.h lib/context.h lib/error.h lib/jit.h lib/lexer.h lib/target.h lib/token.h cmd/hit-and-run.h

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = busl

# Default target
all: $(EXEC)

# Rule to link object files to create the executable
$(EXEC): $(OBJ)
	@echo "Linking $@"
	$(CC) $(OBJ) -o $(EXEC)

# Rule to compile .c files to .o files
src/%.o: src/%.c $(HEADERS)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -f src/*.o $(EXEC)

# Phony targets (targets that are not files)
.PHONY: all clean
