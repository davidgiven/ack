#include	<alloc.h>
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"

static char *RcsId = "$Header$";

extern char *symbol2str();
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

struct idf *
gen_anon_idf()
{
	/*	A new idf is created out of nowhere, to serve as an
		anonymous name.
	*/
	static int name_cnt;
	char buff[100];
	char *sprintf();

	sprintf(buff, "#%d in %s, line %u",
			++name_cnt, FileName, LineNumber);
	return str2idf(buff, 1);
}

int
is_anon_idf(idf)
	struct idf *idf;
{
	return idf->id_text[0] == '#';
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
		dot.TOK_STR = Salloc("", 1);
		break;
	case INTEGER:
/*		dot.TOK_ITP = INT; */
		dot.TOK_INT = 1;
		break;
	case REAL:
		dot.TOK_REL = Salloc("0.0", 4);
		break;
	}
}
