#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int locals_created;

C_pro( s, l)
char *s;
arith l;
{
	swtxt();

	symbol_definition( extnd_name( s));
	procno++;
	prolog();
	locals(l);
	locals_created = 1;
}
