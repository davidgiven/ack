/*
	Trap handling
*/

/* $Header$ */

#include	<setjmp.h>

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"warn.h"
#include	"mem.h"
#include	"shadow.h"
#include	"linfil.h"
#include	"rsb.h"
#include	"fra.h"

extern jmp_buf trapbuf;			/* from main.c */

int must_test;				/* TEST-bit on in EM header word 2 */
int signalled;

PRIVATE int nonreturnable();

PRIVATE char *trap_msg[] = {
#include	"trap_msg"		/* generated from $(EM)/etc/traps */
	""
};

char *trap2text(nr)			/* transient */
	int nr;
{
	if (	/* trap number in predefined range */
		nr < sizeof (trap_msg) / sizeof (trap_msg[0])
	&&	/* trap message not the empty string */
		trap_msg[nr][0]
	) {
		return trap_msg[nr];
	}
	else {
		static char buf[50];

		sprintf(buf, "TRAP %d", nr);
		return buf;
	}
}

/*ARGSUSED*/
do_trap(nr, L, F)
	int nr;
	int L;
	char *F;
{
	/*
	1.	The trap has not been masked.
	2.	This routine does not return; it either ends in a call of
		fatal() or in a longjmp().
	*/
	static int rec_nr;		/* Recursive trap number */
	static int rec_trap = 0;	/* To detect traps inside do_trap() */
	
	register long tpi;		/* Trap Procedure Identifier */

	LOG(("@t1 trap(%d) [%s: %d]", nr, F, L));
	warning(WMSG + nr);

	switch (OnTrap) {
	case TR_ABORT:
		fatal("trap \"%s\" before program started", trap2text(nr));
		/*NOTREACHED*/

	case TR_HALT:
		fatal("trap \"%s\" not caught at %s",
				trap2text(nr), position());
		/*NOTREACHED*/

	case TR_TRAP:
		/* execute the trap */
		if (rec_trap) {
			fatal("recursive trap; first trap number was \"%s\"",
					trap2text(rec_nr));
		}
		rec_trap = 1;
		rec_nr = nr;

		/* save the Function Return Area */
		pushFRA(FRASize);
		wpush((long)FRASize);
		wpush((long)FRA_def);

		/* set up the trap number as the only parameter */
		wpush((long) nr);

		tpi = TrapPI;		/* allowed since OnTrap == TR_TRAP */
		TrapPI = 0;
		OnTrap = TR_HALT;
		call(tpi, (nonreturnable(nr) ? RSB_NRT : RSB_RTT));
		rec_trap = 0;
		longjmp(trapbuf, 1);
		/*NOTREACHED*/
	}
}

PRIVATE int nonreturnable(nr)
	int nr;
{
	switch (nr) {
	case ESTACK:
	case EILLINS:
	case EODDZ:
	case ECASE:
	case EMEMFLT:
	case EBADPTR:
	case EBADPC:
	case EBADLAE:
	case EBADGTO:
		return 1;
	default:
		return 0;
	}
	/*NOTREACHED*/
}

