/*
	Shadowbyte macros
*/

/* $Header$ */

#include	"logging.h"

#ifdef	LOGGING

extern char *data_sh;		/* shadowbytes of data space */
extern char *stack_sh;		/* shadowbytes of stack space */
extern char *stackML_sh;	/* stack_sh + ML (to speed up stack access) */

/* Bit 0, 1, 2 and 3: datatype/pointertype. */

#define	SH_INT		(0x01)
#define	SH_FLOAT	(0x02)
#define	SH_DATAP	(0x04)
#define	SH_INSP		(0x08)

/* Bit 7: protection bit */

#define	SH_PROT		(0x80)

/******** Shadowbytes, general ********/

#define	dt_sh(a)	(*(data_sh + (p2i(a))))
#define	st_sh(a)	(*(stackML_sh - (a)))
#define	mem_sh(a)	(in_stack(a) ? st_sh(a) : dt_sh(a))

/******** Shadowbytes settings for data ********/

#define	dt_undef(a)	(dt_sh(a) = UNDEFINED)

#define	dt_int(a)	(dt_sh(a) = SH_INT)
#define	dt_fl(a)	(dt_sh(a) = SH_FLOAT)
#define	dt_ip(a)	(dt_sh(a) = SH_INSP)
#define	dt_dp(a)	(dt_sh(a) = SH_DATAP)

#define	dt_prot2b(a)	{ dt_sh(a) |= SH_PROT; dt_sh(a+1) |= SH_PROT; }
#define	dt_unpr2b(a)	{ dt_sh(a) &= ~SH_PROT; dt_sh(a+1) &= ~SH_PROT; }
#define	dt_prot(a,n)	{	dt_prot2b(a); \
				if ((n) == 4) { dt_prot2b(a+2); } }
#define	dt_unprot(a,n)	{	dt_unpr2b(a); \
				if ((n) == 4) { dt_unpr2b(a+2); } }

/******** Shadowbytes settings for stack ********/

#define	st_undef(a)	(st_sh(a) = UNDEFINED)

#define	st_int(a)	(st_sh(a) = SH_INT)
#define	st_fl(a)	(st_sh(a) = SH_FLOAT)
#define	st_ip(a)	(st_sh(a) = SH_INSP)
#define	st_dp(a)	(st_sh(a) = SH_DATAP)

#define	st_prot2b(a)	{ st_sh(a) |= SH_PROT; st_sh(a+1) |= SH_PROT; }
#define	st_unpr2b(a)	{ st_sh(a) &= ~SH_PROT; st_sh(a+1) &= ~SH_PROT; }
#define	st_prot(a,n)	{	st_prot2b(a); \
				if ((n) == 4) { st_prot2b(a+2); } }
#define	st_unprot(a,n)	{	st_unpr2b(a); \
				if ((n) == 4) { st_unpr2b(a+2); } }

/******** Shadowbytes checking for data ********/

#define	is_dt_set(a,n,s)	((dt_sh(a) & s) && (dt_sh(a+(n-1)) & s))
#define	is_dt_prot(a)		(dt_sh(a) & SH_PROT)
#define	ch_dt_prot(a)		{ if (is_dt_prot(a)) warning(WDESROM); }

/******** Shadowbytes checking for stack ********/

#define	is_st_set(a,n,s)	((st_sh(a) & s) && (st_sh(a+(n-1)) & s))
#define	is_st_prot(a)		(st_sh(a) & SH_PROT)
#define	ch_st_prot(a)		{ if (is_st_prot(a)) warning(WDESRSB); }

#else

#define	dt_undef(a)

#define	dt_int(a)
#define	dt_fl(a)
#define	dt_ip(a)
#define	dt_dp(a)

#define	dt_prot(a,n)
#define	dt_unprot(a,b)

#define	st_undef(a)

#define	st_int(a)
#define	st_fl(a)
#define	st_ip(a)
#define	st_dp(a)

#define	st_prot(a,n)
#define	st_unprot(a,b)

#define	ch_dt_prot(a)
#define	ch_st_prot(a)

#endif	/* LOGGING */

