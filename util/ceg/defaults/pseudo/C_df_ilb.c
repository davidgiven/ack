#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_df_ilb( l)
label l;
{
	char *s;

	swtxt();
	symbol_definition(s = extnd_ilb( l));
	set_local_visible(s);
}
