/* $Header$ */

#include	"logging.h"
#include	"global.h"
#include	"mem.h"
#include	"shadow.h"
#include	"fra.h"
#include	"alloc.h"

#ifdef	LOGGING
char *FRA_sh;				/* shadowbytes */
#endif	/* LOGGING */

init_FRA() {
	FRA = Malloc(FRALimit, "Function Return Area");
#ifdef	LOGGING
	FRA_sh = Malloc(FRALimit, "shadowspace for Function Return Area");
#endif	/* LOGGING */
	FRA_def = UNDEFINED;		/* set FRA illegal */
}

pushFRA(sz)
	size sz;
{
	register int i;

	if (sz == 0)
		return;

	st_inc(max(sz, wsize));
	for (i = 0; i < sz; i++) {
		stack_loc(SP + i) = FRA[i];
#ifdef	LOGGING
		st_sh(SP + i) = (i < FRASize ? FRA_sh[i] : UNDEFINED);
#endif	/* LOGGING */
	}
}

popFRA(sz)
	size sz;
{
	register int i;

	if (sz == 0)
		return;

	for (i = 0; i < sz; i++) {
		FRA[i] = stack_loc(SP + i);
#ifdef	LOGGING
		FRA_sh[i] = st_sh(SP + i);
#endif	/* LOGGING */
	}
	st_dec(max(sz, wsize));
}

