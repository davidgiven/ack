/*
 * VideoCore IV assembler for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include "binary.h"

#define ALWAYS 14

extern void alu_instr_reg(quad opcode, int cc, int rd, int ra, int rb);
extern void alu_instr_lit(quad opcode, int cc, int rd, int ra, long value);
extern void misc_instr_reg(quad opcode, int cc, int rd, int ra, int rb);
extern void misc_instr_lit(quad opcode, int cc, int rd, int ra, quad value);
extern void branch_instr(int bl, int cc, struct expr_t* expr);
extern void stack_instr(quad opcode, int loreg, int hireg, int extrareg);
extern void mem_instr(quad opcode, int cc, int rd, long offset, int rs);
extern void mem_offset_instr(quad opcode, int cc, int rd, int qa, int rb);
extern void mem_postincr_instr(quad opcode, int cc, int rd, int rs);
extern void mem_address_instr(quad opcode, int rd, struct expr_t* expr);
extern void branch_addcmp_reg_reg_instr(int cc, int rd, int ra, int rs, struct expr_t* expr);
extern void branch_addcmp_lit_reg_instr(int cc, int rd, long va, int rs, struct expr_t* expr);
extern void branch_addcmp_reg_lit_instr(int cc, int rd, int ra, long vs, struct expr_t* expr);
extern void branch_addcmp_lit_lit_instr(int cc, int rd, long va, long vs, struct expr_t* expr);
extern void lea_stack_instr(int rd, long va, int rs);
extern void lea_address_instr(int rd, struct expr_t* expr);
extern void fltcnv_instr(quad opcode, int cc, int rd, int ra, quad shift);
