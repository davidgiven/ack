/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	process.h - Define administration types and functions
 *
 *      This file is to be included by implementors of the higher
 *      level routines
 *
 */
#include "ocm_parco.h"

#ifndef ptrdiff	/* This type must be able to hold a pointer difference */
#if EM_WSIZE <EM_PSIZE
#define ptrdiff long
#else
#define ptrdiff int	/* Define as long int if necessary */
#endif
#endif

#define nil	0
void *alloc(), free();

typedef ptrdiff wordsize, identification;

wordsize top_size();
int top_save();
void top_load();		/* Primitives */

struct procgroup;

struct process {
	struct process *next;	/* Next process in the same group */
	struct procgroup *down;	/* Process group running under this process */
	void *stack;		/* Pointer to the saved stack top */
	identification id;	/* Coroutine identification */
};

#define init_between	__i_b__	/* These names are hidden */
#define save_between	__s_b__
#define load_betweens	__l_b__
#define delete_between	__d_b__

void init_between(), save_between(), load_betweens(), delete_between();

struct procgroup {
	struct process **active;/* Active process within this group */
	struct procgroup *up;	/* The group that this group belongs to */
	struct process *first;	/* List of processes belonging to this group */
	void *s_brk;		/* Point where the stack is split */
	void *between;		/* Stack space between s_brk and up->s_brk */
};

#define group		__grp__	/* Ignore this please */
#define highest_group	__hgrp__

extern struct procgroup *group;		/* Current running group */
extern struct procgroup *highest_group;	/* highest group that has been seen
					 * while searching for a process
					 */
