#include <stdlib.h>
#include "lib.h"

char* salloc(unsigned length)
{
	char* c, *s;
	c = malloc(length);
	if (!c)
		error(5);
	for (s = c; s < c + length; s++)
		*s = 0;
	return (c);
}

void sfree(char* c)
{
	if (!c)
		return;
	free(c);
}
