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

do_instr(opcode)
	unsigned int opcode;
{
	switch (opcode) {
#include	"switch/DoCases"	/* for the muscle */
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
