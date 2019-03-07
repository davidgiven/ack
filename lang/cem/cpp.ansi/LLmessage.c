/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		PARSER ERROR ADMINISTRATION		*/

#include	"arith.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include    "skip.h"
#include    "error.h"

extern char *symbol2str();

void LLmessage(int tk)
{
	if (tk < 0)
		error("garbage at end of line");
	else if (tk)
	{
		error("%s missing", symbol2str(tk));
		if (DOT != EOF)
			SkipToNewLine();
		DOT = EOF;
	}
	else
		error("%s deleted", symbol2str(DOT));
}
