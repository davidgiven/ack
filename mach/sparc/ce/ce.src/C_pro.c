#define CODE_EXPANDER
#include <em.h>
#include "back.h"
#include <alloc.h>

extern int B_locals_created;
extern int B_procno;
char	*B_procnam;

void
C_pro( s, l)
char *s;
arith l;
{
	swtxt();
	s = extnd_name(s);
#ifdef __solaris__
	fprint(codefile, "\t.type\t%s,#function\n", s);
	if (B_procnam) free(B_procnam);
	B_procnam = Salloc(s, strlen(s)+1);
#endif

	symbol_definition(  s);
	B_procno++;
	C_prolog();
	C_locals(l);
	B_locals_created = 1;
}
