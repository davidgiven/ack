/* H I G H   L E V E L   S Y M B O L   E N T R Y   A N D   L O O K U P */

static char *RcsId = "$Header$";

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"
#include	"misc.h"

extern struct idf *str2idf();
extern struct def *define();

struct def *
Enter(name, kind, type, pnam)
	char *name;
	struct type *type;
{
	struct idf *id;
	struct def *df;

	id = str2idf(name, 0);
	if (!id) fatal("Out of core");
	df = define(id, CurrentScope, kind);
	df->df_type = type;
	if (kind == D_STDPROC || kind == D_STDFUNC) {
		df->df_value.df_stdname = pnam;
	}
	return df;
}

EnterIdList(idlist, kind, flags, type)
	register struct id_list *idlist;
	struct type *type;
{
	register struct def *df;
	struct def *last = 0;
	int assval = 0;

	while (idlist) {
		df = define(idlist->id_ptr, CurrentScope, kind);
		df->df_type = type;
		df->df_flags = flags;
		if (kind == D_ENUM) {
			df->df_value.df_enum.en_val = assval++;
			if (last) last->df_value.df_enum.en_next = df;
			last = df;
		}
		idlist = idlist->next;
	}
	if (last) last->df_value.df_enum.en_next = 0;
}
