#include "../lib/buffer.h" // see if ts will work if i put 0 for size_t n
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

// Instruction primary opcodes.
enum {
	ADD_RM8_IMM8 = 0x80,
	ADD_RM64_IMM32 = 0X81,
	CALL_REL32 = 0xE8,
	CALL_RM64 = 0xFF,
	CMP_EAX_IMM32 = 0x3D,
	CMP_RAX_IMM32 = 0x80,
	JMP_REL8 = 0xEB,
	JMP_RM64 = 0xFF,
	LEA_R64_M = 0x8D,
	MOV_RM8_R8 = 0x88,
	MOV_RM64_R64 = 0x89,
	MOV_R64_RM64 = 0x8B,
	MOV_R32_IMM32 = 0xB8,
	MOV_R64_IMM64 = 0xB8,
	POP_R64 = 0x58,
	POP_RM64 = 0x8F,
	PUSH_R64 = 0x50,
	RET_NEAR = 0xC3,
	SUB_RM8_IMM8 = 0x80,
	SUB_RM64_IMM32 = 0x81,
	SUB_RM64_R64 = 0x29,
	XOR_RM32_R32 = 0x31,

	JAE_REL32 = 0x0F83,
	JB_REL32 = 0x0F82,
	JE_REL32 = 0xF84,
	JNE_REL8 = 0x075,
	JNE_REL32 = 0x0F85,
};

// Register IDs
enum {
	REG_AL = 0x0,
	REG_EAX = 0x0,
	REG_EDI = 0x7,
	REG_RAX = 0x0,
	REG_RBX = 0x3,
	REG_RSI = 0x6,
	REG_RDI = 0x7,
	REG_R12 = 0x4,
	REG_R13 = 0x5,
	REG_R14 = 0x6,
	REG_R15 = 0x7,
};

enum {
	MODRM_DISP0 = 0x0 << 6,
	MODRM_DISP8 = 0x1 << 6,
	MODRM_DIRCT = 0x3 << 6,
};

enum {
	ADD_RM_IMM = 0x0 << 3,
	CALL_RM = 0x2 << 3,
	CMP_RM_IMM = 0x7 << 3,
	SUB_RM_IMM = 0x5 << 3,
	JMP_RM = 0x4 << 3,
	POP_RM = 0x0 << 3,

	MODRM_REG_AL = REG_AL << 3,

	MODRM_REG_EAX = REG_EAX << 3,
	MODRM_REG_EDI = REG_EDI << 3,

	MODRM_REG_RBX = REG_RBX << 3,
	MODRM_REG_RSI = REG_RSI << 3,
	MODRM_REG_RDI = REG_RDI << 3,
	MODRM_REG_R14 = REG_R14 << 3,
	MODRM_REG_R15 = REG_R15 << 3,
};

