/* S Y N T A X   E R R O R   R E P O R T I N G */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"

extern char *symbol2str();
extern struct idf *gen_anon_idf();
int err_occurred = 0;

LLmessage(tk)
	int tk;
{
	++err_occurred;
	if (tk)	{
		error("%s missing", symbol2str(tk));
		insert_token(tk);
	}
	else
		error("%s deleted", symbol2str(dot.tk_symb));
}

insert_token(tk)
	int tk;
{
	aside = dot;

	dot.tk_symb = tk;

	switch (tk)	{
	/* The operands need some body */
	case IDENT:
		dot.TOK_IDF = gen_anon_idf();
		break;
	case STRING:
		dot.TOK_SLE = 1;
		dot.TOK_STR = Salloc("", 1);
		break;
	case INTEGER:
		dot.TOK_INT = 1;
		break;
	case REAL:
		dot.TOK_REL = Salloc("0.0", 4);
		break;
	}
}
