/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include "inputtype.h"
#include "file_info.h"
#include "input.h"

#define INP_PUSHBACK	3
#define INP_TYPE	struct file_info
#define INP_VAR		finfo
struct file_info	finfo;
extern int		nestlevel;
#include "nopp.h"
#include <inp_pkg.body>

#ifndef NOPP
char *
getwdir(fn)
	register char *fn;
{
	register char *p;
	char *strrindex();

	p = strrindex(fn, '/');
	while (p && *(p + 1) == '\0') {	/* remove trailing /'s */
		*p = '\0';
		p = strrindex(fn, '/');
	}

	if (fn[0] == '\0' || (fn[0] == '/' && p == &fn[0])) /* absolute path */
		return "";
	if (p) {
		*p = '\0';
		fn = Salloc(fn,(unsigned) (p - &fn[0] + 1));
		*p = '/';
		return fn;
	}
	return "";
}
#endif NOPP

int	NoUnstack;
int	InputLevel;
#if 0
#endif

AtEoIT()
{
	InputLevel--;
	unstackrepl();
	return 0;
}

AtEoIF()
{
#ifndef NOPP
	if (nestlevel != nestlow) lexwarning("missing #endif");
	else
#endif NOPP
	if (NoUnstack) lexerror("unexpected EOF");
#ifndef NOPP
	nestlevel = nestlow;
#endif
	return 0;
}