// ModR/M byte `rm` field values
enum {
	MODRM_RM_EAX = REG_EAX,
	MODRM_RM_RAX = REG_RAX,
	MODRM_RM_RBX = REG_RBX,
	MODRM_RM_RDI = REG_RDI,
	MODRM_RM_R12 = REG_R12,
	MODRM_RM_R13 = REG_R13,
	MODRM_RM_R14 = REG_R14,
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

static enum bus_error jit_x86_64_em_head(struct bus_buffer *dst)
{
	uint8_t instrs[] = {
		PUSH_R64 + REG_RBX, BUS_REX_B, PUSH_R64 + REG_R12, BUS_REX_B,
		PUSH_R64 + REG_R13, BUS_REX_B, PUSH_R64 + REG_R14, BUS_REX_B,
		PUSH_R64 + REG_R15, BUS_REX_W, MOV_RM64_R64,
		MODRM_DIRCT | MODRM_REG_RDI | MODRM_RM_RBX,

		// mov r12 fgetc
		BUS_REX_W | BUS_REX_B, MOV_R64_IMM64 + REG_R12,
		QWORD_BYTES((int64_t)fgetc),

		// mov r13 fputc
		BUS_REX_W | BUS_REX_B, MOV_R64_IMM64 + REG_R13,
		QWORD_BYTES((int64_t)fputc),

		// mov r14, qword ptr [rdi + offsetof (struct bus_context, dp)]
		BUS_REX_W | BUS_REX_R, MOV_R64_RM64,
		MODRM_DISP8 | MODRM_REG_R14 | MODRM_RM_RDI,
		offsetof(struct bus_context, dp),

		// mov r15, qword ptr [rdi + offsetof
		// (struct bus_context, memory)]
		BUS_REX_W | BUS_REX_R, LEA_R64_M,
		MODRM_DISP8 | MODRM_REG_R15 | MODRM_RM_RDI,
		offsetof(struct bus_context, memory)
	};
	return BUFFER_WRITE(dst, instrs);
}

static enum bus_error jit_x86_64_set_rel8(struct bus_buffer *dst, size_t fro,
					  size_t to)
{
	ssize_t rel8 = (ssize_t)(to - fro);

	if (rel8 < INT8_MIN || rel8 > INT8_MAX)
		return BUS_ERROR_JIT_JUMP;

	*(int8_t *) &dst->data[fro - sizeof(int8_t)] = (int8_t)rel8;
	return BUS_ERROR_SUCCESS;
}

static enum bus_error jit_x86_64_set_rel32(struct bus_buffer *dst, size_t fro,
					   size_t to)
{
	ssize_t rel32 = (ssize_t)(to - fro);

	if (rel32 < INT32_MIN || rel32 > INT32_MAX)
		return BUS_ERROR_JIT_JUMP;

	unsigned int *rel32_dst = &dst->data[fro - sizeof(int32_t)];
	rel32_dst[0] = rel32 & 0xFF;
	rel32_dst[1] = (rel32 >> 8) & 0xFF;
	rel32_dst[2] = (rel32 >> 16) & 0xFF;
	rel32_dst[3] = (rel32 >> 24) & 0xFF;
	return BUS_ERROR_SUCCESS;
}

static enum bus_error jit_x86_64_em_ret_err_segf(size_t exit,
						 struct bus_buffer *dst)
{
	uint8_t instrs[] = { // mov eax segfault
				  MOV_R32_IMM32 + REG_EAX,
				  DWORD_BYTES(BUS_ERROR_SEGFAULT), JMP_REL8, 0
	};

	enum bus_error error = BUFFER_WRITE(dst, instrs);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	return jit_x86_64_set_rel8(dst, dst->size, exit);
}

static enum bus_error jit_ret_err_io(size_t exit, struct bus_buffer *dst)
{
	uint8_t instrs[] = { MOV_R32_IMM32 + REG_EAX,
				  DWORD_BYTES(BUS_ERROR_IO), JMP_REL8, 0 };

	return jit_x86_64_set_rel8(dst, dst->size, exit);
}

static enum bus_error jit_x86_64_ham_fgetc_err(size_t exit,
					       struct bus_buffer *dst)
{
	uint8_t ret_err_ferror[] = {
		// mov rdi qword ptr
		BUS_REX_W,
		MOV_R64_RM64,
		MODRM_DISP8 | MODRM_REG_RDI | MODRM_RM_RBX,
		offsetof(struct bus_context, in),

		// mov rax ferr
		BUS_REX_W,
		MOV_R64_IMM64 + REG_EAX,
		QWORD_BYTES((int64_t)ferror),

		// call RAX
		CALL_RM64,
		MODRM_DIRCT | CALL_RM | MODRM_RM_RAX,

		// cmp eax 0
		CMP_EAX_IMM32,
		DWORD_BYTES(0),

		// mov eax ERR IO
		MOV_R32_IMM32 + REG_EAX,
		DWORD_BYTES(BUS_ERROR_IO),

		// offset later
		BUS_TO_BYTES(JNE_REL8),
		0,
	};

	enum bus_error error = BUFFER_WRITE(dst, ret_err_ferror);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	error = jit_x86_64_set_rel8(dst, dst->size, exit);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	uint8_t ret_err_runtime_end_io_file[] = {
		// mov eax BUS_ERROR_RUNTIME_END
		MOV_R32_IMM32 + REG_EAX,
		DWORD_BYTES(BUS_ERROR_RUNTIME_END),
		// jmp exit
		JMP_REL8,
		0,
	};

