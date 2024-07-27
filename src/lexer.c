#include "../lib/lexer.h"
#include "../lib/token.h"

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

void bus_lexer_init(struct bus_lexer *lexer, FILE *src)
{
	lexer->src = src;
	lexer->line = 1;
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

	ungetc('\n', lexer->src);
}

static void lexer_new_line(struct bus_lexer *lexer)
{
	lexer->line++;
	lexer->column = 0;
}

static enum bus_token_type bus_lexer_match_token(struct bus_lexer *lexer,
						 const char *end,
						 size_t end_len,
						 enum bus_token_type type)
{
	for (size_t i = 0; i < end_len; i++) {
		if (lexer_next_char(lexer) != end[i]) {
			return BUS_TOKEN_INVALID;
		}
	}

	if (token_char(lexer_peek_char(lexer))) {
		return BUS_TOKEN_INVALID;
	}

	return type;
}

static enum bus_token_type bus_lexer_next_token_type(struct bus_lexer *lexer,
						     int ch)
{
	switch (ch) {
	case '1':
		return BUS_LEXER_MATCH_TOKEN(lexer, "02", BUS_TOKEN_102);

	case 'M':
		return BUS_LEXER_MATCH_TOKEN(lexer, "ARKHAM",
					     BUS_TOKEN_MARKHAM);

	case 'R':
		ch = lexer_next_char(lexer);
		switch (ch) {
		case 'O':
			ch = lexer_next_char(lexer);
			switch (ch) {
			case 'U':
				return BUS_LEXER_MATCH_TOKEN(lexer, "TE",
							     BUS_TOKEN_ROUTE);
			case 'A':
				return BUS_LEXER_MATCH_TOKEN(lexer, "D",
							     BUS_TOKEN_ROAD);
			default:
				return BUS_TOKEN_INVALID;
			}
			break;
			default:
				return BUS_TOKEN_INVALID;
		}

	case 'S':
		ch = lexer_next_char(lexer);
		switch (ch) {
		case 'O':
			return BUS_LEXER_MATCH_TOKEN(lexer, "UTHBOUND",
						     BUS_TOKEN_SOUTHBOUND);
		case 'T':
			return BUS_LEXER_MATCH_TOKEN(lexer, "ATION",
						     BUS_TOKEN_STATION);

		default:
			return BUS_TOKEN_INVALID;
		}
		break;
				

	case 'T':
		return BUS_LEXER_MATCH_TOKEN(lexer, "OWARDS",
					     BUS_TOKEN_TOWARDS);

	case 'W':
		return BUS_LEXER_MATCH_TOKEN(lexer, "ARDEN", BUS_TOKEN_WARDEN);

	default:
		return BUS_TOKEN_INVALID;
	}
}

struct bus_token bus_lexer_next_token(struct bus_lexer *lexer)
{
	int ch;

	while ((ch = lexer_next_char(lexer)) != EOF) {
		switch (ch) {
		case COMMENT_CHARACTER:
			lexer_skip(lexer);
			break;
		case '\n':
			lexer_new_line(lexer);
			break;
		default:
			if (space(ch)) {
				continue;
			}
			size_t column = lexer->column;
			enum bus_token_type type =
				bus_lexer_next_token_type(lexer, ch);
			return (struct bus_token){ type, lexer->line, column };
		}
	}

	return (struct bus_token){ BUS_TOKEN_EOF, lexer->line, lexer->column };
}
