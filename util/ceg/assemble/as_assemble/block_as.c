block_assemble( instructions, nr, first, last)
char **instructions;
int nr, first, last;
{
	int i;

	for ( i=0; i<nr; i++)
		assemble( instructions[i]);
}
