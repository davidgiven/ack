block_assemble( instructions, nr, first, last)

/* Assemble a block of assembly instructions. If the table writer wants to
 * combine assembly instructions he must rewrite this routine to fulfill his
 * own needs.
 */

char **instructions;
int nr, first, last;
{
	int i;

	for ( i=0; i<nr; i++)
		assemble( instructions[i]);
}
