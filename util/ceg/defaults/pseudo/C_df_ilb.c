#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int B_procno;

void
C_df_ilb( l)
label l;
{
	char *s;

	swtxt();
	symbol_definition(s = extnd_ilb( l, B_procno));
	set_local_visible(s);
}
