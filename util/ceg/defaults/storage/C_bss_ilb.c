#define CODE_EXPANDER
#include <em.h>
#include "mach.h"
#include "back.h"

#if EM_WSIZE == 1
#define conEM_WSIZE	con1
#define romEM_WSIZE	rom1
#define CAST_WSIZE	ONE_BYTE
#endif
#if EM_WSIZE == 2
#define conEM_WSIZE	con2
#define romEM_WSIZE	rom2
#define CAST_WSIZE	TWO_BYTES
#endif
#if EM_WSIZE == 4
#define conEM_WSIZE	con4
#define romEM_WSIZE	rom4
#define CAST_WSIZE	FOUR_BYTES
#endif

#if EM_PSIZE == 1
#define relocEM_PSIZE	reloc1
#endif
#if EM_PSIZE == 2
#define relocEM_PSIZE	reloc2
#endif
#if EM_PSIZE == 4
#define relocEM_PSIZE	reloc4
#endif

extern int B_procno;

void
C_bss_ilb( n, l, i)
arith n;
label l;
int i;
{
	if ( i == 0) {
		common( n);
	}
	else {  
		switchseg( SEGCON);
		dump_label();

		for ( i = 0; i < n/EM_PSIZE; i++)
			relocEM_PSIZE( extnd_ilb( l, B_procno), 0, ABSOLUTE);
	}
}
