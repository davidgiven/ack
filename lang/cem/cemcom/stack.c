/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	S T A C K / U N S T A C K  R O U T I N E S	*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	"lint.h"
#include	"nofloat.h"
#include	<system.h>
#ifndef	LINT
#include	<em.h>
#else
#include	"l_em.h"
#endif	/* LINT */
#include	"debug.h"
#include	"botch_free.h"
#include	<alloc.h>
#include	"Lpars.h"
#include	"arith.h"
#include	"stack.h"
#include	"type.h"
#include	"idf.h"
#include	"def.h"
#include	"struct.h"
#include	"level.h"
#include	"mes.h"
#include	"noRoption.h"

extern char options[];

static struct stack_level UniversalLevel;
struct stack_level *local_level = &UniversalLevel;
/*	The main reason for having this secondary stacking
	mechanism besides the linked lists pointed to by the idf's
	is efficiency.
	To remove the idf's of a given level, one could scan the
	hash table and chase down the idf chains; with a hash
	table size of 100 this is feasible, but with a size of say
	100000 this becomes painful. Therefore all idf's are also
	kept in a stack of sets, one set for each level.
*/

int level;	/* Always equal to local_level->sl_level. */

stack_level()	{
	/*	A new level is added on top of the identifier stack.
	*/
	register struct stack_level *stl = new_stack_level();
	register struct stack_level *loclev = local_level;
	
	loclev->sl_next = stl;
	stl->sl_previous = loclev;
	stl->sl_level = ++level;
	stl->sl_max_block = loclev->sl_max_block;
	local_level = stl;
#ifdef	LINT
	lint_start_local();
#endif	/* LINT */
}

stack_idf(idf, stl)
	struct idf *idf;
	register struct stack_level *stl;
{
	/*	The identifier idf is inserted in the stack on level stl.
	*/
	register struct stack_entry *se = new_stack_entry();

	/* link it into the stack level */
	se->next = stl->sl_entry;
	se->se_idf = idf;
	stl->sl_entry = se;
}

struct stack_level *
stack_level_of(lvl)
{
	/*	The stack_level corresponding to level lvl is returned.
		The stack should probably be an array, to be extended with
		realloc where needed.
	*/
	register struct stack_level *stl;

	if (lvl == level)
		return local_level;
	stl = &UniversalLevel;
		
	while (stl->sl_level != lvl)
		stl = stl->sl_next;
	return stl;
}

unstack_level()
{
	/*	The top level of the identifier stack is removed.
	*/
	struct stack_level *lastlvl;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("before unstackidfs", 0);
#endif	/* DEBUG */

#ifdef	LINT
	lint_end_local(local_level);
#endif	/* LINT */

	/*	The implementation below is more careful than strictly
		necessary. Optimists may optimize it afterwards.
	*/
	while (local_level->sl_entry)	{
		register struct stack_entry *se = local_level->sl_entry;
		register struct idf *idf = se->se_idf;
		register struct def *def;
		register struct sdef *sdef;
		register struct tag *tag;

		/* unlink it from the local stack level */
		local_level->sl_entry = se->next;
		free_stack_entry(se);

		while ((def = idf->id_def) && def->df_level >= level)	{
			/* unlink it from the def list under the idf block */
			if (def->df_sc == LABEL)
				unstack_label(idf);
			else if (def->df_sc == REGISTER || def->df_sc == AUTO)
				FreeLocal(def->df_address);
			idf->id_def = def->next;
			free_def(def);
			update_ahead(idf);
		}
		while (	(sdef = idf->id_sdef)
		&&	sdef->sd_level >= level
		)	{
			/* unlink it from the sdef list under the idf block */
			idf->id_sdef = sdef->next;
			free_sdef(sdef);
		}
		while (	(tag = idf->id_struct)
		&&	tag->tg_level >= level
		)	{
			/* unlink it from the struct list under the idf block */
			idf->id_struct = tag->next;
			free_tag(tag);
		}
		while ((tag = idf->id_enum) && tag->tg_level >= level)	{
			/* unlink it from the enum list under the idf block */
			idf->id_enum = tag->next;
			free_tag(tag);
		}
	}
	/*	Unlink the local stack level from the stack.
	*/
	lastlvl = local_level;
	local_level = local_level->sl_previous;
	if (level >= L_LOCAL)	{
		local_level->sl_max_block = lastlvl->sl_max_block;
	}
	free_stack_level(lastlvl);
	local_level->sl_next = (struct stack_level *) 0;
	level = local_level->sl_level;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("after unstackidfs", 0);
#endif	/* DEBUG */
}

