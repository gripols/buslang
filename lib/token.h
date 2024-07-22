#ifndef LIB_TOKEN_H
#define LIB_TOKEN_H

#include <stddef.h>

enum bus_token_type {

	// end of file
	BUS_TOKEN_EOF = -1,

	BUS_TOKEN_ROUTE,

	BUS_TOKEN_102,

	BUS_TOKEN_MARKHAM,

	BUS_TOKEN_ROAD,

	BUS_TOKEN_SOUTHBOUND,

	BUS_TOKEN_TOWARDS,

	BUS_TOKEN_WARDEN,

	BUS_TOKEN_STATION,

	BUS_TOKEN_INVALID,
};

const char *bus_token_type_name(enum bus_token_type type);

struct bus_token {
	enum bus_token_type type;
	size_t line;
	size_t column;
};

#endif // LIB_TOKEN_H
