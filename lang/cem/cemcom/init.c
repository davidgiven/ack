/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* PREPROCESSOR: INITIALIZATION ROUTINES */

#include	"nopp.h"

#ifndef NOPP
#include	<system.h>
#include	<alloc.h>
#include	"class.h"
#include	"macro.h"
#include	"idf.h"
#include	"interface.h"

PRIVATE struct mkey	{
	char *mk_reserved;
	int mk_key;
} mkey[] =	{
	{"define",	K_DEFINE},
	{"elif",	K_ELIF},
	{"else",	K_ELSE},
	{"endif",	K_ENDIF},
	{"if",		K_IF},
	{"ifdef",	K_IFDEF},
	{"ifndef",	K_IFNDEF},
	{"include",	K_INCLUDE},
	{"line",	K_LINE},
	{"undef",	K_UNDEF},
	{"pragma",	K_PRAGMA},
	{0,		K_UNKNOWN}
};

char *strcpy();

EXPORT
init_pp()
{
	long clock, sys_time();
	static char date[30];
	char *ctime();

	/*	Initialise the control line keywords (if, include, define, etc)
		Although the lexical analyzer treats them as identifiers, the
		control line handler can recognize them as keywords by the
		id_resmac field of the identifier.
	*/
	{
		register struct mkey *mk = &mkey[0];

		while (mk->mk_reserved)	{
			register struct idf *idf = str2idf(mk->mk_reserved);
			
			if (idf->id_resmac)
				fatal("maximum identifier length insufficient");
			idf->id_resmac = mk->mk_key;
			mk++;
		}
	}

	/*	Initialize __DATE__, __FILE__ and __LINE__ macro
		definitions.
	*/
	/* __DATE__	*/
	clock = sys_time();
	strcpy(&date[1], ctime(&clock));
	date[26] = '\0';		/* zap nl	*/
	date[0] = date[25] = '"';
	macro_def(str2idf("__DATE__"), date, -1, 26, NOFLAG);

	/* __LINE__	*/
	macro_def(str2idf("__LINE__"), "0", -1, 1, FUNC);

	/* __FILE__	*/
	macro_def(str2idf("__FILE__"), "", -1, 1, FUNC);

	/* defined(??) */
	macro_def(str2idf("defined"), "", 1, 1, FUNC);
}
#endif /* NOPP */
