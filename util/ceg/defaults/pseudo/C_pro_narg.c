#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int locals_created; 

C_pro_narg( s)
char*s;

/* Het aantal locale variabelen is nog niet bekend, maar het stack-frame moet
 * nu wel gemaakt worden! Oplossing : Pas bij bij C_end() is het aantal locale
 * variabelen bekend dus nu een "jump" genereren en bij C_end() prolog() aan-
 * roepen en daarna terug-jump-en naar het begin van de EM-procedure.
 */
{
	swtxt();

	symbol_definition( extnd_name( s));
	procno++;
	prolog();
	locals_created = 0;
	jump( extnd_pro( procno));
	symbol_definition( extnd_start( procno));
	set_local_visible( extnd_start( procno));
}
