/* $Header$ */

#include	"tokenname.h"
#include	"Lpars.h"
#include	"position.h"
#include	"file.h"
#include	"idf.h"
#include	"misc.h"

/*	To centralize the declaration of %tokens, their presence in this
	file is taken as their declaration. The Makefile will produce
	a grammar file (tokenfile.g) from this file. This scheme ensures
	that all tokens have a printable name.
	Also, the "token2str.c" file is produced from this file.
*/

#if 0
struct tokenname tkspec[] =	{	/* the names of the special tokens */
	{NAME, "name"},
	{STRING, "string"},
	{INTEGER, "number"},
	{EXPRESSION, "<expression>"},
	{REAL, "real"},
	{CHAR, "char"},
	{BIN_OP, "<operator>"},
	{PREF_OR_BIN_OP, "<operator>"},
	{PREF_OP, "<operator>"},
	{POST_OP, "<operator>"},
	{SEL_OP, "<operator>"},
	{0, ""}
};
#endif

struct tokenname tkidf[] =	{	/* names of the identifier tokens */
	{LIST, "list"},
	{XFILE, "file"},
	{RUN, "run"},
	{RERUN, "rerun"},
	{STOP, "stop"},
	{WHEN, "when"},
	{AT, "at"},
	{IN, "in"},
	{ON, "on"},
	{IF, "if"},
	{CONT, "cont"},
	{STEP, "step"},
	{NEXT, "next"},
	{REGS, "regs"},
	{WHERE, "where"},
	{STATUS, "status"},
	{DELETE, "delete"},
	{PRINT, "print"},
	{DUMP, "dump"},
	{RESTORE, "restore"},
	{TRACE, "trace"},
	{SET, "set"},
	{TO, "to"},
	{FIND, "find"},
	{DISPLAY, "display"},
	{WHICH, "which"},
	{HELP, "help"},
	{DISABLE,"disable"},
	{ENABLE,"enable"},
	{SOURCE, "source"},
	{FRAME, "frame"},
	{LOG, "log"},
	{-1, "quit"},
	{0, ""}
};

#if 0
struct tokenname tkinternal[] = {	/* internal keywords	*/
	{0, "0"}
};

struct tokenname tkstandard[] =	{	/* standard identifiers */
	{0, ""}
};
#endif

/* Some routines to handle tokennames */

reserve(resv)
	register struct tokenname *resv;
{
	/*	The names of the tokens described in resv are entered
		as reserved words.
	*/
	register struct idf *p;

	while (resv->tn_symbol)	{
		p = str2idf(resv->tn_name, 0);
		if (!p) fatal("out of Memory");
		p->id_reserved = resv->tn_symbol;
		resv++;
	}
}
