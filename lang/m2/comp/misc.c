/* M I S C E L L A N E O U S    R O U T I N E S */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	"f_info.h"
#include	"misc.h"
#include	"LLlex.h"
#include	"idf.h"

match_id(id1, id2)
	struct idf *id1, *id2;
{
	/*	Check that identifiers id1 and id2 are equal. If they
		are not, check that we did'nt generate them in the
		first place, and if not, give an error message
	*/
	if (id1 != id2 && !is_anon_idf(id1) && !is_anon_idf(id2)) {
		error("Name \"%s\" does not match block name \"%s\"",
		      id1->id_text,
		      id2->id_text
		);
	}
}

struct id_list *h_id_list;	/* Header of free list of id_list structures */

/*	FreeIdList: take a list of id_list structures and put them
	on the free list of id_list structures
*/
FreeIdList(p)
	struct id_list *p;
{
	register struct id_list *q;

	while (q = p) {
		p = p->next;
		free_id_list(q);
	}
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
