/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E F I N I T I O N   M O D U L E S */

/* $Id$ */

#include "debug.h"
#include "parameters.h"

#include <alloc.h>
#include <assert.h>
#include <em_arith.h>
#include <em_label.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LLlex.h"
#include "Lpars.h"
#include "def.h"
#include "f_info.h"
#include "idf.h"
#include "input.h"
#include "error.h"
#include "main.h"
#include "misc.h"
#include "node.h"
#include "lookup.h"
#include "main.h"
#include "scope.h"
#include "type.h"

#ifdef DEBUG
size_t sys_filesize();
#endif

extern void DefModule(void); /* Lpars */

struct idf* DefId;

char* getwdir(register char *fn)
{
	register char* p;

	while ((p = strrchr(fn, '/')) && *(p + 1) == '\0')
	{
		/* remove trailing /'s */
		*p = '\0';
	}

	if (p)
	{
		*p = '\0';
		fn = Salloc(fn, (unsigned)(p - &fn[0] + 1));
		*p = '/';
		return fn;
	}
	return "";
}

static int GetFile(char *name)
{
	/*	Try to find a file with basename "name" and extension ".def",
		in the directories mentioned in "DEFPATH".
	*/
	size_t len;
	int found;
	char *buf;

	len = strlen(name);
	buf = Malloc(len + 5);
	memcpy(buf, name, len);
	memcpy(buf + len, ".def", 5);
	DEFPATH[0] = WorkingDir;
	found = InsertFile(buf, DEFPATH, &(FileName));
	free(buf);
	if (!found)
	{
		error("could not find a DEFINITION MODULE for \"%s\"", name);
		return 0;
	}
	WorkingDir = getwdir(FileName);
	LineNumber = 1;
	DO_DEBUG(options['F'], debug("File %s : %ld characters", FileName, sys_filesize(FileName)));
	return 1;
}

struct def* GetDefinitionModule(register struct idf* id, int incr)
{
	/*	Return a pointer to the "def" structure of the definition
		module indicated by "id".
		We may have to read the definition module itself.
		Also increment level by "incr".
	*/
	register struct def* df;
	static int level;
	struct scopelist* vis;
	char* fn = FileName;
	int ln = LineNumber;
	struct scope* newsc;

	level += incr;
	df = lookup(id, GlobalScope, D_IMPORTED, 0);
	if (!df)
	{
		/* Read definition module. Make an exception for SYSTEM.
		*/
		extern int ForeignFlag;

		ForeignFlag = 0;
		DefId = id;
		open_scope(CLOSEDSCOPE);
		newsc = CurrentScope;
		vis = CurrVis;
		newsc->sc_defmodule = incr;
		if (!strcmp(id->id_text, "SYSTEM"))
		{
			do_SYSTEM();
			df = lookup(id, GlobalScope, D_IMPORTED, 0);
		}
		else
		{
			if (!is_anon_idf(id) && GetFile(id->id_text))
			{

				char* f = FileName;
				DefModule();
				df = lookup(id, GlobalScope, D_IMPORTED, 0);
				if (level == 1 && (df && !(df->df_flags & D_FOREIGN)))
				{
					/* The module is directly imported by
					   the currently defined module, and
					   is not foreign, so we have to
					   remember its name because we have 
					   to call its initialization routine
					*/
					static struct node* nd_end;
					register struct node* n;
					extern struct node* Modules;

					n = dot2leaf(Def);
					n->nd_def = newsc->sc_definedby;
					if (nd_end)
						nd_end->nd_NEXT = n;
					else
						Modules = n;
					nd_end = n;
				}
				free(f);
			}
			else
			{
				df = lookup(id, GlobalScope, D_IMPORTED, 0);
				newsc->sc_name = id->id_text;
			}
		}
		close_scope(SC_CHKFORW);
		if (!df)
		{
			df = MkDef(id, GlobalScope, D_ERROR);
			df->mod_vis = vis;
			newsc->sc_definedby = df;
		}
	}
	else if (df->df_flags & D_BUSY)
	{
		error("definition module \"%s\" depends on itself",
		    id->id_text);
	}
	else if (df == Defined && level == 1)
	{
		error("cannot import from current module \"%s\"", id->id_text);
		df->df_kind = D_ERROR;
	}
	FileName = fn;
	LineNumber = ln;
	assert(df);
	level -= incr;
	return df;
}
