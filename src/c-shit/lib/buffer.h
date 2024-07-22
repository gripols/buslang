#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "error.h"

struct bus_buffer {
	size_t size;
	size_t cap;
	unsigned int *data;
};

// Inits given buffer to store executable code.
// Returns err. occuring in process
enum bus_error bus_error_init(struct bus_buffer *buffer);

// Write `n` bytes from data at end of buffer. Returns err. occuring
// in process. Buffer must init. w/ init func.
enum bus_error bus_buffer_write(struct bus_buffer *buffer, const void *data,
				size_t n);

// Writes the given `uint8_t` val at end of given buffer.
static inline enum bus_error bus_buffer_write_u8(struct bus_buffer *buffer,
						 uint8_t value)
{
	return bus_buffer_write(buffer, &value, sizeof(value));
}

// Writes the given `size_t` val at end of given buffer.
static inline enum bus_error bus_buffer_write_size(struct bus_buffer
						   *buffer, size_t value)
{
	return bus_buffer_write(buffer, &value, sizeof(value));
}

#define BUFFER_WRITE(buffer, data) \
	bus_buffer_write((buffer), (data), sizeof(data))

// returns + removes 'size_t' val at end of given buffer.
size_t bus_buffer_pop_size(struct bus_buffer *buffer);

// finals given buffer init'd with `buffer_init_jit`
// and frees alloc'd storage cap
void bus_buffer_finish(struct bus_buffer *buffer);

#endif // BUFFER_H
