/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef _COST_
#include "cost.h"
#endif

#define AD_RO 01	/* Read only operand */
#define AD_WO 02	/* Write only operand */	
#define AD_RW 03	/* Read-write operand */
#define AD_RWMASK 03	/* Mask to select these possiblities */

#define AD_CC 04	/* Condition codes set to this one */

typedef struct operand {
	struct operand *o_next;
	short		o_setno;
	short		o_adorn;
} operand;

typedef struct instruction {
	char		*i_name;
	short	 	 i_asname;
	short	 	 i_nops;
	operand 	*i_oplist;
	struct varinfo	*i_erases;
	cost_t   	 i_cost;
} instr_t,*instr_p;

extern instr_t l_instr[];

/*
 * The read only information on the operands is not used at the moment.
 * Predicted future use:
 * When using :ro data it is possible to use a register in its stead
 * if it contains the same information and is allowed as an operand
 * in this place. Too difficult for now.
 */
