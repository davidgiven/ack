#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_insertpart( d)
int d;

{
	swtxt();

	jump(extnd_part( d));
	symbol_definition( extnd_cont( d));
	set_local_visible( extnd_cont( d));
}


C_beginpart( d)
int d;

{
	swtxt();

	symbol_definition( extnd_part( d));
	set_local_visible( extnd_part( d));
}


C_endpart( d)
int d;

{
	swtxt();

	jump(extnd_cont( d));
}


C_getid()
{
	static int id = 0;

	return ++id;
}
