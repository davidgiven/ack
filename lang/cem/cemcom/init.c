/* $Header$ */
/* PREPROCESSOR: INITIALIZATION ROUTINES */

#include	"nopp.h"

#ifndef NOPP
#include	<system.h>
#include	"predefine.h"	/* UF */
#include	"alloc.h"
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
			struct idf *idf = str2idf(mk->mk_reserved);
			
			if (idf->id_resmac)
				fatal("maximum identifier length insufficient");
			idf->id_resmac = mk->mk_key;
			mk++;
		}
	}

	/*	Initialize __DATE__, __FILE__ and __LINE__ macro
		definitions.  The compile-time specified predefined macros
		are also predefined:  if this file is compiled with
		-DPREDEFINE="vax,pdp", the macro definitions "vax" and
		"pdp" are predefined macros.
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

#ifdef	PREDEFINE
	{
		/*	PREDEFINE is a compile-time defined string
			containing a number of identifiers to be
			predefined at the host machine (for example
			-DPREDEFINE="vax,unix,pmds").
		*/
		register char *s = PREDEFINE;
		register char *id;
		char c;

		for (;;)	{
			while (*s && class(*s++) != STIDF);
			if (*s)	{
				/* gobble identifier */
				id = s - 1;
				while (in_idf(*s++));
				c = *--s;
				*s = '\0';
				macro_def(str2idf(id), "1", -1, 1, PREDEF);
				*s = c;
			}
			else
				break;
		}
	}
#endif	PREDEFINE
}
#endif NOPP
