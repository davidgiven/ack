#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_insertpart( d)
int d;

{
	char *s;
	swtxt();

	C_jump(extnd_part( d));
	symbol_definition( s = extnd_cont( d));
	set_local_visible( s);
}


C_beginpart( d)
int d;

{
	char *s;
	swtxt();

	symbol_definition(s = extnd_part( d));
	set_local_visible(s);
}


C_endpart( d)
int d;

{
	swtxt();

	C_jump(extnd_cont( d));
}


C_getid()
{
	static int id = 0;

	return ++id;
}
