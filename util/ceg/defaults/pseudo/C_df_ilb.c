#define CODE_EXPANDER
#include <em.h>
#include "back.h"

C_df_ilb( l)
label l;
{
	symbol_definition( extnd_ilb( l));
	set_local_visible( extnd_ilb( l));
}
