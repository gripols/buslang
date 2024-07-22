#include "../lib/context.h"
#include "../lib/error.h"
#include "../lib/jit.h"

enum bus_error bus_run_jit_x86(struct bus_context *ctx)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	return ((enum bus_error(*)(struct bus_context *))ctx->ip)(ctx);
#pragma GCC diagnostic pop
}
