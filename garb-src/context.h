#ifndef LIB_CONTEXT_H
#define LIB_CONTEXT_H

#include <stdint.h>
#include <stdio.h>

#define CONTEXT_MEMORY (1U << 16U)

struct bus_context {
	const unsigned int *ip;
	uint8_t *dp;
	FILE *in;
	FILE *out;
	const unsigned int *program;
	unsigned int memory[CONTEXT_MEMORY];
};

void bus_context_init(struct bus_context *ctx, const unsigned int *program,
		      FILE *in, FILE *out, void (*)(struct bus_context *));

#endif // LIB_CONTEXT_H