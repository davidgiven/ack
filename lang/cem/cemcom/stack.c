/* $Header$ */
/*	S T A C K / U N S T A C K  R O U T I N E S	*/

#include	<system.h>
#include	<em.h>
#include	<em_reg.h>

#include	"debug.h"
#include	"use_tmp.h"
#include	"botch_free.h"

#include	"alloc.h"
#include	"Lpars.h"
#include	"arith.h"
#include	"stack.h"
#include	"type.h"
#include	"idf.h"
#include	"def.h"
#include	"struct.h"
#include	"storage.h"
#include	"level.h"
#include	"mes.h"

/* #include	<em_reg.h> */

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
	struct stack_level *stl = new_stack_level();
	
	clear((char *)stl, sizeof(struct stack_level));
	local_level->sl_next = stl;
	stl->sl_previous = local_level;
	stl->sl_level = ++level;
	stl->sl_local_offset =
	stl->sl_max_block = local_level->sl_local_offset;
	local_level = stl;
}

stack_idf(idf, stl)
	struct idf *idf;
	struct stack_level *stl;
{
	/*	The identifier idf is inserted in the stack on level stl.
	*/
	register struct stack_entry *se = new_stack_entry();

	clear((char *)se, sizeof(struct stack_entry));
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
	if (lvl == level)
		return local_level;
	else	{
		register struct stack_level *stl = &UniversalLevel;
		
		while (stl->sl_level != lvl)
			stl = stl->sl_next;
		return stl;
	}
	/*NOTREACHED*/
}

unstack_level()
{
	/*	The top level of the identifier stack is removed.
	*/
	struct stack_level *lastlvl;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("before unstackidfs", 0);
#endif	DEBUG
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
			else
			if (level == L_LOCAL || level == L_FORMAL1)	{
				if (	def->df_register != REG_NONE &&
					def->df_sc != STATIC &&
					options['n'] == 0
				)	{
					int reg;
					
					switch (def->df_type->tp_fund)	{
					
					case POINTER:
						reg = reg_pointer;
						break;
#ifndef NOFLOAT
					case FLOAT:
					case DOUBLE:
						reg = reg_float;
						break;
#endif NOFLOAT
					default:
						reg = reg_any;
						break;
					}
					C_ms_reg(def->df_address,
						def->df_type->tp_size,
						reg, def->df_register
					);
				}
			}
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
	if (	level > L_LOCAL
	&&	lastlvl->sl_max_block < local_level->sl_max_block
	)	{
		local_level->sl_max_block = lastlvl->sl_max_block;
	}
	free_stack_level(lastlvl);
	local_level->sl_next = (struct stack_level *) 0;
	level = local_level->sl_level;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("after unstackidfs", 0);
#endif	DEBUG
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
#endif DEBUG
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
			if (options['R'])
				warning("static function %s never defined, %s",
					idf->id_text,
					"changed to extern"
				);
			def->df_sc = EXTERN;
		}
		
		if (
			def->df_alloc == ALLOC_SEEN &&
			!def->df_initialized
		)	{
			/* space must be allocated */
			bss(idf);
			namelist(idf->id_text);		/* may be common */
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
