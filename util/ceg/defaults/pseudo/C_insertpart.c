#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_insertpart( d)
int d;

{
	char *s;
	swtxt();

	C_jump(extnd_part( d));
	symbol_definition( s = extnd_cont( d));
	set_local_visible( s);
}


void
C_beginpart( d)
int d;

{
	char *s;
	swtxt();

	C_jump(extnd_main( d));
	symbol_definition(s = extnd_part( d));
	set_local_visible(s);
}


void
C_endpart( d)
int d;

{
	char *s;
	swtxt();

	C_jump(extnd_cont( d));
	symbol_definition(s = extnd_main( d));
	set_local_visible(s);
}


int
C_getid()
{
	static int id = 0;

	return ++id;
}
