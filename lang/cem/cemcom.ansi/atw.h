/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* Align To Word boundary Definition	*/

#include "sizes.h"

#if word_size==2 || word_size==4
#define	ATW(arg)	(((arg) + (word_size - 1)) & ~ (word_size - 1))
#else
#define	ATW(arg)	((((arg) + ((int)word_size - 1)) / word_size) * word_size)
#endif
