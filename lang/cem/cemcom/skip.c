/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* PREPROCESSOR: INPUT SKIP FUNCTIONS */

#include	"nopp.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"input.h"
#include	"interface.h"

#ifndef NOPP
PRIVATE int
skipspaces(ch)
	register int ch;
{
	/*	skipspaces() skips any white space and returns the first
		non-space character.
	*/
	for (;;) {
		while (class(ch) == STSKIP)
			LoadChar(ch);

		/* How about "\\\n"?????????	*/

		if (ch == '/') {
			LoadChar(ch);
			if (ch == '*') {
				skipcomment();
				LoadChar(ch);
			}
			else	{
				PushBack();
				return '/';
			}
		}
		else
			return ch;
	}
}
#endif NOPP

PRIVATE 
skipline()
{
	/*	skipline() skips all characters until a newline character
		is seen, not escaped by a '\\'.
		Any comment is skipped.
	*/
	register int c;

	LoadChar(c);
	while (class(c) != STNL && c != EOI) {
		if (c == '\\') {
			LoadChar(c);
			if (class(c) == STNL)
				++LineNumber;
		}
		if (c == '/') {
			LoadChar(c);
			if (c == '*')
				skipcomment();
			else
				continue;
		}
		LoadChar(c);
	}
	++LineNumber;

	if (c == EOI) {		/* garbage input...		*/
		lexerror("unexpected EOF while skipping text");
		PushBack();
	}
}
