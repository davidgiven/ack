/* $Header$ */

#ifndef NDEBUG
#define assert(x) if(!(x)) badassertion("x",__FILE__,__LINE__)
#else
#define assert(x)	/* nothing */
#endif
