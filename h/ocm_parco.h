/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	parco.h - Define names for simulation routines
 *
 *      This file is to be included by users of the higher-level routines
 *
 */

void pc_begin(), resumenext(), parend(), resume(), coend();
int pc_fork();

#define nullid	((int *) 0 - (int *) 0)
	/* I.e. a 0 of type "pointer difference" */

#define parbegin(sbrk)		pc_begin(sbrk, nullid)
#define parfork()		pc_fork(nullid)
#define cobegin(sbrk, id)	pc_begin(sbrk, id)
#define cofork(id)		pc_fork(id)

extern int deadlock;
