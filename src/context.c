#include "../lib/context.h"

#include <string.h>

// TODO: include param name in func. declaration for void(*)
void bus_context_init(struct bus_context *ctx, const unsigned int *program,
		      FILE *in, FILE *out, void (*)(struct bus_context *ctx)) // what the fuck???
{
	ctx->ip = program;
	ctx->dp = ctx->memory;
	ctx->in = in;
	ctx->out = out;
	ctx->program = program;
	memset(ctx->memory, 0, CONTEXT_MEMORY);
}
