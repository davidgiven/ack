/* $Id$ */
/*	parco.c	- Common routines for simulating parallelism or coroutines on
 *		  machines with downward growing stacks
 */
#include "ocm_proc.h"

struct procgroup *group=nil, *highest_group;

int deadlock=0;

void pc_begin(s_brk, id)
	register void *s_brk;
	identification id;
/* Sets up a group of processes and puts the current process in it */
{
	register struct procgroup *pg;
	register struct process *p;

	pg= (struct procgroup *) alloc(sizeof *pg);
	p= (struct process *) alloc(sizeof *p);

	pg->s_brk= s_brk==nil ? (void *) (&id +1) : s_brk;
	pg->up=group;
	pg->first=p;
	pg->active= &pg->first;

	p->next=nil;
	p->down=nil;
	p->id=id;

	if (group!=nil)
		(*group->active)->down=pg;

	group=pg;
	init_between(group);
}

int pc_fork(id) identification id;
/* Makes a copy of the stack top of the calling function and creates an
 * entry for it in the current process group.  Pc_fork() returns 1 in the
 * current process, 0 in the copied process. The current process runs first.
 */
{
	register struct process *newp;
	register wordsize size;

	newp= (struct process *) alloc(sizeof *newp);

	newp->down=nil;
	newp->id=id;

	newp->next= *group->active;
	*group->active= newp;
	group->active= &newp->next;

	size=top_size(group->s_brk);
	newp->stack=alloc((unsigned) size);

	if (top_save(size, newp->stack))
		return 1;
	else {
		free(newp->stack);
		load_betweens();
		return 0;
	}
}

void init_between(group) register struct procgroup *group;
/* Allocates memory to hold the stack space between s_brk and up->s_brk. */
{
	register wordsize size;

	if (group->up==nil
	    || (size= (wordsize) group->up->s_brk - (wordsize) group->s_brk)==0)
		group->between=nil;
	else
		group->between=alloc((unsigned) size);
}

void block_move();

void save_between(group) register struct procgroup *group;
/* Saves the stack space between  s_brk and up->s_brk. */
{
	register wordsize size;

	if (group->between!=nil) {
	    	size= (wordsize) group->up->s_brk - (wordsize) group->s_brk;
		block_move(size, group->s_brk, group->between);
	}
}

void load_betweens()
/* All stack pieces between s_brk and up->s_brk from the current group
 * upto the 'highest_group' are loaded onto the stack at the right
 * place (i.e. s_brk).
 */
{
	register struct procgroup *gr=group, *up;
	register wordsize size;

	while (gr!=highest_group) {
		up=gr->up;
		if (gr->between!=nil) {
			size= (wordsize) up->s_brk - (wordsize) gr->s_brk;

			block_move(size, gr->between, gr->s_brk);
		}
		gr=up;
	}
}

void delete_between(group) register struct procgroup *group;
/* Deallocates the stack space between s_brk and up->s_brk. */
{
	if (group->between!=nil)
		free(group->between);
}

void *malloc();

void *alloc(size) unsigned size;
{
	register void *mem;

	if ((mem=malloc(size))==nil) {
		write(2, "Heap error\n", 14);
		abort();
	}
	return mem;
}
