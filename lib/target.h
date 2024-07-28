#ifndef TARGET_H
#define TARGET_H

#include "buffer.h"
#include "context.h"
#include "error.h"
#include "token.h"

#include <stdbool.h>
#include <stdio.h>

enum bus_target { BUS_TARG_JIT_X86, BUS_INTERP };

const char *bus_targ_name(enum bus_target targ);

enum bus_error bus_comp(enum bus_target targ, FILE *src, struct bus_buffer *dst,
			struct bus_buffer *last_token_dst);

enum bus_error bus_run(enum bus_target targ, struct bus_context *ctx);

#endif // TARGET_H