	error = BUFFER_WRITE(dst, ret_err_runtime_end_io_file);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	return jit_x86_64_set_rel8(dst, dst->size, exit);
}

static enum bus_error jit_x86_emit_jmp_targ(enum jit_x86_64_jump_target target,
					    size_t exit, struct bus_buffer *dst)
{
	switch (target) {
	case BUS_JUMP_ERROR_SEGFAULT:
		return jit_x86_64_em_ret_err_segf(exit, dst);
		
	case BUS_JUMP_ERROR_IO:
		return jit_ret_err_io(exit, dst);
	case BUS_JUMP_FGETC_EOFM: // shut the fuck up
		return jit_x86_64_ham_fgetc_err(exit, dst);

	default:
		return BUS_ERROR_COMP_INTERNAL;
	}
}

static enum bus_error jit_x86_emit_foot(struct bus_buffer *jumps,
					struct bus_buffer *dst)
{
	uint8_t set_err_succ[] = {
		// xor eax, eax
		XOR_RM32_R32,
		MODRM_DIRCT | MODRM_REG_EAX | MODRM_REG_EAX,
	};

	enum bus_error error = BUFFER_WRITE(dst, set_err_succ);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	size_t exit = dst->size;
	uint8_t kill_me[] = {
		// pop r15
		BUS_REX_B,
		POP_R64 + REG_R15,

		// pop r14
		BUS_REX_B,
		POP_R64 + REG_R14,

		// pop r13
		BUS_REX_B,
		POP_R64 + REG_R13,

		// pop r12
		BUS_REX_B,
		POP_R64 + REG_R12,

		// pop rbx
		POP_R64 + REG_RBX,

		// ret
		RET_NEAR,
	};

	error = BUFFER_WRITE(dst, kill_me);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	size_t jmp_targ_offset[BUS_NUM_JUMP_TARGS] = { 0 };
	size_t jmp_struct_size = sizeof(struct jit_x86_64_jump);

	for (size_t i = 0; i < jumps->size; i += jmp_struct_size) {
		struct jit_x86_64_jump *jump =
			(struct jit_x86_64_jump *)&jumps->data[i];

		if (jmp_targ_offset[jump->to] == 0) {
			jmp_targ_offset[jump->to] = dst->size;
			jit_x86_emit_jmp_targ(jump->to, exit, dst);
		}

		error = jit_x86_64_set_rel32(dst, jump->fro,
					     jmp_targ_offset[jump->to]);

		if (error != BUS_ERROR_SUCCESS)
			return error;
	}

	return BUS_ERROR_SUCCESS;
}

static enum bus_error jit_x86_em_addp(struct jit_x86_64_comp *comp)
{
	struct bus_lexer *lexer = &comp->lexer;
	enum bus_token_type bus_token_type = comp->token.type;

	uint16_t operand = 1;
	struct bus_token next_token;

	while ((next_token = bus_lexer_next_token(lexer)).type ==
	       bus_token_type) {
		if (operand == UINT16_MAX) {
			comp->token = next_token;
			return BUS_ERROR_SEGFAULT;
		}
		operand++;
	}

	uint16_t jccop;
	uint8_t op;
	uint8_t modrm_reg;
	int32_t cmp_bound;

	switch (bus_token_type) {
	case BUS_TOKEN_ROUTE:
		jccop = JAE_REL32;
		op = ADD_RM64_IMM32;
		modrm_reg = ADD_RM_IMM;
		cmp_bound = CONTEXT_MEMORY - operand;
		break;
	case BUS_TOKEN_102:
		jccop = JB_REL32;
		op = SUB_RM64_IMM32;
		modrm_reg = SUB_RM_IMM;
		cmp_bound = CONTEXT_MEMORY - operand;
		break;
	default:
		return BUS_ERROR_COMP_INTERNAL;
	}

	comp->token = next_token;

