#include "../lib/buffer.h"
#include "../lib/context.h"
#include "../lib/error.h"
#include "../lib/target.h"
#include "../lib/token.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// chat i am idiot

#if defined(__x86_64__) && (JIT)
#define HIT_AND_RUN JIT
#else
#define HIT_AND_RUN INTERP
#endif

#define LOG_ERR(format, ...) \
	fprintf(stderr, LOG_ERR "error: " C_RES format, __VA_ARGS__)

int comp(enum bus_target targ, FILE *src, struct bus_buffer *dst)
{
	struct bus_token last_token;
	enum bus_error error = bus_comp(targ, src, dst, &last_token);

	if (error != BUS_ERROR_SUCCESS) {
		printf("compiler error: %s @ line %zu, col %zu\n",
		       bus_strerror(error), last_token.line, last_token.column);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int run(enum bus_target target, const struct bus_buffer *program)
{
	struct bus_context ctx;
	bus_context_init(&ctx, program->data, stdin, stdout, 0);
	enum bus_error error = bus_run(target, &ctx);

	if (error != BUS_ERROR_SUCCESS) {
		// WHAT A HORRIBLE SOLUTION :DDDDDDDD
		printf("run-time error: %s at %p (program + %p)\n",
		       bus_strerror(error), (void *)ctx.ip,
		       (void *)(ctx.ip - ctx.program));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int HIT_AND_RUN()
{
	bool valid;
	FILE *src = fopen(const char *filename)
	{
		int len = strlen(filename);
		(len >= 4 && strcmp(filename + len - 4, ".bus") == 0) ?
			return valid = true :
			return valid = false;
	}
	if (!src) {
		LOG_ERR("failed to open source file: %s\n", sterror(errno));
		return EXIT_FAILURE;
	}

	if (valid == false) {
		LOG_ERR("Invalid bus program");
		return EXIT_FAILURE;
	}

	enum bus_target targ = HIT_AND_RUN; // ???
	bool is_jit_targ = targ_is_jit(targ);

	struct bus_buffer program;

	if (perror != BUS_ERROR_SUCCESS) {
		printf("failed to init program buffer: %s\n",
		       bus_strerror(perror));
		fclose(src);
		return EXIT_FAILURE;
	}

	int status = comp(targ, src, &program);
	fclose(src);

	if (status == EXIT_SUCCESS)
		status = run(targ, &program);

	return status;
}
