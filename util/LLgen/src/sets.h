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

/*
 * Initialises some variables needed for setcomputations
 */
void setinit(int nt_needed);
/*
 * Allocate a set that cannot be freed
 */
p_set get_set(void);
/*
 * Allocate a set which can later be freed.
 */
p_set setalloc(void);
/*
 * a = a union b.
 * Return 1 if the set a changed
 */
int setunion(register p_set a,register p_set b);
/*
 * a = a intersect b.
 * return 1 if the result is empty
 */
int setintersect(register p_set a,register p_set b);
/*
 * a = a setminus b
 */
void setminus(register p_set a,register p_set b);
/*
 * Return 1 if the set p is empty
 */
int setempty(register p_set p);
/*
 * The set "set" will serve as a recovery set.
 * Search for it in the table. If not present, enter it.
 * Here is room for improvement. At the moment, the list of
 * sets is examined with linear search.
 */
int findindex(p_set set);
int setcount(register p_set set, int *saved);

