/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_info.h"
#include "input.h"
#include "error.h"
#include "replace.h"
#include "domacro.h"

#define INP_NPUSHBACK 3
#define INP_TYPE struct file_info
#define INP_VAR finfo
struct file_info finfo;
#include <inp_pkg.body>
#include <alloc.h>

char* getwdir(register char* fn)
{
	register char* p;

	p = strrchr(fn, '/');
	while (p && *(p + 1) == '\0')
	{ /* remove trailing /'s */
		*p = '\0';
		p = strrchr(fn, '/');
	}

	if (fn[0] == '\0' || (fn[0] == '/' && p == &fn[0])) /* absolute path */
		return "";
	if (p)
	{
		*p = '\0';
		fn = Salloc(fn, (unsigned)(p - &fn[0] + 1));
		*p = '/';
		return fn;
	}
	return ".";
}

int NoUnstack;
int InputLevel;

int AtEoIT(void)
{
	InputLevel--;
	/* if (NoUnstack) warning("unexpected EOF"); ??? */
	unstackrepl();
	return 0;
}

int AtEoIF(void)
{
	/* Configure inp_pkg to always return EOIs at the end of source files. */
	return 1;
}
