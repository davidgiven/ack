/*
	The big switch on all the opcodes
*/

/* $Id$ */

#include	<em_abs.h>
#include	"global.h"
#include	"opcode.h"
#include	"text.h"
#include	"trap.h"
#include	"warn.h"

void do_instr(unsigned int opcode)
{
	switch (opcode) {
#include	"DoCases"	/* for the muscle */
		case SECONDARY:
			do_instr(SEC_BASE + nextPCbyte());
			break;
		case TERTIARY:
			do_instr(TERT_BASE + nextPCbyte());
			break;
		default:
			wtrap(WBADOPC, EILLINS);
			break;
	}
}
