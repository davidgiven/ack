/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*  LEVEL DEFINITIONS */

/*	The level of the top-most stack_level is kept in a global variable
	with the obvious name 'level'. Although this variable is consulted
	by a variety of routines, it turns out that its actual value is of
	importance in only a very few files. Therefore the names of the
	values are put in a separate include-file.
	The L_PROTO level is a dummy level, which only occurs within
	prototype declarations. When the declaration is really declared
	the level is turned into L_FORMAL2.
*/

#define	L_PROTO		(-1)		/* prototype declaration */
#define	L_UNIVERSAL	0
#define	L_GLOBAL	1
#define	L_FORMAL1	2		/* formal declaration */
#define	L_FORMAL2	3		/* formal definition */
#define	L_LOCAL		4		/* and up */
