/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*		PARSER ERROR ADMINISTRATION		*/

#include	<alloc.h>
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
#ifndef LLNONCORR
		error("%s missing before %s", symbol2str(tk), symbol2str(DOT));
#endif
		insert_token(tk);
	}
	else	{
#ifndef LLNONCORR
		error("%s deleted", symbol2str(DOT));
#else
		error("%s not expected", symbol2str(DOT));
#endif
	}
	tk_nmb_at_last_syn_err = token_nmb;
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
		dot.tk_idf = str2idf("int", 0);
		break;
	case STRING:
		dot.tk_bts = Salloc("", 1);
		dot.tk_len = 1;
		break;
	case INTEGER:
		dot.tk_fund = INT;
		dot.tk_ival = 1;
		break;
	case FLOATING:
		dot.tk_fval = Salloc("0.0", 4);
		break;
	}
}
