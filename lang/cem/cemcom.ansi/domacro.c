/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: CONTROLLINE INTERPRETER */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "parameters.h"
#include "idf.h"
#include "arith.h"
#include "LLlex.h"
#include "Lpars.h"
#include "input.h"

#ifdef DBSYMTAB
#include <stb.h>
#include <em.h>
int IncludeLevel = 0;
#endif

extern char options[];

struct idf* GetIdentifier(skiponerr) int skiponerr; /* skip the rest of the line on error */
{
	/*	returns a pointer to the descriptor of the identifier that is
	    read from the input stream. When the input does not contain
	    an identifier, the rest of the line is skipped when
	    skiponerr is on, and a null-pointer is returned.
	    The substitution of macros is disabled.
	*/
	int tok;
	struct token tk;

	tok = GetToken(&tk);
	if (tok != IDENTIFIER)
	{
		if (skiponerr && tok != EOI)
			SkipToNewLine();
		return (struct idf*)0;
	}
	return tk.tk_idf;
}

domacro()
{
	int tok;
	struct token tk;

	EoiForNewline = 1;
	if ((tok = GetToken(&tk)) == IDENTIFIER)
	{
		if (!strcmp(tk.tk_idf->id_text, "pragma"))
		{
			do_pragma();
			EoiForNewline = 0;
			return;
		}
	}
	else if (tok == INTEGER)
	{
		do_line((unsigned int)tk.tk_ival);
		EoiForNewline = 0;
		return;
	}
	lexerror("illegal # line");
	EoiForNewline = 0;
	SkipToNewLine();
}

do_line(l) unsigned int l;
{
	struct token tk;
	int t = GetToken(&tk);

	if (t != EOI)
		SkipToNewLine();
	LineNumber = l; /* the number of the next input line */
	if (t == STRING)
	{ /* is there a filespecifier? */
/*
 * Do not attempt to free the old string, since it might
 * be used in a def structure.
 */
#ifdef DBSYMTAB
		if (options['g'] && strcmp(FileName, tk.tk_bts) != 0)
		{
			C_ms_std(tk.tk_bts, N_SOL, 0);
		}
#endif /* DBSYMTAB */
		FileName = tk.tk_bts;
	}
}
