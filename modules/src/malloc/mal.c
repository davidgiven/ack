/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	"param.h"
#include	"impl.h"
#include	"check.h"
#include	"log.h"
#include	"phys.h"

/*	Malloc space is traversed by N doubly-linked lists of chunks, each
	containing a couple of house-keeping data addressed as a
	'mallink' and a piece of useful space, called the block.
	The N lists are accessed through their starting pointers in
	free_list[].  Free_list[n] points to a list of chunks between
	2**(n+LOG_MIN_SIZE) and 2**(n+LOG_MIN_SIZE+1)-1, which means
	that the smallest chunk is 2**LOG_MIN_SIZE (== MIN_SIZE).
*/

#ifdef SYSTEM
#include	<system.h>
#define SBRK	sys_break
#else
#define SBRK	sbrk
#define	ILL_BREAK		(char *)(-1)	/* funny failure value */
#endif
extern char *SBRK();
#ifdef STORE
#define	MAX_STORE	32
#define MAX_SZ_IN_STORE	(MAX_STORE*ALIGNMENT)
private do_free(), sell_out();
privatedata mallink *store[MAX_STORE];
#endif /* STORE */

char *
malloc(n)
	register unsigned int n;
{check_mallinks("malloc entry");{
	register mallink *ml;
	register int min_class;

	if (n == 0) {
		return 0;
	}
	if (n < MIN_SIZE) n = align(MIN_SIZE); else n = align(n);
#ifdef STORE
	if (n <= MAX_SZ_IN_STORE)	{
		/* look in the store first */
		register mallink **stp = &store[(n >> LOG_ALIGNMENT) - 1];
		
		if (ml = *stp)	{
			*stp = log_next_of(ml);
			set_store(ml, 0);
			check_mallinks("malloc fast exit");
			assert(! in_store(ml));
			return block_of_mallink(ml);
		}
	}
#endif /* STORE */

	check_work_empty("malloc, entry");

	/*	Acquire a chunk of at least size n if at all possible;
		Try everything.
	*/
	{
		/*	Inline substitution of "smallest".
		*/
		register unsigned int n1 = n;

		assert(n1 < (1L << LOG_MAX_SIZE));
		min_class = 0;

		do {
			n1 >>= 1;
			min_class++;
		} while (n1 >= MIN_SIZE);
	}

	if (min_class >= MAX_FLIST)
		return (char *) 0;	/* we don't deal in blocks that big */
	ml = first_present(min_class);
	if (ml == MAL_NULL)	{
		/*	Try and extend */
		register char *p;
#define	GRABSIZE	4096		/* Power of 2 */
		register unsigned int req =
			((MIN_SIZE<<min_class)+ mallink_size() + GRABSIZE - 1) &
				~(GRABSIZE-1);
	
		if (!ml_last)	{
			/* first align SBRK() */
		
			p = SBRK(0);
			SBRK((int) (align((size_type) p) - (size_type) p));
		}

		/* SBRK takes an int; sorry ... */
		if ((int) req < 0) {
			p = ILL_BREAK;
		}
		else {
			p = SBRK((int)req);
		}
		if (p == ILL_BREAK) {
			req = n + mallink_size();
			if ((int) req >= 0) p = SBRK((int)req);
		}
		if (p == ILL_BREAK)	{
			/*	Now this is bad.  The system will not give us
				more memory.  We can only liquidate our store
				and hope it helps.
			*/
#ifdef STORE
			sell_out();
			ml = first_present(min_class);
			if (ml == MAL_NULL)	{
#endif /* STORE */
				/* In this emergency we try to locate a suitable
				   chunk in the free_list just below the safe
				   one; some of these chunks may fit the job.
				*/
				ml = search_free_list(min_class - 1, n);
				if (!ml)	/* really out of space */
					return (char *) 0;
				started_working_on(ml);
				unlink_free_chunk(ml);
				check_mallinks("suitable_chunk, forced");
#ifdef STORE
			}
			else started_working_on(ml);
#endif /* STORE */
		}
		else {
			assert((size_type)p == align((size_type)p));
			ml = create_chunk(p, req);
		}
		check_mallinks("suitable_chunk, extended");
	}
	else started_working_on(ml);

	/* we have a chunk */
	set_free(ml, 0);
	calc_checksum(ml);
	check_mallinks("suitable_chunk, removed");
	n += mallink_size();
	if (n + MIN_SIZE <= size_of(ml)) {
		truncate(ml, n);
	}
	stopped_working_on(ml);
	check_mallinks("malloc exit");
	check_work_empty("malloc exit");
#ifdef STORE
	assert(! in_store(ml));
#endif
	return block_of_mallink(ml);
}}

free(addr)
	char *addr;
{check_mallinks("free entry");{
	register mallink *ml;
	
	if (addr == 0) {
		check_mallinks("free(0) very fast exit");
		return;
	}
	ml = mallink_of_block(addr);

#ifdef STORE

	if (free_of(ml) || in_store(ml))
		return;				/* user frees free block */
	if (size_of(ml) <= MAX_SZ_IN_STORE)	{
		/* return to store */
		mallink **stp = &store[(size_of(ml) >> LOG_ALIGNMENT) - 1];
		
		set_log_next(ml, *stp);
		*stp = ml;
		set_store(ml, 1);
		calc_checksum(ml);
		check_mallinks("free fast exit");
	}
	else	{
		do_free(ml);
		check_mallinks("free exit");
	}
}}

