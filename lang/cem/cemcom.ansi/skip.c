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
int
skipspaces(ch, skipnl)
	register int ch;
{
	/*	skipspaces() skips any white space and returns the first
		non-space character.
	*/
	for (;;) {
		while (class(ch) == STSKIP)
			ch = GetChar();
		if (skipnl && class(ch) == STNL) {
			ch = GetChar();
			++LineNumber;
			continue;
		}

		/* \\\n are handled by trigraph */

		if (ch == '/') {
			ch = GetChar();
			if (ch == '*') {
				skipcomment();
				ch = GetChar();
			}
			else	{
				UnGetChar();
				return '/';
			}
		}
		else
			return ch;
	}
}
#endif NOPP

SkipToNewLine(garbage)
	int garbage;
{
	register int ch;
	register int pstrict = 0;

	UnGetChar();
	while ((ch = GetChar()) != '\n') {
		if (ch == '/') {
			if ((ch = GetChar()) == '*') {
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
