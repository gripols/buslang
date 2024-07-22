#include "../lib/error.h"

const char *bus_strerror(enum bus_error error)
{
	switch (error) {
	case BUS_ERROR_SUCCESS:
		return "success";
	case BUS_ERROR_MALLOC:
		return "malloc error";
	case BUS_ERROR_IO:
		return "input/output error";
	case BUS_ERROR_COMP_INV_TARGET:
		return "invalid target";
	case BUS_ERROR_COMP_INV_TOKEN:
		return "invalid token";
	case BUS_ERROR_COMP_NO_LOOP_END:
		return "loop(s) has no end";
	case BUS_ERROR_COMP_UNCLOSED_LOOPS:
		return "unclosed loop(s)";
	case BUS_ERROR_COMP_INTERNAL:
		return "we fucked up and have no idea why";
	case BUS_ERROR_SEGFAULT:
		return "SEGFAULT: pointer out of bounds";
	case BUS_ERROR_RUNTIME_END:
		return "End of file";
	case BUS_ERROR_JIT_JUMP:
		return "jump too large";
	default:
		return "huh???";
	}
}
