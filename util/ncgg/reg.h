/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#define _REGINFO_
typedef struct reginfo {
	char *ri_name;
	char *ri_repr;
	int   ri_size;
	int   ri_class;
	int   ri_rregvar;
	int   ri_memb[2];
} reginfo;

extern struct reginfo l_regs[];
