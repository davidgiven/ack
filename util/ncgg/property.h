/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef struct propinfo {
	int	pr_size;
	short	pr_regset[SZOFSET(MAXREGS)];
} propinfo;

extern struct propinfo l_props[];
