#include "stdio.h"
#include "f2c.h"

extern VOID sig_die();

/* called when a subscript is out of range */

VOID s_rnge(varn, offset, procn, line)
char *varn, *procn;
long int offset, line;
{
register int i;

fprintf(stderr, "Subscript out of range on file line %ld, procedure ", line);
while((i = *procn) && i != '_' && i != ' ')
	putc(*procn++, stderr);
fprintf(stderr, ".\nAttempt to access the %ld-th element of variable ", offset+1);
while((i = *varn) && i != ' ')
	putc(*varn++, stderr);
sig_die(".", 1);
}