	uint8_t bound_check[] = {
		// mov rax r14
		BUS_REX_W | BUS_REX_R,
		MOV_RM64_R64,
		MODRM_DIRCT | MODRM_REG_R14 | MODRM_RM_RAX,
		// sub rax r15
		BUS_REX_W | BUS_REX_R,
		SUB_RM64_R64,
		MODRM_DIRCT | MODRM_REG_R15 | MODRM_RM_RAX,
		// cmp rax cmp bound
		BUS_REX_W,
		CMP_RAX_IMM32,
		DWORD_BYTES(cmp_bound),
		// ret segf offset ltr
		BUS_TO_BYTES(jccop),
		DWORD_BYTES(0),
	};

	enum bus_error error = BUFFER_WRITE(comp->dst, bound_check);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	uint8_t instr[] = {
		BUS_REX_W | BUS_REX_B,
		op,
		MODRM_DIRCT | modrm_reg | MODRM_RM_R14,
		DWORD_BYTES(operand),
	};

	return BUFFER_WRITE(comp->dst, instr);
}

static enum bus_error jit_x86_em_addv(struct jit_x86_64_comp *comp)
{
	enum bus_token_type bus_token_type = comp->token.type;
	struct bus_lexer *lexer = &comp->lexer;

	uint8_t op;
	uint8_t modrm_reg;

	switch (bus_token_type) {
	case BUS_TOKEN_MARKHAM:
		op = ADD_RM8_IMM8;
		modrm_reg = ADD_RM_IMM;
		break;
	case BUS_TOKEN_ROAD:
		op = SUB_RM8_IMM8;
		modrm_reg = SUB_RM_IMM;
		break;
	default:
		return BUS_ERROR_COMP_INTERNAL;
	}

	uint8_t operand = 1;
	struct bus_token next_token;

	while ((next_token = bus_lexer_next_token(lexer)).type ==
	       bus_token_type) {
		operand++;
	}

	comp->token = next_token;

	if (operand == 0)
		return BUS_ERROR_SUCCESS;

	uint8_t instr[] = {
		BUS_REX_B,
		op,
		MODRM_DISP0 | modrm_reg | MODRM_RM_R14,
		operand,
	};
	return BUFFER_WRITE(comp->dst, instr);
}

static enum bus_error jit_x86_em_write(struct jit_x86_64_comp *comp)
{
	uint8_t instrs[] = {
		BUS_REX_B,
		BUS_TO_BYTES(0x0FB6), // what the heck is this var name
		MODRM_DISP0 | MODRM_REG_EDI | MODRM_RM_R14,
		// mov rsi qword ptr
		BUS_REX_W,
		MOV_R64_RM64,
		MODRM_DISP8 | MODRM_REG_RSI | MODRM_RM_RBX,
		offsetof(struct bus_context, out),
		// call r13
		BUS_REX_B,
		CALL_RM64,
		MODRM_DIRCT | CALL_RM | MODRM_RM_R13,
		// cmp eax -1
		CMP_EAX_IMM32,
		DWORD_BYTES((int32_t)-1),
		BUS_TO_BYTES(JE_REL32),
		DWORD_BYTES(0),
	};

	enum bus_error error = BUFFER_WRITE(comp->dst, instrs);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	struct jit_x86_64_jump jump = {
		.fro = comp->dst->size,
		.to = BUS_JUMP_ERROR_IO,
	};

	return bus_buffer_write(&comp->jumps, &jump, sizeof(jump));
}

static enum bus_error jit_x86_em_read(struct jit_x86_64_comp *comp)
{
	uint8_t call_fgetc[] = {
		// mov rdi QWORD ptr
		BUS_REX_W,
		MOV_R64_RM64,
		MODRM_DISP8 | MODRM_REG_RDI | MODRM_RM_RBX,
		offsetof(struct bus_context, in),
		//call r12
		BUS_REX_B,
		CALL_RM64,
		MODRM_DIRCT | CALL_RM | MODRM_RM_R12,
		// cmp eax -1
		CMP_EAX_IMM32,
		DWORD_BYTES((int32_t)-1),
		BUS_TO_BYTES(JE_REL32),
		DWORD_BYTES(0),
	};

	enum bus_error error = BUFFER_WRITE(comp->dst, call_fgetc);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	struct jit_x86_64_jump jmp = {
		.fro = comp->dst->size,
		.to = BUS_JUMP_FGETC_EOFM,
	};

