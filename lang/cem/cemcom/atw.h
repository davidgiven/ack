/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* Align To Word boundary Definition	*/

extern arith word_size;

#define	ATW(arg)	((((arg) + word_size - 1) / word_size) * word_size)
