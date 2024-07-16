#include "token.h"

const char *bus_token_type_name(enum bus_token_type type)
{
	switch (type) {
	case BUS_TOKEN_EOF:
		return "EOF";
	case BUS_TOKEN_ROUTE:
		return "ROUTE";
	case BUS_TOKEN_102:
		return "102";
	case BUS_TOKEN_MARKHAM:
		return "MARKHAM";
	case BUS_TOKEN_ROAD:
		return "ROAD";
	case BUS_TOKEN_SOUTHBOUND:
		return "SOUTHBOUND";
	case BUS_TOKEN_TOWARDS:
		return "TOWARDS";
	case BUS_TOKEN_WARDEN:
		return "WARDEN";
	case BUS_TOKEN_STATION:
		return "STATION";
	default:
		return "???";
	}
}
