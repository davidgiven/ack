/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S Y N T A X   E R R O R   R E P O R T I N G */

/* $Id$ */

/*	Defines the LLmessage routine. LLgen-generated parsers require the
	existence of a routine of that name.
	The routine must do syntax-error reporting and must be able to
	insert tokens in the token stream.
*/

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include 	"parameters.h"
#include	"idf.h"
#include	"error.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"misc.h"

extern char		*symbol2str();


void LLmessage(register int tk)
{
	if (tk > 0)	{
		/* if (tk > 0), it represents the token to be inserted.
		*/
		register t_token *dotp = &dot;

#ifndef LLNONCORR
		error("%s missing before %s", symbol2str(tk), symbol2str(dotp->tk_symb));
#endif

		aside = *dotp;

		dotp->tk_symb = tk;

		switch (tk)	{
		/* The operands need some body */
		case IDENT:
			dotp->TOK_IDF = gen_anon_idf();
			break;
		case STRING:
			dotp->tk_data.tk_str = (struct string *)
						Malloc(sizeof (struct string));
			dotp->TOK_SLE = 1;
			dotp->TOK_STR = Salloc("", 1);
			break;
		case INTEGER:
			dotp->TOK_INT = 1;
			break;
		case REAL:
			dotp->tk_data.tk_real = new_real();
			dotp->TOK_RSTR = Salloc("0.0", 4);
			flt_str2flt(dotp->TOK_RSTR, &dotp->TOK_RVAL);
			break;
		}
	}
	else if (tk  < 0) {
		error("end of file expected");
	}
	else	{
#ifndef LLNONCORR
		error("%s deleted", symbol2str(dot.tk_symb));
#else
		error("%s not expected", symbol2str(dot.tk_symb));
#endif
	}
	tk_nmb_at_last_syn_err = token_nmb;
}

