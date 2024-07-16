#include "lexer.h"
#include "token.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define COMMENT_CHARACTER ';'

#define BUS_LEXER_MATCH_TOKEN(lexer, end, type) \
	bus_lexer_match_token(lexer, end, strlen(end), type)

static bool space(int ch)
{
        return ch == '\t' || ch == '\n' || ch == '\r' || ch == ' ';
}

static bool token_char(int ch)
{
        return !space(ch) && ch != COMMENT_CHARACTER && ch != EOF;
}

void lexer_init(struct bus_lexer *lexer, FILE *src)
{
        lexer->src =    src;
        lexer->line =   1;
        lexer->column = 0;
}

static int lexer_next_char(struct bus_lexer *lexer)
{
        lexer->column++;
        return fgetc(lexer->src);
}

static int lexer_peek_char(struct bus_lexer *lexer)
{
        int ch = fgetc(lexer->src);
        ungetc(ch, lexer->src);
        return ch;
}

static void lexer_skip(struct bus_lexer *lexer)
{
        int ch;

        do {
                ch = lexer_next_char(lexer);
        } while (ch != '\n' && ch != EOF);

	ungetc('\n',  lexer->src);
}

static void lexer_new_line(struct bus_lexer *lexer)
{
	lexer->line++;
	lexer->column = 0;
}

static void bus_token_type bus_lexer_match_token(
	struct bus_lexer *lexer,
	const char *end,
	size_t end_len,
	enum bus_token_type type
	)
{

	for (size_t = 0; i < end_len; i++) {
		if (lexer_next_char(lexer) != end[i])
			return BUS_TOKEN_INV; // ???
	}

	if (bus_is_token_char(bus_lexer_peek_char(lexer)))
		return BUS_TOKEN_INVALID;

	return type;
}

static enum bus_token_type
bus_lexer_next_token_type(struct bus_lexer *lexer, int ch) {
	switch (ch) {
	case 'R': return BUS_LEXER_MATCH_TOKEN(lexer, "OUTE", BUS_TOKEN_ROUTE);
        case '1': return BUS_LEXER_MATCH_TOKEN(lexer, "02", BUS_TOKEN_ROUTE);
	case 'M': return BUS_LEXER_MATCH_TOKEN(lexer, "ARKHAM", BUS_TOKEN_ROUTE);
	case 'R': return BUS_LEXER_MATCH_TOKEN(lexer, "OAD", BUS_TOKEN_ROUTE);
	case 'S': ; // i dont feel like doing ts

	}
}
// finish + debug ltr
	
	
