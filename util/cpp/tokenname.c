/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* TOKEN NAME DEFINITIONS */

#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"

struct tokenname	{	/*	Used for defining the name of a
					token as identified by its symbol
				*/
	int tn_symbol;
	char *tn_name;
};

/*	To centralize the declaration of %tokens, their presence in this
	file is taken as their declaration. The Makefile will produce
	a grammar file (tokenfile.g) from this file.
	Moreover, rather than looking up a symbol in all these lists
	to find its printable name, a fast version of symbol2str() is
	generated from these tables.
	Consequenty some of these tables are not referenced explicitly
	in the C text any more.  To save space and to avoid lint confusion,
	these have been made pseudo-invisible by #ifdefs.
*/

#ifdef	____
struct tokenname tkspec[] =	{	/* the names of the special tokens */
	{IDENTIFIER, "identifier"},
	{STRING, "string"},
	{FILESPECIFIER, "filespecifier"},
	{INTEGER, "integer"},
	{0, ""}
};

struct tokenname tkcomp[] =	{	/* names of the composite tokens */
	{NOTEQUAL, "!="},
	{AND, "&&"},
	{LEFT, "<<"},
	{LESSEQ, "<="},
	{EQUAL, "=="},
	{GREATEREQ, ">="},
	{RIGHT, ">>"},
	{OR, "||"},
	{0, ""}
};

struct tokenname tkfunny[] =	{	/* internal keywords */
	{ERRONEOUS, "erroneous"},
	{0, ""}
};
#endif	/* ____ */
