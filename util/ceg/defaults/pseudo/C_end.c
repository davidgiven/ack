#include <em.h>
#include "back.h"

extern int locals_created;

C_end( l)
arith l;
{	
	char *p;

	swtxt();

	if ( !locals_created) {
		p = extnd_pro( procno);

		symbol_definition( p);
		set_local_visible( p);
		locals( l);
		jump( extnd_start( procno));
	}
}
