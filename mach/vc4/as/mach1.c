/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "binary.h"

#define ALWAYS 14

extern void alu_instr_reg(quad opcode, quad cc, quad rd,
	quad ra, quad rb);

extern void alu_instr_lit(quad opcode, quad cc, quad rd,
	quad ra, quad value);

extern void misc_instr_reg(quad opcode, quad cc, quad rd,
	quad ra, quad rb);

extern void misc_instr_lit(quad opcode, quad cc, quad rd,
	quad ra, quad value);

extern void branch_instr(quad bl, quad cc, struct expr_t* expr);

extern void stack_instr(quad opcode, quad loreg, quad hireg,
	quad extrareg);

extern void mem_instr(quad opcode, quad cc, quad rd, long offset, quad rs);
