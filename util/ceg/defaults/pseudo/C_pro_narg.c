#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int B_locals_created; 
extern int B_procno; 

void
C_pro_narg( s)
char*s;

/* The number of locals is unknown, but the stackframe must be made anyway.
 * Solution: jump to end of procedure, where C_end() will generate code to
 * create the stackframe, and also will generate code to jump back.
 */
{
	char *ss;
	swtxt();

	symbol_definition( extnd_name( s));
	B_procno++;
	C_prolog();
	B_locals_created = 0;
	C_jump( extnd_pro( B_procno));
	symbol_definition( ss = extnd_start( B_procno));
	set_local_visible( ss);
}
