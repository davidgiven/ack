/* $Header$ */

#ifndef lint
#ifdef NASSERT

#define assert(ex)

#else NASSERT

#define assert(ex) \
{if (!(ex)) fatal("Assertion failed: file %s, line %d", __FILE__, __LINE__);}

#endif NASSERT
#else lint

#define assert(ex)

#endif lint
