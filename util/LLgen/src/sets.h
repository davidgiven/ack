/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * $Id$
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
