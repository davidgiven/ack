/* M I S C E L L A N E O U S    R O U T I N E S */

#include	<alloc.h>
#include	<em.h>

#include	"LLlex.h"
#include	"f_info.h"
#include	"idf.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"

struct idf *
gen_anon_idf()
{
	/*	A new idf is created out of nowhere, to serve as an
		anonymous name.
	*/
	static int name_cnt;
	char *s = Malloc(strlen(FileName) + 50);
	char *sprint();

	sprint(s, "#%d in %s, line %u", ++name_cnt, FileName, LineNumber);
	s = Realloc(s, strlen(s)+1);
	return str2idf(s, 0);
}

not_declared(what, id, where)
	char *what, *where;
	register struct node *id;
{
	/*	The identifier "id" is not declared. If it is not generated,
		give an error message
	*/
	if( !is_anon_idf(id->nd_IDF) )	{
		node_error(id, "%s \"%s\" not declared%s",
			   		what, id->nd_IDF->id_text, where);
	}
}

char *
gen_proc_name(id, inp)
	register struct idf *id;
{
	/* generate pseudo and internal name for procedure or function */

	static int name_cnt;
	static char buf[256];
	char *sprint(), *Salloc();

	if( inp )	{
		sprint(buf, "_%d%s", ++name_cnt, id->id_text);
		C_inp(buf);
		return Salloc(buf, (unsigned) (strlen(buf) + 1));
	}
	else	{
		C_exp(id->id_text);
		return id->id_text;
	}

}
