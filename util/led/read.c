#ifndef lint
static char rcsid[] = "$Header$";
#endif

int	infile;	/* The current input file. */

rd_fatal()
{
	fatal("read error");
}
