#include <em.h>
#include <system.h>

extern File *codefile;

extern char *extnd_name(), *extnd_dnam(), *extnd_dlb(), *extnd_ilb(),
	    *extnd_hol(), *extnd_ext(), *extnd_pro(), *extnd_start(),
	    *extnd_part(), *extnd_cont();

extern int holno, procno;

#define 	swtxt() 	switchseg( SEGTXT)

#define 	SEGTXT		0
#define 	SEGROM		1
#define 	SEGCON		2
#define 	SEGBSS		3
#define 	SEGHOL		-1	/* Does not exist */

#define 	ABSOLUTE	1
