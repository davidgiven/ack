/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: INPUT SKIP FUNCTIONS */

#include	"LLlex.h"
#include	"class.h"
#include	"input.h"

int
skipspaces(ch, skipnl)
	register int ch;
{
	/*	skipspaces() skips any white space and returns the first
		non-space character.
	*/
	register int nlseen = 0;

	for (;;) {
		while (class(ch) == STSKIP) {
			nlseen = 0;
			LoadChar(ch);
		}
		if (skipnl && class(ch) == STNL) {
			LoadChar(ch);
			++LineNumber;
			nlseen++;
			continue;
		}
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
		else if (nlseen && ch == '#') {
			domacro();
			LoadChar(ch);
			/* ch is the first character of a line. This means
			 * that nlseen will still be true.
			 */
		} else
			return ch;
	}
}

skipline()
{
	/*	skipline() skips all characters until a newline character
		is seen, not escaped by a '\\'.
		Any comment is skipped.
	*/
	register int c;

	LoadChar(c);
	while (class(c) != STNL && c != EOI) {
		if (class(c) == STSTR || class(c) == STCHAR) {
			register int stopc = c;
			int escaped;
			do {
				escaped = 0;
				LoadChar(c);
				if (class(c) == STNL || c == EOI) {
					break;
				}
				if (c == '\\') {
					LoadChar(c);
					if (c == '\n') {
						++LineNumber;
					}
					else escaped = 1;
				}
			} while (escaped || c != stopc);
			if (class(c) != STNL && c != EOI) {
				LoadChar(c);
			}
			continue;
		}
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
		error("unexpected EOF while skipping text");
		PushBack();
	}
}
