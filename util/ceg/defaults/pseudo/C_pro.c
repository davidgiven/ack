#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int B_locals_created;
extern int B_procno;

C_pro( s, l)
char *s;
arith l;
{
	swtxt();

	symbol_definition( extnd_name( s));
	B_procno++;
	prolog();
	locals(l);
	B_locals_created = 1;
}
