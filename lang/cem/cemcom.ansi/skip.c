/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: INPUT SKIP FUNCTIONS */

#include "parameters.h"
#include "arith.h"
#include "LLlex.h"
#include "class.h"
#include "input.h"
#include "skip.h"


int SkipToNewLine(void)
{
	register int ch;
	register int garbage = 0;

	while ((ch = GetChar()) != '\n')
	{
		if (!is_wsp(ch))
			garbage = 1;
	}
	++LineNumber;
	return garbage;
}
