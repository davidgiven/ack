#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int B_locals_created;
extern int B_procno;

C_end( l)
arith l;
{	
	char *p;

	swtxt();

	if ( !B_locals_created) {
		p = extnd_pro( B_procno);

		symbol_definition( p);
		set_local_visible( p);
		locals( l);
		jump( extnd_start( B_procno));
	}
}
