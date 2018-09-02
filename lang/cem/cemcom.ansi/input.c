/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include "parameters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_info.h"
#include "input.h"

#define INP_PUSHBACK	3
#define INP_TYPE	struct file_info
#define INP_VAR		finfo
struct file_info	finfo;
#include <inp_pkg.body>
#include <alloc.h>

int	NoUnstack;

AtEoIT()
{
	return 0;
}

extern char *source;

AtEoIF()
{
	if (NoUnstack) lexerror("unexpected EOF");
	return 0;
}
