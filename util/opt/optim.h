/* $Header$ */

/* #define DIAGOPT /* if defined diagnostics are produced */
#ifdef DIAGOPT
#define OPTIM(x) optim(x)
#define O_UNREACH 1001
#define O_BRALAB  1002
#define O_LINLNI  1003
#define O_LINGONE 1004
#else
#define OPTIM(x)	/* NOTHING */
#endif
