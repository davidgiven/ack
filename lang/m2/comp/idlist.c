static char *RcsId = "$Header$";

#include "idf.h"
#include "idlist.h"

struct id_list *h_id_list;	/* Header of free list */

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
