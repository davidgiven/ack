/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * sets.h $Header$
 * Some macros that deal with bitsets and their size
 */

# define BITS		(8 * sizeof (int))
# define IN(a,i)	((a)[(i)/BITS] & (1<<((i) % BITS)))
# define NTIN(a,i)	((a)[(i)/BITS+tsetsize]&(1<<((i)%BITS)))
# define PUTIN(a,i)	((a)[(i)/BITS] |=(1<<((i) % BITS)))
# define NTPUTIN(a,i)	((a)[(i)/BITS+tsetsize]|=(1<<((i)%BITS)))
# define NBYTES(n)	(((n) + 7) / 8)
/*
 * The next two macros operate on byte counts!
 */
# define NINTS(n)	(((n) + (int) (sizeof(int) - 1)) / (int) sizeof(int))
# define ALIGN(n)	(NINTS(n) * (int) sizeof (int))

extern int	tsetsize;
extern p_set	*setptr, *maxptr;
extern int	nbytes;
