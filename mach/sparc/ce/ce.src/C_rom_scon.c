#define CODE_EXPANDER
#include <em.h>
#include "mach.h"
#include "back.h"

C_rom_scon( s, n)
char *s;
arith n;
{
	switchseg( SEGROM);
	dump_label();	

	rom_str( s,n);
}
