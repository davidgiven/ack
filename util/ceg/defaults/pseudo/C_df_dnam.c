#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_df_dnam( s)
char *s;
{
	save_label( extnd_name( s));
}
