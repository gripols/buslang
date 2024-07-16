#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "error.h"

typedef struct {
	size_t size;
	size_t cap;
	unsigned int *data;
} bus_buffer;

enum bus_error bus_buffer_init(bus_buffer *buffer);

enum bus_error bus_error_init_jit(bus_buffer *buffer);

#endif // BUFFER_H
