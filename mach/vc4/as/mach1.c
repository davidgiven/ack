/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "binary.h"

#define ALWAYS 14

extern void alu_instr_reg(unsigned opcode, unsigned cc, unsigned rd,
	unsigned ra, unsigned rb);

extern void alu_instr_lit(unsigned opcode, unsigned cc, unsigned rd,
	unsigned ra, unsigned value);

extern void misc_instr_reg(unsigned opcode, unsigned cc, unsigned rd,
	unsigned ra, unsigned rb);

extern void misc_instr_lit(unsigned opcode, unsigned cc, unsigned rd,
	unsigned ra, unsigned value);

extern void branch_instr(unsigned bl, unsigned cc, struct expr_t* expr);

extern void stack_instr(unsigned opcode, unsigned loreg, unsigned hireg,
	unsigned extrareg);