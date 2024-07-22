#include "../lib/buffer.h"
#include "../lib/context.h"
#include "../lib/error.h"
#include "../lib/lexer.h"
#include "../lib/token.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUS_TO_BYTES(op2) (((op2) >> 8) & 0xFF), ((op2) & 0xFF)

#define DWORD_BYTES(dword)                                                   \
	((dword) & 0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
		(((dword) > 24) & 0xFF)

#define QWORD_BYTES(dword)                                                   \
	((dword) & 0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
		(((dword) >> 24) & 0xFF), (((dword) >> 32) & 0xFF),          \
		(((dword) >> 40) & 0xFF), (((dword) >> 48) & 0xFF),          \
		(((dword) >> 56) & 0xFF)

// REX prefixes
enum {
	BUS_REX_W = 0x48,
	BUS_REX_R = 0x44,
	BUS_REX_B = 0x41,
};

enum jit_x86_64_jump_target {
	BUS_JUMP_ERROR_SEGFAULT,
	BUS_JUMP_ERROR_IO,
	BUS_JUMP_FGETC_EOFM,
	BUS_NUM_JUMP_TARGS,
};

struct jit_x86_64_jump {
	size_t fro;
	enum jit_x86_64_jump_target to;
};

struct jit_x86_64_comp {
	struct bus_lexer lexer;
	struct bus_token token;
	struct bus_buffer jumps;
	struct bus_buffer loop_stack;
	struct bus_buffer *dst;
};

/*
static enum bus_error jit_x86_64_comp_init(struct jit_x86_64_comp *comp,
					       FILE *src, struct bus_buffer *dst)
{
	comp->dst = dst;
	bus_lexer_init(&comp->lexer, src);
	comp->token = bus_lexer_next_token(&comp->lexer);
	enum bus_error error = bus_buffer_init(&comp->jumps);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	return bus_buffer_init(&comp->loop_stack);
}

static void jit_x86_64_comp_fini(struct jit_x86_64_comp *comp)
{
	bus_buffer_finish(&comp->jumps);
	bus_buffer_finish(&comp->loop_stack);
}
*/