unstack_world()
{
	/*	The global level of identifiers is scanned, and final
		decisions are taken about such issues as
		extern/static/global and un/initialized.
		Effects on the code generator: initialised variables
		have already been encoded while the uninitialised ones
		are not and have to be encoded at this moment.
	*/
	register struct stack_entry *se = local_level->sl_entry;

#ifdef	LINT
	lint_end_global(local_level);
#endif	/* LINT */

	open_name_list();

	while (se)	{
		register struct idf *idf = se->se_idf;
		register struct def *def = idf->id_def;
		
		if (!def)	{
			/* global selectors, etc. */
			se = se->next;
			continue;
		}
		
#ifdef DEBUG
		if (options['a']) {
			char *symbol2str();

			print("\"%s\", %s, %s, %s, %s\n",
				idf->id_text,
				(def->df_alloc == 0) ? "no alloc" :
				(def->df_alloc == ALLOC_SEEN) ? "alloc seen" :
				(def->df_alloc == ALLOC_DONE) ? "alloc done" :
				"illegal alloc info",
				symbol2str(def->df_sc),
				def->df_initialized ? "init" : "no init",
				def->df_used ? "used" : "not used");
		}
#endif /* DEBUG */
		/*
		/_* find final storage class *_/
		if (def->df_sc == GLOBAL || def->df_sc == IMPLICIT)
			/_* even now we still don't know *_/
			def->df_sc = EXTERN;
		*/
		
		if (	def->df_sc == STATIC
			&& def->df_type->tp_fund == FUNCTION
			&& !def->df_initialized
		)	{
			/* orphaned static function */
#ifndef NOROPTION
			if (options['R'])
				warning("static function %s never defined, %s",
					idf->id_text,
					"changed to extern"
				);
#endif
			def->df_sc = EXTERN;
		}
		
		if (
			def->df_alloc == ALLOC_SEEN &&
			!def->df_initialized
		)	{
			/* space must be allocated */
			bss(idf);
			if (def->df_sc != STATIC)
				namelist(idf->id_text);	/* may be common */
			def->df_alloc = ALLOC_DONE;	/* see Note below */
		}
		se = se->next;
	}
	/*	Note:
		df_alloc must be set to ALLOC_DONE because the idf entry
		may occur several times in the list.
		The reason for this is that the same name may be used
		for different purposes on the same level, e.g.
			struct s {int s;} s;
		is a legal definition and contains 3 defining occurrences
		of s.
		Each definition has been entered into the identifier stack.
		Although only one of them concerns a variable, we meet the
		s 3 times when scanning the identifier stack.
	*/
}

/*	A list of potential common names is kept, to be fed to
	an understanding loader.  The list is written to a file
	the name of which is nmlist.  If nmlist == NULL, no name
	list is generated.
*/
extern char *nmlist;	/* BAH! -- main.c	*/
static File *nfp = 0;

open_name_list()
{
	if (nmlist && sys_open(nmlist, OP_WRITE, &nfp) == 0)
		fatal("cannot create namelist %s", nmlist);
}

namelist(nm)
	char *nm;
{
	if (nmlist)	{
		sys_write(nfp, nm, strlen(nm));
		sys_write(nfp, "\n", 1);
	}
}
