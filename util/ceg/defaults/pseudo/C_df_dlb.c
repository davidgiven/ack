#include <em.h>
#include "back.h"

C_df_dlb( l)
label l;

/* Bij welk data-segment hoort dit label thuis? ROM, CON, HOL, BSS ???
 * Oplossing : Bewaar het label totdat de eerstvolgende data-instructie aange-
 * roepen wordt, op dat moment het label genereren!!
 */
{
	save_label( extnd_dlb( l));
}
