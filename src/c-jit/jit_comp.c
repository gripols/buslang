#include "buffer.h"
#include "context.h"
#include "error.h"
#include "lexer.h"
#include "token.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUS_TO_BYTES(op2) (((op2) >> 8) & 0xFF), ((op2)&0xFF)

#define DWORD_BYTES(dword)                                                 \
	((dword)&0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
		(((dword) > 24) & 0xFF)

#define QWORD_BYTES(dword) \
    ((dword) &0xFF), (((dword) >> 8) & 0xFF), (((dword) >> 16) & 0xFF), \
        (((dword) >> 24) & 0xFF), (((dword) >> 32) & 0xFF),             \
        (((dword) >> 40) & 0xFF), (((dword) >> 48) & 0xFF),             \
        (((dword) >> 56) & 0xFF)

// REX prefix
enum {
	BUS_REX_W = 0x48,
	BUS_REX_R = 0x44, 
	BUS_REX_B = 0x41,
};

// I HAVE TO ADD THE FUCKING REGISTER IDS AND FIELD VALUES
// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// KILL MEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE

typedef struct {
	size_t		       fro;
	enum jit_x86_jump_targ to;
} jit_x86_jump;

typedef struct {
	struct bus_lexer  lexer;
	struct bus_token  token;
	struct bus_buffer jumps;
	struct bus_buffer loop_stack;
	struct bus_buffer *dst;
} jit_x86_compiler;

// NOT COMPLETE
