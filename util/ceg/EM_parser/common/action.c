#include "decl.h"
#include <alloc.h>

/* This file contains some routines needed in "pars.g" to handle the  action-
 * grammarrule. The assembler-instructions are handeld in blocks rather than
 * one at a time. So these routines provide saving and removing of assembler-
 * instructions.
 */

char **as_instructions;	    /* The buffer(?) where the instructions are saved */

int quantum = 0,	    /* Max. nr. of instructions in as_instructions[] */
    nr_instr,		    /* Number of saved instructions */
    first_action,	    /* Is this block of assembler-instr. the first after
			     * a '==>'?
			     */
    last_action;	    /* Is this block followed by a '.' ? */


init_as_block()
{
	nr_instr = 0;

	if ( quantum == 0) {
		quantum = 16;
		as_instructions = (char **)Malloc( quantum*sizeof( char *));
	}
}


save_as( instr)
char *instr;

/* Save a copy of 'instr'
 */
{
	if ( nr_instr == quantum) {
		quantum *= 2;
		as_instructions = (char **) Realloc( (char *) as_instructions,
					   quantum*sizeof( char *));
	}

	as_instructions[nr_instr++] = Salloc( instr, strlen( instr) + 1);
}

do_block_assemble()
{
	int i;

	if ( nr_instr > 0) {
		block_assemble( as_instructions, nr_instr,
				first_action, last_action);

		for ( i=0; i<nr_instr; i++)
			free( as_instructions[i]);
	}
}
