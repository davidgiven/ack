/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* U S E R   O P T I O N - H A N D L I N G */

/* stripped down version from the one in the Modula-2 compiler */

/* $Id$ */

#include	<alloc.h>
#include	"main.h"

static int	ndirs = 1;

DoOption(text)
	register char *text;
{
	extern char *mflags;
	extern char *suff;
	extern char *compiler;
	extern char *llibs;

	switch(*text++)	{

	case 'L' :
		AddLibDir(text);
		break;

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

	case 'l':
		{	static unsigned int liblen = 0;
			unsigned int len = strlen(text) + 4;
			
			if (liblen) {
				llibs = Realloc(llibs, liblen += len);
			}
			else {
				llibs = Malloc(liblen = len);
				*llibs = '\0';
			}
			strcat(llibs,"\\\n\t");
			strcat(llibs, text);
		}
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

	for (i = ndirs++; i < nDEF; i++) {
		char *tmp = DEFPATH[i];

		DEFPATH[i] = new;
		new = tmp;
	}
}

AddLibDir(text)
	char *text;
{
	if (*text) {
		set_libdir(ndirs);
		AddInclDir(text);
	}
}
