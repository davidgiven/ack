/*  G L O B A L   V A R I A B L E S   */

/* sizes of TARGET machine */

extern int ps;		/* pointer size */
extern int ws;		/* word size	*/

/* sizes of SOURCE machine (i.e. machine on which
 * the optimizer runs)
 */

/* number of bits in a byte */
#define BYTELENGTH 8

/* number of bits in a word */
#define WORDLENGTH 32

#if BYTELENGTH==8
#define DIVBL(a)	((a) >> 3)
#define MODBL(a)	((a) & 07)
#else
#define DIVBL(a)	(a/BYTELENGTH)
#define MODBL(a)	(a%BYTELENGTH)
#endif

#if WORDLENGTH==16
#define DIVWL(a)	((a) >> 4)
#define MODWL(a)	((a) & 017)
#else
#if WORDLENGTH==32
#define DIVWL(a)	((a) >> 5)
#define MODWL(a)	((a) & 037)
#else
#define DIVWL(a)	(a/WORDLENGTH)
#define MODWL(a)	(a%WORDLENGTH)
#endif
#endif


#define UNKNOWN_SIZE (-1)

extern proc_p curproc;  /* current procedure */

extern char *filename; /* name of current input file */

extern lset mesregs;	/* set of MES ms_reg pseudos */

extern short time_space_ratio; /* 0   if optimizing for space only,
				 * 100 if optimizing for time only,
				 * else something 'in between'.
				 */
