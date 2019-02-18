/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef DECSPECS_H_
#define DECSPECS_H_

/* $Id$ */
/* DECLARATION SPECIFIER DEFINITION */

struct decspecs	{
	struct decspecs *next;
	struct type *ds_type;	/* single type */
	int ds_notypegiven;	/* set if type not given explicitly */
	int ds_typedef;		/* 1 if type was a user typedef */
	int ds_sc_given;	/* 1 if the st. class is explicitly given */
	int ds_sc;		/* storage class, given or implied */
	int ds_size;		/* LONG, SHORT or 0 */
	int ds_unsigned;	/* SIGNED, UNSIGNED or 0 */
	int ds_typequal;	/* type qualifiers - see type.str */
};

extern struct type *qualifier_type();
extern struct decspecs null_decspecs;

struct type;

void do_decspecs(register struct decspecs *ds);
struct type *qualifier_type(register struct type *tp, int typequal);

#endif
