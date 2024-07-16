// X86_64 JIT compiler

#ifndef JIT_H
#define JIT_H

#include "error.h"
#include "token.h"
#include <stdint.h>
#include <stdio.h>

enum bus_error bus_comp_jit(FILE *src, struct bus_buffer *dst,
			    struct bus_token *last_token_dst);

enum bus_error bus_run_jit_x86(struct bus_context *ctx);

#endif // JIT_H
