#include "../lib/buffer.h"
#include "../lib/error.h"
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_DEF_CAP 64
#define BUFFER_GROW_FAC 2

enum bus_error bus_buffer_init(struct bus_buffer *buffer)
{
	buffer->size = 0;
	buffer->cap = sysconf(_SC_PAGESIZE);
	buffer->data = mmap(NULL, buffer->cap,
			    PROT_READ | PROT_WRITE | PROT_EXEC,
			    MAP_PRIVATE | MAP_ANON, -1, 0);

	if (!buffer->data)
		return BUS_ERROR_MALLOC;

	return BUS_ERROR_SUCCESS;
}

enum bus_error bus_buffer_write(struct bus_buffer *buffer, const void *data,
				size_t n)
{
	if (buffer->size + n > buffer->cap) {
		size_t prev_cap = buffer->cap;
		buffer->cap *=
			(buffer->cap + n - 1) / buffer->cap * BUFFER_GROW_FAC;
		uint8_t *new_data = mmap(NULL, buffer->cap,
					 PROT_READ | PROT_WRITE | PROT_EXEC,
					 MAP_PRIVATE | MAP_ANON, -1, 0);

		if (new_data == MAP_FAILED)
			return BUS_ERROR_MALLOC;

		memcpy(new_data, buffer->data, buffer->size);
		munmap(buffer->data, prev_cap);
		buffer->data = new_data;
	}

	memcpy(&buffer->data[buffer->size], data, n);
	buffer->size += n;
	return BUS_ERROR_SUCCESS;
}

size_t bus_buffer_pop_size(struct bus_buffer *buffer)
{
	size_t top = ((size_t *)&buffer->data[buffer->size])[-1];
	buffer->size -= sizeof(top);
	return top;
}

void bus_buffer_finish(struct bus_buffer *buffer)
{
	munmap(buffer->data, buffer->cap);
}
