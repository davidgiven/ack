/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * assert.h $Header$
 * an assertion macro
 */

#ifndef NDEBUG
#define assert(x) if(!(x)) badassertion("x",__FILE__,__LINE__)
#else
#define assert(x)	/* nothing */
#endif
