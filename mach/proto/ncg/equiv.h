/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#define MAXCREG 4

struct perm {
	struct perm *p_next;
	int p_rar[MAXCREG];
};

struct perm *tuples(rl_p *, int);
