/* $Header$ */

#include "inputtype.h"
#include "file_info.h"
#include "input.h"
#define INP_TYPE	struct file_info
#define INP_VAR		finfo
struct file_info	finfo;
#include <inp_pkg.body>
#include "nopp.h"

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
	else
	if (p) {
		*p = '\0';
		fn = Salloc(fn, p - &fn[0] + 1);
		*p = '/';
		return fn;
	}
	else return ".";
}
#endif NOPP

int	NoUnstack;

AtEoIT()
{
#ifndef NOPP
	if (NoUnstack) lexerror("unexpected EOF");
	DoUnstack();
#endif NOPP
	return 0;
}

AtEoIF()
{
	if (NoUnstack) lexerror("unexpected EOF");
	return 0;
}
