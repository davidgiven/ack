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
 * sets.c
 * Some general setmanipulation routines are defined,
 * and also two set allocating routines are defined
 */

# include "types.h"
# include "extern.h"
# include "sets.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid9 = "$Header$";
# endif

/* In this file the following routines are defined: */
extern		setinit();
extern p_set	setalloc();
extern int	setunion();
extern int	setintersect();
extern 		setminus();
extern int	setempty();
extern int	findindex();
extern int	setcount();

int		tbitset;
int		setsize,tsetsize;
p_set		*setptr, *maxptr, *topptr;

static unsigned	size,nbytes;

setinit(ntneeded) {
	/*
	 * Initialises some variables needed for setcomputations
	 */
	register int	 bitset;

	nbytes = NBYTES(nterminals);
	tbitset = ALIGN(nbytes);
	tsetsize = NINTS(tbitset);
	bitset = tbitset;
	if (ntneeded) {
		/* nonterminals must be included in the sets */
		bitset += NBYTES(nnonterms);
	}
	setsize = NINTS(bitset);
	tbitset *= 8;
}

p_set
setalloc(size) int size; {
	/*
	 * Allocate a set of size "size" ints
	 */
	register p_set	t;
	register int	i;
	p_mem		alloc();

	assert(size == tsetsize || size == setsize);
	t = (p_set) alloc((unsigned) (size * sizeof(int)));
	i = size;
	t += i;
	for (; i; i--) {
		*--t = 0;
	}
	return t;
}

int
setunion(a,b,size) register p_set a,b; int size; {
	/*
	 * a = a union b.
	 * Return 1 if the set a changed
	 */
	register	i;
	register	j;
	int		nsub = 0;

	assert(size == tsetsize || size == setsize);
	for (i = size; i; i--) {
		*a = (j = *a) | *b++;
		if (*a++ != j) {
			nsub = 1;
		}
	}
	return nsub;
}

int
setintersect(a,b,size) register p_set a,b; int size; {
	/*
	 * a = a intersect b.
	 * return 1 if the resut is empty
	 */
	register	i;
	register	nempty;

	assert(size == tsetsize || size == setsize);
	nempty = 1;
	for (i = size; i; i--) {
		if (*a++ &= *b++) nempty = 0;
	}
	return nempty;
}

setminus(a,b,size) register p_set a,b; int size; {
	/*
	 * a = a setminus b
	 */
	register	i;

	assert(size == tsetsize || size == setsize);
	for (i = size; i; i--) {
		*a++ &= ~(*b++);
	}
}

int
setempty(p) register p_set p; {
	/*
	 * Return 1 if the set p is empty
	 */
	register	i;

	for (i = tsetsize; i; i--) {
		if (*p++) return 0;
	}
	return 1;
}

int
findindex(set) p_set *set; {
	/*
	 * The set "set" will serve as a recovery set.
	 * Search for it in the table. If not present, enter it
	 */
	register p_set	*t;
	p_mem		alloc(),ralloc();
	register p_set	a;
	register p_set	b;
	register	i;
	register	j;
	int		saved;

	/*
	 * First search for the set in the table
	 */
	for (t = setptr; t < maxptr; t++) {
		a = *t;
		b = *set;
		for (i = tsetsize; i; i--) {
			if (*a++ != *b++) break;
		}
		if (i) continue;
		/*
		 * Here, the sets are equal.
		 */
		return nbytes * (t - setptr);
	}
	/*
	 * Now check if the set consists of only one element.
	 * It would be a waste to use a set for that
	 */
	if (setcount(*set, &saved) == 1) return -h_entry[saved].h_num;
	/*
	 * If it does, return its number as a negative number.
	 */
	if (maxptr >= topptr) {
		/*
		 * Need new space for the list, in chunks of 50 pointers
		 */
		if (setptr == 0) {
			setptr = (p_set *) alloc(50 * sizeof(p_set));
			size = 50;
			maxptr = setptr;
		} else {
			setptr = (p_set *) ralloc((p_mem) setptr,
						(50+size)*sizeof(p_set));
			maxptr = &setptr[size-1];
			size += 50;
		}
		topptr = &setptr[size-1];
	}
	*maxptr = setalloc(tsetsize);
	setunion(*maxptr, *set, tsetsize);
	return nbytes * (maxptr++ - setptr);
}

int
setcount(set, saved) register p_set set; int *saved; {
	register int i, j;

	for (j = 0, i = 0; i < nterminals; i++) {
		if (IN(set,i)) {
			j++;
			*saved = i;
		}
	}
	return j;
}
