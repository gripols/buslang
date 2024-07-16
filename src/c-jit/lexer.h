#ifndef LIB_LEXER_H
#define LIB_LEXER_H

#include <stdio.h>

#include "token.h"

struct bus_lexer {
	FILE *src;
	size_t line;
	size_t column;
};

void bus_lexer_init(struct bus_lexer *lexer, FILE *src);

struct bus_token bus_token_next_token(struct bus_lexer *lexer);

#endif // LIB_LEXER_H
