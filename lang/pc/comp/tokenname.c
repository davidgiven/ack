/* T O K E N   D E F I N I T I O N S */

#include	"Lpars.h"
#include	"idf.h"
#include	"tokenname.h"

/*	To centralize the declaration of %tokens, their presence in this
	file is taken as their declaration. The Makefile will produce
	a grammar file (tokenfile.g) from this file. This scheme ensures
	that all tokens have a printable name.
	Also, the "symbol2str.c" file is produced from this file.
*/

#ifdef ____
struct tokenname tkspec[] =	{	/* the names of the special tokens */
	{IDENT, "identifier"},
	{STRING, "string"},
	{INTEGER, "integer"},
	{REAL, "real"},
	{0, ""}
};

struct tokenname tkcomp[] =	{	/* names of the composite tokens */
	{LESSEQUAL, "<="},
	{GREATEREQUAL, ">="},
	{NOTEQUAL, "<>"},
	{UPTO, ".."},
	{BECOMES, ":="},
	{0, ""}
};
#endif /* ____ */

struct tokenname tkidf[] =	{	/* names of the identifier tokens */
	{AND, "and"},
	{ARRAY, "array"},
	{BEGIN, "begin"},
	{CASE, "case"},
	{CONST, "const"},
	{DIV, "div"},
	{DO, "do"},
	{DOWNTO, "downto"},
	{ELSE, "else"},
	{END, "end"},
	{FILE, "file"},
	{FOR, "for"},
	{FUNCTION, "function"},
	{GOTO, "goto"},
	{IF, "if"},
	{IN, "in"},
	{LABEL, "label"},
	{MOD, "mod"},
	{NIL, "nil"},
	{NOT, "not"},
	{OF, "of"},
	{OR, "or"},
	{PACKED, "packed"},
	{PROCEDURE, "procedure"},
	{PROGRAM, "program"},
	{RECORD, "record"},
	{REPEAT, "repeat"},
	{SET, "set"},
	{THEN, "then"},
	{TO, "to"},
	{TYPE, "type"},
	{UNTIL, "until"},
	{VAR, "var"},
	{WHILE, "while"},
	{WITH, "with"},
	{0, ""}
};

struct tokenname tkstandard[] =	{	/* standard identifiers */
	/* These are the only standard identifiers entered here, because
	 * they can get a variable number of arguments, and there are
	 * special syntaxrules in the grammar for them
	 */
	{READ, "read"},
	{READLN, "readln"},
	{WRITE, "write"},
	{WRITELN, "writeln"},
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

	while( resv->tn_symbol )	{
		p = str2idf(resv->tn_name, 0);
		if( !p ) fatal("out of Memory");
		p->id_reserved = resv->tn_symbol;
		resv++;
	}
}
