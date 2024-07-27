#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>
#include <stdio.h>

#define CONTEXT_MEMORY (1U << 16U)

struct bus_context {
	const uint8_t *ip;
	uint8_t *dp;
	FILE *in;
	FILE *out;
	const uint8_t *program;
	uint8_t memory[CONTEXT_MEMORY];
};

void bus_context_init(struct bus_context *ctx, const uint8_t *program,
		      FILE *in, FILE *out, struct bus_context *);

#endif // CONTEXT_H
