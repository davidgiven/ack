/* $Header$ */
/*		PARSER ERROR ADMINISTRATION		*/

#include	<alloc.h>
#include	"nofloat.h"
#include	"idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"Lpars.h"

extern char *symbol2str();

LLmessage(tk)	{
	err_occurred = 1;
	if (tk < 0)	{
		error("end of file expected");
	}
	else if (tk)	{
		error("%s missing", symbol2str(tk));
		insert_token(tk);
	}
	else
		error("%s deleted", symbol2str(DOT));
}

insert_token(tk)
	int tk;
{
	aside = dot;

	DOT = tk;

	switch (tk)	{
	/* The operands need some body */
	case IDENTIFIER:
		dot.tk_idf = gen_idf();
		break;
	case TYPE_IDENTIFIER:
		dot.tk_idf = str2idf("int");
		break;
	case STRING:
		dot.tk_bts = Salloc("", 1);
		dot.tk_len = 1;
		break;
	case INTEGER:
		dot.tk_fund = INT;
		dot.tk_ival = 1;
		break;
#ifndef NOFLOAT
	case FLOATING:
		dot.tk_fval = Salloc("0.0", 4);
		break;
#endif NOFLOAT
	}
}
