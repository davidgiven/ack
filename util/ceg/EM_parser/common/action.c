#include "decl.h"
#include <alloc.h>

int quantum = 0, nr_instr, block_saved, first_action, last_action;
extern char *malloc(), *realloc(), *Salloc();
char **as_instructions;

init_as_block()
{
	nr_instr = 0;
	block_saved = TRUE;

	if ( quantum == 0) {
		quantum = 16;
		as_instructions = (char **)malloc( quantum*sizeof( char *));
	}
}


save_as( instr)
char *instr;
{
	if ( nr_instr == quantum) {
		quantum *= 2;
		as_instructions = (char **) realloc( as_instructions,
					   quantum*sizeof( char *));
	}

	as_instructions[nr_instr++] = Salloc( instr, strlen( instr) + 1);
}

do_block_assemble()
{
	int i;

	if ( block_saved) {
		block_assemble( as_instructions, nr_instr,
				first_action, last_action);

		for ( i=0; i<nr_instr; i++)
			free( as_instructions[i]);
		
		block_saved = FALSE;
	}
}
