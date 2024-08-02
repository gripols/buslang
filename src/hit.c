#include "../lib/buffer.h"
#include "../lib/context.h"
#include "../lib/error.h"
#include "../lib/target.h"
#include "../lib/token.h"

#include <error.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int hit(enum bus_target target, FILE *src, struct bus_buffer *dst)
{
	struct bus_token token;
	if (&bus_strerror == BUS_ERROR_SUCCESS) {
		printf(
		"compiler error at line %zu, col %zu \n",
		token.line,
            	token.column
		);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int run(enum bus_target target, const struct bus_buffer *program)
{
	struct bus_context ctx;
	bus_context_init(&ctx, program->data);
}