	error = bus_buffer_write(&comp->jumps, &jmp, sizeof(jmp));

	if (error != BUS_ERROR_SUCCESS)
		return error;

	uint8_t w_ret_char_mem[] = {
		BUS_REX_B,
		MOV_RM8_R8,
		MODRM_DISP0 | REG_AL | MODRM_RM_R14,
	};
	return BUFFER_WRITE(comp->dst, w_ret_char_mem);
}

static enum bus_error beg_loop(struct jit_x86_64_comp *comp)
{
	uint8_t instrs[] = {
		BUS_REX_B,
		ADD_RM8_IMM8,
		MODRM_DISP0 | CMP_RM_IMM | MODRM_RM_R14,
		0,
		BUS_TO_BYTES(JNE_REL32),
		DWORD_BYTES(0),
	};

	enum bus_error error = BUFFER_WRITE(comp->dst, instrs);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	return bus_buffer_write_size(&comp->loop_stack, comp->dst->size);
}

static enum bus_error end_loop(struct jit_x86_64_comp *comp)
{
	if (comp->loop_stack.size == 0)
		return BUS_ERROR_COMP_UNCLOSED_LOOPS;

	uint8_t instrs[] = {
		// cmp byte ptr [r14] 0
		BUS_REX_B,
		CMP_RM_IMM,
		MODRM_DISP0 | CMP_RM_IMM | MODRM_RM_R14,
		0,
		// jne loop_start
		BUS_TO_BYTES(JNE_REL32),
		DWORD_BYTES(0),
	};

	enum bus_error error = BUFFER_WRITE(comp->dst, instrs);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	size_t loop_start = bus_buffer_pop_size(&comp->loop_stack);
	
	error = jit_x86_64_set_rel32(comp->dst, comp->dst->size, loop_start);

	if (error != BUS_ERROR_SUCCESS)
		return error;

	return jit_x86_64_set_rel32(comp->dst, loop_start, comp->dst->size);
}

static enum bus_error jit_x86_emit(struct jit_x86_64_comp *comp)
{
	enum bus_error error;

	switch (comp->token.type) {
	case BUS_TOKEN_ROUTE:
	case BUS_TOKEN_102:
		return jit_x86_em_addp(comp);
	case BUS_TOKEN_MARKHAM:
	case BUS_TOKEN_ROAD:
		return jit_x86_em_addv(comp);
	case BUS_TOKEN_SOUTHBOUND:
		error = jit_x86_em_write(comp);
		break;
	case BUS_TOKEN_TOWARDS:
		error = jit_x86_em_read(comp);
		break;
	case BUS_TOKEN_WARDEN:
		error = beg_loop(comp);
		break;
	case BUS_TOKEN_STATION:
		error = end_loop(comp);
		break;
	default:
		return BUS_ERROR_COMP_INV_TOKEN;
	}
	if (error == BUS_ERROR_SUCCESS)
		comp->token = bus_lexer_next_token(&comp->lexer);

	return error;
}

enum bus_error bus_compile(FILE *src, struct bus_buffer *dst,
			   struct bus_token *last_token_dst)
{
	struct jit_x86_64_comp comp;
	enum bus_error error = jit_x86_64_comp_init(&comp, src, dst);

	if (error != BUS_ERROR_SUCCESS) {
		*last_token_dst = comp.token;
		return error;
	}

	error = jit_x86_64_em_head(dst);

	if (error != BUS_ERROR_SUCCESS) {
		*last_token_dst = comp.token;
		jit_x86_64_comp_fini(&comp);
		return error;
	}

	while (comp.token.type != BUS_TOKEN_EOF) {
		error = jit_x86_emit(&comp);

		if (error != BUS_ERROR_SUCCESS) {
			*last_token_dst = comp.token;
			jit_x86_64_comp_fini(&comp);
			return error;
		}
	}

	*last_token_dst = comp.token;

	if (comp.loop_stack.size != 0) {
		jit_x86_64_comp_fini(&comp);
		return BUS_ERROR_COMP_UNCLOSED_LOOPS;
	}

	error = jit_x86_emit_foot(&comp.jumps, dst);
	jit_x86_64_comp_fini(&comp);
	return error;
}
