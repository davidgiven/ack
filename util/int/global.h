/*
	Defines and externs of general interest
*/

/* $Header$ */


/********* PRIVATE/static *********/

#define	PRIVATE		static		/* or not */


/********* The internal data types ********/

#define	UNSIGNED			/* the normal case */
#ifdef	UNSIGNED

/* The EM pointer is an abstract type and requires explicit conversion*/
typedef unsigned long ptr;		/* pointer to EM address */
#define	p2i(p)		(p)		/* convert pointer to index */
#define	i2p(p)		(ptr)(p)	/* convert index to pointer */

#else	/* UNSIGNED */

typedef char *ptr;			/* pointer to EM address */
#define	p2i(p)		(long)(p)	/* convert pointer to index */
#define	i2p(p)		(ptr)(p)	/* convert index to pointer */

#endif /* UNSIGNED */

/* The EM size is an integer type; a cast suffices */
typedef long size;


/********* Mathematical constants ********/

#define	I_MAXU1		255L
#define	I_MAXS1		127L
#define	I_MINS1		(-127L-1L)

#define	I_MAXU2		65535L
#define	I_MAXS2		32767L
#define	I_MINS2		(-32767L-1L)

#define	I_MAXU4		4294967295L
#define	I_MAXS4		2147483647L
#define	I_MINS4		(-2147483647L-1L)

#define	FL_MAXU1	255.0
#define	FL_MAXS1	127.0
#define	FL_MINS1	-128.0

#define	FL_MAXU2	65535.0
#define	FL_MAXS2	32767.0
#define	FL_MINS2	-32768.0

#define	FL_MAXU4	4294967295.0
#define	FL_MAXS4	2147483647.0
#define	FL_MINS4	-2147483648.0

#define	BIT(n)		(1L<<(n))

#define	SIGNBIT1	BIT(7)		/* Signbit of one byte signed int */
#define	SIGNBIT2	BIT(15)		/* Signbit of two byte signed int */
#define	SIGNBIT4	BIT(31)		/* Signbit of four byte signed int */

#define	MASK1		0xFF		/* To mask one byte */
#define	MASK2		0xFFFF		/* To mask two bytes */


/******** Machine constants ********/

#define	MAX_OFF2	I_MAXS2
#define	MAX_OFF4	I_MAXS4


/******** EM machine data sizes ********/

#define	FRALIMIT	8L		/* Default limit */
#define	LINSIZE		4L		/* Fixed size of LIN number */


/******** EM Machine capacity parameters ********/

extern size wsize;		/* wordsize */
extern size dwsize;		/* double wordsize */
extern size psize;		/* pointersize */
extern long i_minsw;		/* Min. value for signed integer of wsize */
extern long i_maxsw;		/* Max. value for signed integer of wsize */
extern unsigned long i_maxuw;	/* Max. value for unsigned integer of wsize */
extern long min_off;		/* Minimum offset */
extern long max_off;		/* Maximum offset */
extern ptr max_addr;		/* Maximum address */
extern size wsizem1;		/* wordsize - 1 */


/******** EM program parameters ********/

extern ptr ML;			/* Memory Limit */
extern ptr HB;			/* Heap Base */
extern ptr DB;			/* Procedure Descriptor Base, end of text */
extern long NProc;		/* Number of Procedure Descriptors */
extern long PreIgnMask;		/* Preset Ignore Mask, from command line */


/******** EM machine registers ********/

#define	UNDEFINED	(0)
#define	DEFINED		(1)

extern long PI;			/* Procedure Identifier of running proc */
extern ptr PC;			/* Program Counter */

extern ptr HP;			/* Heap Pointer */
extern ptr SP;			/* Stack Pointer */
extern ptr LB;			/* Local Base */
extern ptr AB;			/* Actual Base */

extern long ES;			/* program Exit Status */
extern int ES_def;		/* set iff Exit Status legal */

#define	TR_ABORT	(1)
#define	TR_HALT		(2)
#define	TR_TRAP		(3)
extern int OnTrap;		/* what to do upon trap */
extern long IgnMask;		/* Ignore Mask for traps */
extern long TrapPI;		/* Procedure Identifier of trap routine */

extern char *FRA;		/* Function Return Area */
extern size FRALimit;		/* Function Return Area maximum Size */
extern size FRASize;		/* Function Return Area actual Size */
extern int FRA_def;		/* set iff Function Return Area legal */


/******** The EM Machine Memory ********/

extern char *text;		/* program text & procedure descriptors */

extern char *data;		/* global data & heap space */
extern ptr HL;			/* Heap Limit */

extern char *stack;		/* stack space and local data */
extern char *stackML;		/* stack + ML, to speed up stack accesses */
extern ptr SL;			/* Stack Limit */


/********* Global inline functions ********/

#define	btol(c)		(long)((c) & MASK1)
#define	btou(c)		(unsigned int)((c) & MASK1)
#define	btos(c)		(c)

#define	max(i,j)	(((i) > (j)) ? (i) : (j))
#define	min(i,j)	(((i) < (j)) ? (i) : (j))


