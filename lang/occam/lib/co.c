/* $Id$ */
/*	co.c - Routines to handle coroutines */
#include "ocm_proc.h"

static void search(), RESUMERR();

void resume(id) identification id;
/* Stops the current process, by saving its stack, and searches for the
 * process with identification 'id' in the group the running process
 * belongs to. If 'id' cannot be found then repeat these actions with
 * the running process' parent. If 'id' is found it is activated. It
 * is a fatal error if 'id' cannot be found.
 */
{
	if (group!=nil) {
		register wordsize size;

		size=top_size(group->s_brk);
		(*group->active)->stack=alloc((unsigned) size);

		if (top_save(size, (*group->active)->stack))
			search(id);
		else {
			free((*group->active)->stack);
			load_betweens();
		}
	} else
		RESUMERR();
}

static void search(id) identification id;
/* Searches for the process with identification 'id'.
 * If the process is found it is activated and its process tree is
 * traversed to find the running process.
 */
{
	register struct process **aproc, *proc;

	for(;;) {
		aproc= &group->first;

		while (*aproc!=nil && (*aproc)->id!=id)
			aproc= &(*aproc)->next;

		if (*aproc!=nil) break;

		save_between(group);

		if ((group=group->up)==nil)
			RESUMERR();
	}
	group->active=aproc;
	proc= *aproc;
	highest_group=group;

	while (proc->down!=nil) {
		group=proc->down;
		proc= *group->active;
	}
	top_load(proc->stack);
}

static void delete_group(group) struct procgroup *group;
/* Removes the whole group and sub-groups recursively from the running
 * process.
 */
{
	register struct process *proc, *next;

	proc=group->first;

	while (proc!=nil) {
		if (proc->down!=nil)
			delete_group(proc->down);
		else
			free(proc->stack);
		next=proc->next;
		free( (void *) proc);
		proc=next;
	}
	delete_between(group);
	free( (void *) group);
}

void coend()
{
	register struct process *proc, *next;
	register struct procgroup *junk;

	proc=group->first;

	while (proc!=nil) {
		if (proc!= *group->active) {
			if (proc->down!=nil)
				delete_group(proc->down);
			else
				free(proc->stack);
		}
		next=proc->next;
		free( (void *) proc);
		proc=next;
	}
	delete_between(group);
	junk=group;
	group=group->up;
	free( (void *) junk);

	if (group!=nil)
		(*group->active)->down=nil;
}

static void RESUMERR()
{
	write(2, "RESUMERR\n", 9);
	abort();
}
