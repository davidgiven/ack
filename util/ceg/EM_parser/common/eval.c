#include <ctype.h>
#include "decl.h"


eval( str)
char *str;

/* Output 'str' and replace in it all $i occurances by the corrresponding
 * parameter-names, converted where necessary.
 */
{
	register char c;
	int i;

	for ( c = *str++; c != '\0'; c= *str++)
		if ( c == '$' && isdigit( *str)) {

			i = *str++ - '0' - 1;
			
			out( "%s", C_instr_info->arg_conv[i]);
		}
		else
			out( "%c", c);
}
