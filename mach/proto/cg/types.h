/* $Header$ */

#ifndef EM_WSIZE
EM_WSIZE should be defined at this point
#endif
#ifndef EM_PSIZE
EM_PSIZE should be defined at this point
#endif
#if EM_WSIZE>4 || EM_PSIZE>4
Implementation will not be correct unless a long integer
has more then 4 bytes of precision.
#endif

typedef char byte;
typedef char * string;

#if EM_WSIZE>2 || EM_PSIZE>2
#define full            long
#else
#define full            int
#endif

#if EM_WSIZE>2
#define word long
#ifndef WRD_FMT
#define WRD_FMT "%D"
#endif WRD_FMT
#else
#define word int
#ifndef WRD_FMT
#define WRD_FMT "%d"
#endif WRD_FMT
#endif
