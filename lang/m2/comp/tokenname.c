/* T O K E N   D E F I N I T I O N S */

static char *RcsId = "$Header$";

#include	"tokenname.h"
#include	"Lpars.h"
#include	"idf.h"

/*	To centralize the declaration of %tokens, their presence in this
	file is taken as their declaration. The Makefile will produce
	a grammar file (tokenfile.g) from this file. This scheme ensures
	that all tokens have a printable name.
	Also, the "token2str.c" file is produced from this file.
*/

struct tokenname tkspec[] =	{	/* the names of the special tokens */
	{IDENT, "identifier"},
	{STRING, "string"},
	{INTEGER, "integer"},
	{REAL, "real"},
	{0, ""}
};

struct tokenname tkcomp[] =	{	/* names of the composite tokens */
	{UNEQUAL, "<>"},
	{LESSEQUAL, "<="},
	{GREATEREQUAL, ">="},
	{UPTO, ".."},
	{BECOMES, ":="},
	{0, ""}
};

struct tokenname tkidf[] =	{	/* names of the identifier tokens */
	{AND, "AND"},
	{ARRAY, "ARRAY"},
	{BEGIN, "BEGIN"},
	{BY, "BY"},
	{CASE, "CASE"},
	{CONST, "CONST"},
	{DEFINITION, "DEFINITION"},
	{DIV, "DIV"},
	{DO, "DO"},
	{ELSE, "ELSE"},
	{ELSIF, "ELSIF"},
	{END, "END"},
	{EXIT, "EXIT"},
	{EXPORT, "EXPORT"},
	{FOR, "FOR"},
	{FROM, "FROM"},
	{IF, "IF"},
	{IMPLEMENTATION, "IMPLEMENTATION"},
	{IMPORT, "IMPORT"},
	{IN, "IN"},
	{LOOP, "LOOP"},
	{MOD, "MOD"},
	{MODULE, "MODULE"},
	{NOT, "NOT"},
	{OF, "OF"},
	{OR, "OR"},
	{POINTER, "POINTER"},
	{PROCEDURE, "PROCEDURE"},
	{QUALIFIED, "QUALIFIED"},
	{RECORD, "RECORD"},
	{REPEAT, "REPEAT"},
	{RETURN, "RETURN"},
	{SET, "SET"},
	{THEN, "THEN"},
	{TO, "TO"},
	{TYPE, "TYPE"},
	{UNTIL, "UNTIL"},
	{VAR, "VAR"},
	{WHILE, "WHILE"},
	{WITH, "WITH"},
	{0, ""}
};

struct tokenname tkinternal[] = {	/* internal keywords	*/
	{PROGRAM, ""},
	{SUBRANGE, ""},
	{ENUMERATION, ""},
	{ERRONEOUS, ""},
	{PROCVAR, ""},
	{INTORCARD, ""},
	{0, "0"}
};

struct tokenname tkstandard[] =	{	/* standard identifiers */
	{CHAR, ""},
	{BOOLEAN, ""},
	{LONGINT, ""},
	{CARDINAL, ""},
	{LONGREAL, ""},
	{WORD, ""},
	{ADDRESS, ""},
	{0, ""}
};

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
		if (!p) fatal("Out of Memory");
		p->id_reserved = resv->tn_symbol;
		resv++;
	}
}
