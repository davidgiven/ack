/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  G L O B A L   V A R I A B L E S   */

/* sizes of TARGET machine */

extern int ps;		/* pointer size */
extern int ws;		/* word size	*/

/* sizes of SOURCE machine (i.e. machine on which
 * the optimizer runs)
 */

/* number of bits in a byte */
#define BYTELENGTH 8

#if BYTELENGTH==8
#define DIVBL(a)	((a) >> 3)
#define MODBL(a)	((a) & (int)07)
#else
#define DIVBL(a)	(a/BYTELENGTH)
#define MODBL(a)	(a%BYTELENGTH)
#endif

#define WORDLENGTH	(sizeof(int)*BYTELENGTH)

#define DIVWL(a)	(WORDLENGTH==16 ? \
			  ((a)>>4) : \
			  (WORDLENGTH==32 ? \
			    ((a)>>5) : \
			    ((a)/(8*(int)sizeof(int)))))
#define MODWL(a)	(WORDLENGTH==16 ? \
			  ((a)&(int)017) : \
			  (WORDLENGTH==32 ? \
			    ((a)&(int)037) : \
			    ((a)%(8*(int)sizeof(int)))))

#define UNKNOWN_SIZE (-1)

extern proc_p curproc;  /* current procedure */

extern char *filename; /* name of current input file */

extern lset mesregs;	/* set of MES ms_reg pseudos */

extern short time_space_ratio; /* 0   if optimizing for space only,
				 * 100 if optimizing for time only,
				 * else something 'in between'.
				 */
