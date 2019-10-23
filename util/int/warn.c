/** @file
 *  Warning management.
*/

/* $Id$ */

#include	<stdio.h>

#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"alloc.h"
#include	"warn.h"
#include	"linfil.h"

extern FILE *mess_fp;			/* from io.c */
extern char *trap2text();		/* from trap.c */

/********  The warnings  ********/

struct warn_msg {
	char *wm_text;
	int wm_nr;
};

#define	WMASK		0x5555		/* powers of 4 */

PRIVATE struct warn_msg warn_msg[] = {
#include	"warn_msg"		/* generated from $(EM)/doc/int */
	{0,		0}		/* sentinel */
};

PRIVATE char *warn_text[WMSG+1];

void init_wmsg(void)
{
	register int i;
	register struct warn_msg *wmsg;

	for (i = 0; i <= WMSG; i++) {
		warn_text[i] = "*** Unknown warning (internal error) ***";
	}
	
	for (wmsg = &warn_msg[0]; wmsg->wm_nr; wmsg++) {
		warn_text[wmsg->wm_nr] = wmsg->wm_text;
	}
}

/********  The warning counters  ********/

struct warn_cnt {
	struct warn_cnt *next;
	ptr wc_fil;			/* file name pointer */
	long wc_lin;			/* line number */
	long wc_cnt;			/* the counter */
};

PRIVATE struct warn_cnt *warn_cnt[WMSG];
PRIVATE char warnmask[WMSG];

PRIVATE long count_wrn(int nr)
{	/*	returns the occurrence counter for the warning with number
		nr; keeps track of the warnings, sorted by warning number,
		file name and line number.
	*/
	register struct warn_cnt **warn_hook = &warn_cnt[nr];
	register struct warn_cnt *wrn;

	while ( (wrn = *warn_hook) ) {
		if (wrn->wc_fil == FIL && wrn->wc_lin == LIN) {
			return ++wrn->wc_cnt;
		}
		warn_hook = &wrn->next;
	}

	wrn = (struct warn_cnt *)
		Malloc((size) sizeof (struct warn_cnt), (char *)0);
	if (!wrn) {
		/* no problem */
		return 1;
	}
	*warn_hook = wrn;
	wrn->next = 0;
	wrn->wc_fil = FIL;
	wrn->wc_lin = LIN;
	wrn->wc_cnt = 1;
	return 1;
}

/******** The handling ********/

#define	wmask_on(i)	(warnmask[i])

PRIVATE int latest_warning_printed;	/* set if ... */

/*ARGSUSED*/
void do_warn(int nr, int L, const char *F)
{
	latest_warning_printed = 0;
	if (nr < WMSG) {
		if (!wmask_on(nr)) {
			register long wrn_cnt = count_wrn(nr);
			register char *wmsgtxt = warn_text[nr];
			
			LOG(("@w1 warning: %s [%s: %d]", wmsgtxt, F, L));
			if (	/* wrn_cnt is a power of two */
				!((wrn_cnt-1) & wrn_cnt)
			&&	/* and it is the right power of two */
				(WMASK & wrn_cnt)
			) {
				fprintf(mess_fp,
					"(Warning %d, #%ld): %s at %s\n",
					nr, wrn_cnt, wmsgtxt, position());
				latest_warning_printed = 1;
			}
		}
	}
	else {
		/* actually a trap number */
		nr -= WMSG;
		
		fprintf(mess_fp, "(Warning): Trap occurred - %s at %s\n",
					trap2text(nr), position());
	}
}

#ifdef	LOGGING

void warningcont(int nr)
{
	/* continued warning */
	if (latest_warning_printed) {
		if (!wmask_on(nr)) {
			register char *wmsgtxt = warn_text[nr];
			
			LOG(("@w1 warning cont.: %s", wmsgtxt));
			fprintf(mess_fp,
				"(Warning %d, cont.): %s at %s\n",
					nr, wmsgtxt, position());
		}
	}
}

#endif	/* LOGGING */

void set_wmask(int i)
{
	if (i < WMSG) {
		warnmask[i] = 1;
	}
}

