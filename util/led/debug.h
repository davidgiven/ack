/* $Header$ */

#ifdef NDEBUG

#define debug(s, a1, a2, a3, a4)	dummy()

#else

#define debug(s, a1, a2, a3, a4)	printf(s, a1, a2, a3, a4)

#endif
