/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S Y N T A X   E R R O R   R E P O R T I N G */

/* stripped down version from the one in the Modula-2 compiler */

/* $Id$ */

/*	Defines the LLmessage routine. LLgen-generated parsers require the
	existence of a routine of that name.
	The routine must do syntax-error reporting and must be able to
	insert tokens in the token stream.
*/

#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"

extern char		*symbol2str();
extern struct idf	*gen_anon_idf();

LLmessage(tk)
	register int tk;
{
	if (tk > 0)	{
		/* if (tk > 0), it represents the token to be inserted.
		*/
		error("%s missing before %s", symbol2str(tk), symbol2str(dot.tk_symb));

		aside = dot;

		dot.tk_symb = tk;

		switch (tk)	{
		/* The operands need some body */
		case IDENT:
			dot.TOK_IDF = gen_anon_idf();
			break;
		}
	}
	else if (tk  < 0) {
		error("garbage at end of program");
	}
	else	error("%s deleted", symbol2str(dot.tk_symb));
}
