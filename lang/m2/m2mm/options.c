/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   O P T I O N - H A N D L I N G */

/* stripped down version from the one in the Modula-2 compiler */

/* $Header$ */

#include	<alloc.h>
#include	"main.h"

static int	ndirs = 1;

DoOption(text)
	register char *text;
{
	extern char *mflags;
	extern char *suff;
	extern char *compiler;

	switch(*text++)	{

	case 'I' :
		AddInclDir(text);
		break;
	
	case 'M':
		mflags = text;
		break;

	case 'C':
		compiler = text;
		break;

	case 'S':
		suff = text;
		break;

	default:
		Gerror("Unrecognized option: -%s", text-1);
		break;
	}
}

AddInclDir(text)
	char *text;
{
	register int i;
	register char *new = text;

	if (! *text) {
		DEFPATH[ndirs] = 0;
		return;
	}

	if (++nDEF > mDEF) {
		mDEF += 10;
		DEFPATH = (char **) Realloc((char *)DEFPATH,
					(unsigned)(mDEF * sizeof(char *)));
	}

	i = ndirs++;
	while (new) {
		register char *tmp = DEFPATH[i];

		DEFPATH[i++] = new;
		new = tmp;
	}
}
