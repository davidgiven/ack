/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef NDEBUG
#define assert(x) if (!(x)) badassertion("x",__FILE__,__LINE__)
#else
#define assert(x)	/* nothing */
#endif
