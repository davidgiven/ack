#define CODE_EXPANDER
#include <em.h>
#include "mach.h"
#include "back.h"

C_con_scon( s, n)
char *s;
arith n;
{
	switchseg( SEGCON);
	dump_label();	

	con_str( s, n);
}
