/* D E F I N I T I O N   M O D U L E S */

#include	"debug.h"

#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"input.h"
#include	"scope.h"
#include	"def.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"f_info.h"
#include	"main.h"
#include	"node.h"
#include	"type.h"

#ifdef DEBUG
long	sys_filesize();
#endif

STATIC
GetFile(name)
	char *name;
{
	/*	Try to find a file with basename "name" and extension ".def",
		in the directories mentioned in "DEFPATH".
	*/
	char buf[15];
	char *strcpy(), *strcat();

	strncpy(buf, name, 10);
	buf[10] = '\0';			/* maximum length */
	strcat(buf, ".def");
	if (! InsertFile(buf, DEFPATH, &(FileName))) {
		error("could'nt find a DEFINITION MODULE for \"%s\"", name);
		return 0;
	}
	LineNumber = 1;
	DO_DEBUG(options['F'], debug("File %s : %ld characters", FileName, sys_filesize(FileName)));
	return 1;
}

struct def *
GetDefinitionModule(id, incr)
	register struct idf *id;
{
	/*	Return a pointer to the "def" structure of the definition
		module indicated by "id".
		We may have to read the definition module itself.
		Also increment level by "incr".
	*/
	struct def *df;
	static int level;
	struct scopelist *vis;

	level += incr;
	df = lookup(id, GlobalScope);
	if (!df) {
		/* Read definition module. Make an exception for SYSTEM.
		*/
		if (!strcmp(id->id_text, "SYSTEM")) {
			do_SYSTEM();
		}
		else {
			open_scope(CLOSEDSCOPE);
			if (GetFile(id->id_text)) {
				DefModule();
				if (level == 1) {
					/* The module is directly imported by
					   the currently defined module, so we
					   have to remember its name because
					   we have to call its initialization
					   routine
					*/
					static struct node *nd_end;
					register struct node *n;
					extern struct node *Modules;

					n = MkLeaf(Name, &dot);
					n->nd_IDF = id;
					n->nd_symb = IDENT;
					if (nd_end) nd_end->next = n;
					else Modules = n;
					nd_end = n;
				}
			}
			vis = CurrVis;
			close_scope(SC_CHKFORW);
		}
		df = lookup(id, GlobalScope);
		if (! df) {
			df = MkDef(id, GlobalScope, D_ERROR);
			df->df_type = error_type;
			df->mod_vis = CurrVis;
			return df;
		}
	}
	assert(df);
	level -= incr;
	return df;
}
