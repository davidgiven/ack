/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef lint
#ifdef NASSERT

#define assert(ex)

#else /* NASSERT */

#define assert(ex) \
{if (!(ex)) fatal("Assertion failed: file %s, line %d", __FILE__, __LINE__);}

#endif /* NASSERT */
#else /* lint */

#define assert(ex)

#endif /* lint */
