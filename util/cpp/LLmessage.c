/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*		PARSER ERROR ADMINISTRATION		*/

#include	"LLlex.h"
#include	"Lpars.h"

extern char *symbol2str();

LLmessage(tk)	{
	if (tk < 0)
		error("garbage at end of line");
	else if (tk)	{
		error("%s missing", symbol2str(tk));
		skipline();
		DOT = EOF;
	}
	else
		error("%s deleted", symbol2str(DOT));
}
