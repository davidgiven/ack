/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* Align To Word boundary Definition	*/

extern int word_align;	/* align of a word	*/

#define	ATW(arg)	((((arg) + word_align - 1) / word_align) * word_align)
