#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int B_locals_created;
extern int B_procno;

void
C_end( l)
arith l;
{	
	char *p;

	swtxt();

	if ( !B_locals_created) {
		p = extnd_pro( B_procno);

		symbol_definition( p);
		set_local_visible( p);
		C_locals( l);
		C_jump( extnd_start( B_procno));
	}
}
