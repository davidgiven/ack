/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* C O P Y   P R O P A G A T I O N
 *
 * A U X I L I A R Y   R O U T I N E S
 */

#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include "ego/share/types.h"
#include "ud.h"
#include "ego/share/debug.h"
#include "ego/share/global.h"
#include "ego/share/alloc.h"
#include "ego/share/lset.h"
#include "ego/share/cset.h"
#include "ego/share/def.h"
#include "ego/share/locals.h"
#include "ego/share/utils.h"
#include "ud_defs.h"

void repl_line(line_p old, line_p new, bblock_p b)
{
	/* Replace 'old' by 'new' */

	if (PREV(old) == (line_p)0)
	{
		b->b_start = new;
	}
	else
	{
		PREV(old)->l_next = new;
	}
	PREV(new) = PREV(old);
	if ((new->l_next = old->l_next) != (line_p)0)
	{
		PREV(new->l_next) = new;
	}
	oldline(old);
}

bool same_var(line_p use, line_p def)
{
	/* 'use' is an instruction that uses a variable
	 * for which we maintain ud-info (e.g. a LOL).
	 * See if 'def' references the same variable.
	 */

	if (TYPE(use) == OPOBJECT)
	{
		return TYPE(def) == OPOBJECT && OBJ(use) == OBJ(def);
	}
	else
	{
		return TYPE(def) != OPOBJECT && off_set(use) == off_set(def);
	}
}
