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

#ifndef NOPP
extern int InputLevel;

int
skipspaces(ch, skipnl)
	register int ch;
{
	/*	skipspaces() skips any white space and returns the first
		non-space character.
	*/
	register int nlseen = 0;

	for (;;) {
		while (class(ch) == STSKIP)
			ch = GetChar();
		if (skipnl && class(ch) == STNL) {
			ch = GetChar();
			LineNumber++;
			nlseen++;
			continue;
		}
		if (ch == TOKSEP && InputLevel) {
			ch = GetChar();
			continue;
		}

		/* \\\n are handled by trigraph */

		if (ch == '/') {
			ch = GetChar();
			if (ch == '*' && !InputLevel) {
				skipcomment();
				ch = GetChar();
			}
			else	{
				UnGetChar();
				return '/';
			}
		}
		else if (nlseen && ch == '#') {
			domacro();
			ch = GetChar();
		} else
			return ch;
	}
}
#endif NOPP

SkipToNewLine(garbage)
	int garbage;
{
	register int ch;
	register int pstrict = 0;

	while ((ch = GetChar()) != '\n') {
		if (ch == '/') {
			if ((ch = GetChar()) == '*'
#ifndef NOPP
			    && !InputLevel
#endif
			) {
				skipcomment();
				continue;
			}
		}
		if (garbage && !is_wsp(ch))
			pstrict = 1;
	}
	++LineNumber;
	return pstrict;
}
