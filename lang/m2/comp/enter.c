/* H I G H   L E V E L   S Y M B O L   E N T R Y   A N D   L O O K U P */

static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	<em_label.h>
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"scope.h"

extern struct idf *str2idf();
extern struct def *define();

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
}
