#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token.h"

struct bus_lexer {
	FILE *src;
	size_t line;
	size_t column;
};

void bus_lexer_init(struct bus_lexer *lexer, FILE *src);

struct bus_token bus_lexer_next_token(struct bus_lexer *lexer);

#endif // LEXER_H
