/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint-specific comment handling	*/

#include	<ctype.h>

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>
#include	"interface.h"
#include	"arith.h"
#include	"l_state.h"
#include	"l_comment.h"

extern char loptions[];

/*	Since the lexical analyser does a one-token look-ahead, pseudo-
	comments are read too soon.  This is remedied by first storing them
	in static variables and then moving them to the real variables
	one token later.
*/

PRIVATE int notreached;
PRIVATE int varargsN = -1;
PRIVATE int argsused;
PRIVATE int formatN;
PRIVATE int formatVAR;
PRIVATE char *format;
PRIVATE char *prev_format;

PRIVATE make_format();

int LINTLIB;				/* file is lint library */
int s_NOTREACHED;			/* statement not reached */
int f_VARARGSn;				/* function with variable # of args */
int f_ARGSUSED;				/* function does not use all args */
int f_FORMATn;				/* argument f_FORMATn is f_FORMAT */
char *f_FORMAT;
int f_FORMATvar;			/* but the formal argument may be
					   absent because of varargs.h */

lint_init_comment()
{
	LINTLIB = loptions['L'];
}

lint_comment_ahead()
{
	s_NOTREACHED = notreached;
	notreached = 0;
}

lint_comment_function()
{
	f_ARGSUSED = argsused | loptions['v'];
	argsused = 0;

	f_VARARGSn = varargsN;
	varargsN = -1;

	f_FORMATn = formatN;
	formatN = 0;
	f_FORMAT = format;
	if (format)
		prev_format = format;
	format = 0;

	f_FORMATvar = formatVAR;
	formatVAR = 0;
}

PRIVATE char buf[1000];
PRIVATE char *bufpos;			/* next free position in buf */

lint_start_comment()
{
	bufpos = &buf[0];
}

lint_comment_char(c)
	int c;
{
/* This function is called with every character between /_* and *_/ */
	if (bufpos - &buf[0] < sizeof(buf)-1)
		*bufpos++ = (char)c;
}

lint_end_comment()
{
	*bufpos++ = '\0';
	bufpos = &buf[0];

	/* skip initial blanks */
	while (*bufpos && isspace(*bufpos)) {
		bufpos++;
	}

	/* now test for one of the pseudo-comments */
	if (strncmp(bufpos, "NOTREACHED", 10) == 0) {
		notreached = 1;
	}
	else
	if (strncmp(bufpos, "ARGSUSED", 8) == 0) {
		argsused = 1;
	}
	else
	if (strncmp(bufpos, "LINTLIBRARY", 11) == 0) {
		LINTLIB = 1;
	}
	else
	if (strncmp(bufpos, "VARARGS", 7) == 0) {
		bufpos += 7;
		varargsN = isdigit(*bufpos) ? atoi(bufpos) : 0;
	}
	else
	if (strncmp(bufpos, "FORMAT", 6) == 0 && isdigit(bufpos[6])) {
		register int argn;

		bufpos += 6;
		argn = *bufpos++ - '0';
		varargsN = argn + 1;
		if (*bufpos == 'v') {
			/* something like FORMAT3v */
			formatVAR = 1;
			bufpos++;
		}
		make_format(argn, bufpos);
		
	}
}

/*	We use a small FSA to skip layout inside formats, but to preserve
	a space between letters and digits.
*/

#define	NONE		0
#define	LETGIT		1
#define	LETGITSPACE	2

PRIVATE
make_format(argn, oldf)
	int argn;
	char *oldf;
{
	register char *newf;
	register int last_stat;

	while (*oldf && *oldf != '$') {
		oldf++;
	}
	if (!*oldf) {
		/* no format given, repeat previous format */
		if (!prev_format) {
			warning("format missing and no previous format");
		}
		formatN = argn;
		format = prev_format;
		return;
	}
	if (*oldf++ != '$') {
		warning("no format in FORMAT pseudo-comment");
		format = 0;
		return;
	}

	/* there is a new format to be composed */
	newf = Malloc(strlen(oldf));
		/* certainly enough and probably not overly too much */
	formatN = argn;
	format = newf;

	last_stat = NONE;
	while (*oldf && *oldf != '$') {
		register char ch = *oldf++;

		if (isspace(ch)) {
			if (last_stat == LETGIT)
				last_stat = LETGITSPACE;
		}
		else
		if (isalnum(ch)) {
			switch (last_stat) {
			case NONE:
				last_stat = LETGIT;
				break;
			case LETGITSPACE:
				*newf++ = ' ';
				last_stat = LETGIT;
				break;
			}
			*newf++ = ch;
		}
		else {
			last_stat = NONE;
			*newf++ = ch;
		}
	}
	if (*oldf != '$') {
		warning("no end of format in FORMAT pseudo-comment");
		format = 0;
		return;
	}
	*newf++ = '\0';
}

#endif	/* LINT */
