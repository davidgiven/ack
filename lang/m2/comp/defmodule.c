/* D E F I N I T I O N   M O D U L E S */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

#include	"debug.h"

#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"idf.h"
#include	"input.h"
#include	"scope.h"
#include	"def.h"
#include	"LLlex.h"
#include	"f_info.h"
#include	"main.h"

#ifdef DEBUG
long	sys_filesize();
#endif

GetFile(name)
	char *name;
{
	/*	Try to find a file with basename "name" and extension ".def",
		in the directories mentioned in "DEFPATH".
	*/
	char buf[256];
	char *strcpy(), *strcat();

	strcpy(buf, name);
	buf[10] = '\0';			/* maximum length */
	strcat(buf, ".def");
	if (! InsertFile(buf, DEFPATH, &(FileName))) {
		fatal("Could'nt find a DEFINITION MODULE for \"%s\"", name);
	}
	LineNumber = 1;
	DO_DEBUG(1, debug("File %s : %ld characters", FileName, sys_filesize(FileName)));
}

struct def *
GetDefinitionModule(id)
	struct idf *id;
{
	/*	Return a pointer to the "def" structure of the definition
		module indicated by "id".
		We may have to read the definition module itself.
	*/
	struct def *df;
	static int level;

	level++;
	df = lookup(id, GlobalScope);
	if (!df) {
		/* Read definition module. Make an exception for SYSTEM.
		*/
		if (!strcmp(id->id_text, "SYSTEM")) {
			do_SYSTEM();
		}
		else {
			GetFile(id->id_text);
			DefModule();
			if (level == 1) {
				/* The module is directly imported by the
				   currently defined module, so we have to
				   remember its name because we have to call
				   its initialization routine
				*/
				AddModule(id);
			}
		}
		df = lookup(id, GlobalScope);
	}
	assert(df != 0 && df->df_kind == D_MODULE);
	level--;
	return df;
}
