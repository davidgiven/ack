/* S Y N T A X   E R R O R   R E P O R T I N G */

/*	Defines the LLmessage routine. LLgen-generated parsers require the
	existence of a routine of that name.
	The routine must do syntax-error reporting and must be able to
	insert tokens in the token stream.
*/

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"

extern char		*symbol2str();
extern struct idf	*gen_anon_idf();
extern int		err_occurred;

LLmessage(tk)
	int tk;
{
	if (tk)	{
		/* if (tk != 0), it represents the token to be inserted.
		   otherwize, the current token is deleted
		*/
		error("%s missing", symbol2str(tk));
		insert_token(tk);
	}
	else
		error("%s deleted", symbol2str(dot.tk_symb));
}

insert_token(tk)
	int tk;
{
	register struct token *dotp = &dot;

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
		dotp->TOK_REL = Salloc("0.0", 4);
		break;
	}
}
