#ifndef ERROR_H
#define ERROR_H

enum bus_error {

	BUS_ERROR_SUCCESS,

	BUS_ERROR_MALLOC,

	BUS_ERROR_IO,

	BUS_ERROR_COMP_INV_TARGET,

	BUS_ERROR_COMP_INV_TOKEN,

	BUS_ERROR_COMP_NO_LOOP_END,

	BUS_ERROR_COMP_UNCLOSED_LOOPS,
	
	// we fucked up
	BUS_ERROR_COMP_INTERNAL,

	// ptr out of bounds
	BUS_ERROR_SEGFAULT,

	BUS_ERROR_RUNTIME_END,

	BUS_ERROR_JIT_JUMP,
};

const char *bus_strerror(enum bus_error error);

#endif // ERROR_H
