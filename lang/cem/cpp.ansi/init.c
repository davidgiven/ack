/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: INITIALIZATION ROUTINES */

#include    <stdlib.h>
#include    <string.h>
#include    <time.h>
#include	"system.h"
#include	"alloc.h"
#include	"time.h"
#include	"class.h"
#include	"macro.h"
#include	"idf.h"

struct mkey	{
	char *mk_reserved;
	int mk_key;
} mkey[] =	{
	{"define",	K_DEFINE},
	{"elif",	K_ELIF},
	{"else",	K_ELSE},
	{"endif",	K_ENDIF},
	{"error",	K_ERROR},
	{"if",		K_IF},
	{"ifdef",	K_IFDEF},
	{"ifndef",	K_IFNDEF},
	{"include",	K_INCLUDE},
	{"line",	K_LINE},
	{"pragma",	K_PRAGMA},
	{"undef",	K_UNDEF},
	{0,		K_UNKNOWN}
};

char *sprint();

init_pp()
{
	static char *months[12] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	time_t clock;
	static char dbuf[30];
	static char tbuf[30];
	struct tm  *tp;

	/*	Initialise the control line keywords (if, include, define, etc)
		Although the lexical analyzer treats them as identifiers, the
		control line handler can recognize them as keywords by the
		id_resmac field of the identifier.
	*/
	{
		register struct mkey *mk = &mkey[0];

		while (mk->mk_reserved)	{
			register struct idf *idf = str2idf(mk->mk_reserved, 0);
			
			if (idf->id_resmac)
				fatal("maximum identifier length insufficient");
			idf->id_resmac = mk->mk_key;
			mk++;
		}
	}

	/*	Initialize __LINE__, __FILE__, __DATE__, __TIME__,
		and __STDC__ macro definitions.
	*/
	clock = time(NULL);
	tp = localtime(&clock);

	/* __DATE__ */
	sprint(dbuf, "\"%s %2d %d\"", months[tp->tm_mon],
			tp->tm_mday, tp->tm_year+1900);
	/* if (tp->tm_mday < 10) dbuf[5] = ' ';		/* hack */
	macro_def(str2idf("__DATE__", 0), dbuf, -1, strlen(dbuf), NOUNDEF);

	/* __TIME__ */
	sprint(tbuf, "\"%02d:%02d:%02d\"", tp->tm_hour, tp->tm_min, tp->tm_sec);
	macro_def(str2idf("__TIME__", 0), tbuf, -1, strlen(tbuf), NOUNDEF);

	/* __LINE__	*/
	macro_def(str2idf("__LINE__", 0), "0", -1, 1, NOUNDEF | FUNC);

	/* __FILE__	*/
	macro_def(str2idf("__FILE__", 0), "", -1, 1, NOUNDEF | FUNC);

	/* __STDC__ */
	macro_def(str2idf("__STDC__", 0), "1", -1, 1, NOUNDEF);

	/* defined(??) */
	macro_def(str2idf("defined", 0), "", 1, 1, NOUNDEF | FUNC);
}
