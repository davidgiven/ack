/* S Y N T A X   E R R O R   R E P O R T I N G */

/*	Defines the LLmessage routine. LLgen-generated parsers require the
	existence of a routine of that name.
	The routine must do syntax-error reporting and must be able to
	insert tokens in the token stream.
*/

#include	<alloc.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<em_arith.h>
#include	<em_label.h>

#include    "parameters.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"idf.h"
#include	"node.h"
#include	"type.h"
#include	"misc.h"
#include	"error.h"

extern int expect_label;

void LLmessage(int tk)
{
	if( tk > 0 )	{
		/* if( tk > 0 ), it represents the token to be inserted.
		*/
		struct token *dotp = &dot;

		error("%s missing before %s", symbol2str(tk), symbol2str(dotp->tk_symb));

		aside = *dotp;
		asidetype = toktype;

		dotp->tk_symb = tk;

		switch( tk )	{
		/* The operands need some body */
		case IDENT:
			dotp->TOK_IDF = gen_anon_idf();
			break;
		case STRING:
			dotp->tk_data.tk_str = (struct string *)
						malloc(sizeof (struct string));
			dotp->TOK_SLE = 1;
			dotp->TOK_STR = strdup("");
			toktype = standard_type(T_STRINGCONST, 1, (arith) 1);
			break;
		case INTEGER:
			toktype = int_type;
			if( !expect_label )
				dotp->TOK_INT = 1;
			else
				dotp->TOK_INT = -1;
			break;
		case REAL:
			dotp->tk_data.tk_real = (struct real *)
						malloc(sizeof(struct real));
			/* inverse struct */
			dotp->TOK_RIV = (struct real *)
						malloc(sizeof(struct real));
			dotp->TOK_RIV->r_inverse = dotp->tk_data.tk_real;

			dotp->TOK_REL = strdup("0.0");
			dotp->TOK_RIV->r_real = dotp->TOK_REL;
			toktype = real_type;
			break;
		}
	}
	else if( tk < 0 ) error("garbage at end of program");
	     else error("%s deleted", symbol2str(dot.tk_symb));
}
