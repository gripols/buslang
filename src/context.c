#include "../lib/context.h" // stfu

#include <stdint.h>
#include <string.h>

void bus_context_init(struct bus_context *ctx, const uint8_t *program,
		      FILE *in, FILE *out, struct bus_context *)
{
	ctx->ip = program;
	ctx->dp = ctx->memory;
	ctx->in = in;
	ctx->out = out;
	ctx->program = program;
	memset(ctx->memory, 0, sizeof(ctx->memory)); // fuck it hail mary
}
