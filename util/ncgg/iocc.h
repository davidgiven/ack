/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef struct iocc {
	short in_set[SETSIZE];
	int   in_index;
} iocc_t,*iocc_p;
