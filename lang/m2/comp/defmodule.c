/* D E F I N I T I O N   M O D U L E S */

static char *RcsId = "$Header$";

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

#include	"debug.h"

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

	(void) strcpy(buf, name);
	if (strlen(buf) > 10) {
		(void) strcpy(&buf[10], ".def");
	}
	else	(void) strcat(buf, ".def");
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
		}
		df = lookup(id, GlobalScope);
	}
	assert(df != 0 && df->df_kind == D_MODULE);
	return df;
}

AtEoIF()
{
	/*	Make the unstacking of input streams noticable by the
	   	lexical analyzer
	*/
	return 1;
}
