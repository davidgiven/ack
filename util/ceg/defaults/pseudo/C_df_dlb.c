#define CODE_EXPANDER
#include <em.h>
#include "back.h"

void
C_df_dlb( l)
label l;

/* Bij welk data-segment hoort dit label thuis? ROM, CON, HOL, BSS ???
 * Oplossing : Bewaar het label totdat de eerstvolgende data-instructie aange-
 * roepen wordt, op dat moment het label genereren!!
 */
{
	save_label( extnd_dlb( l));
}