private
do_free(ml)
	register mallink *ml;
{{
#endif

#ifndef STORE
	if (free_of(ml))	return;
#endif /* STORE */
	started_working_on(ml);
	set_free(ml, 1);
	calc_checksum(ml);
	if (! last_mallink(ml)) {
		register mallink *next = phys_next_of(ml);

		if (free_of(next)) coalesce_forw(ml, next);
	}

	if (! first_mallink(ml)) {
		register mallink *prev = phys_prev_of(ml);

		if (free_of(prev)) {
			coalesce_backw(ml, prev);
			ml = prev;
		}
	}
	link_free_chunk(ml);
	stopped_working_on(ml);
	check_work_empty("free");

	/* Compile-time checks on param.h */
	switch (0)	{
	case MIN_SIZE < OFF_SET * sizeof(mallink):	break;
	case 1:	break;
	/*	If this statement does not compile due to duplicate case
		entry, the minimum size block cannot hold the links for
		the free blocks.  Either raise LOG_MIN_SIZE or switch
		off NON_STANDARD.
	*/
	}
	switch(0)	{
	case sizeof(char *) != sizeof(size_type):	break;
	case 1:	break;
	/*	If this statement does not compile due to duplicate
		case entry, size_type is not defined correctly.
		Redefine and compile again.
	*/
	}
}}

char *
realloc(addr, n)
	char *addr;
	register unsigned int n;
{check_mallinks("realloc entry");{
	register mallink *ml, *ph_next;
	register size_type size;

	if (addr == 0) {
		/*	Behave like most Unix realloc's when handed a
			null-pointer
		*/
		return malloc(n);
	}
	if (n == 0) {
		free(addr);
		return 0;
	}
	ml = mallink_of_block(addr);
	if (n < MIN_SIZE) n = align(MIN_SIZE); else n = align(n);
#ifdef STORE
	if (in_store(ml)) {
		register mallink *stp = store[(size_of(ml) >> LOG_ALIGNMENT) - 1];
		mallink *stp1 = 0;
		while (ml != stp)	{
			stp1 = stp;
			stp = log_next_of(stp);
		}
		stp = log_next_of(stp);
		if (! stp1) store[(size_of(ml) >> LOG_MIN_SIZE) - 1] = stp;
		else set_log_next(stp1, stp);
		set_store(ml, 0);
		calc_checksum(ml);
	}
#endif
	if (free_of(ml)) {
		unlink_free_chunk(ml);
		set_free(ml, 0);		/* user reallocs free block */
	}
	started_working_on(ml);
	size = size_of(ml);
	if (	/* we can simplify the problem by adding the next chunk: */
		n > size &&
		!last_mallink(ml) &&
		(ph_next = phys_next_of(ml), free_of(ph_next)) &&
		n <= size + mallink_size() + size_of(ph_next)
	)	{
		/* add in the physically next chunk */
		unlink_free_chunk(ph_next);
		combine_chunks(ml, ph_next);
		size = size_of(ml);
		check_mallinks("realloc, combining");
	}
	if (n > size)	{		/* this didn't help */
		char *new;
		register char *l1, *l2 = addr;

		stopped_working_on(ml);
		if (!(new = l1 = malloc(n))) return (char *) 0;	/* no way */
		while (size--) *l1++ = *l2++;
		free(addr);
		check_work_empty("mv_realloc");
#ifdef STORE
		assert(! in_store(mallink_of_block(new)));
#endif
		return new;
	}
	/* it helped, but maybe too well */
	n += mallink_size();
	if (n + MIN_SIZE <= size_of(ml)) {
		truncate(ml, n);
	}
	stopped_working_on(ml);
	check_mallinks("realloc exit");
	check_work_empty("realloc");
#ifdef STORE
	assert(! in_store(ml));
#endif
	return addr;
}}

/*	Auxiliary routines */

#ifdef STORE
private
sell_out()	{
	/*	Frees all block in store.
	*/
	register mallink **stp;
	
	for (stp = &store[0]; stp < &store[MAX_STORE]; stp++)	{
		register mallink *ml = *stp;
		
		while (ml)	{
			*stp = log_next_of(ml);
			set_store(ml, 0);
			do_free(ml);
			ml = *stp;
		}
	}

}
#endif /* STORE */

#ifdef	ASSERT
public
m_assert(fn, ln)
	char *fn;
{
	char ch;
	
	while (*fn)
		write(2, fn++, 1);
	write(2, ": malloc assert failed in line ", 31);
	ch = (ln / 100) + '0'; write(2, &ch, 1); ln %= 100;
	ch = (ln / 10) + '0'; write(2, &ch, 1); ln %= 10;
	ch = (ln / 1) + '0'; write(2, &ch, 1);
	write(2, "\n", 1);
	maldump(1);
}
#endif	/* ASSERT */
