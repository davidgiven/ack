/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ A U X . C
 */

#include "ego/share/types.h"
#include "il.h"
#include "ego/share/debug.h"
#include "ego/share/alloc.h"
#include "ego/share/global.h"
#include "il3_aux.h"

line_p last_line(line_p lines)
{
	/* Determine the last line of a list */

	line_p l;

	assert(lines != (line_p)0);
	for (l = lines; l->l_next != (line_p)0; l = l->l_next)
		;
	return l;
}

void app_list(line_p list, line_p l)
{
	/* Append the list after line l */

	line_p llast;

	assert(l != (line_p)0);
	assert(list != (line_p)0);
	llast = last_line(list);
	llast->l_next = l->l_next;
	if (l->l_next != (line_p)0)
	{
		PREV(l->l_next) = llast;
	}
	l->l_next = list;
	PREV(list) = l;
}

void rem_line(line_p l)
{
	/* Remove a line from the list */

	if (PREV(l) != (line_p)0)
	{
		PREV(l)->l_next = l->l_next;
	}
	if (l->l_next != (line_p)0)
	{
		PREV(l->l_next) = PREV(l);
	}
	oldline(l);
}
