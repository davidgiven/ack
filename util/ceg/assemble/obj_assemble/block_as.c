block_assemble( instructions, nr, first, last)

/* Assembles a block of assembly instructions. If the table writer wants 
 * to combine a block of instructions (e.g., push/pop optimization) he
 * should changes this routine to his own needs.
 */

char **instructions;
int nr, first, last;
{
	int i;

	for ( i=0; i<nr; i++)
		assemble( instructions[i]);
}
