/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	"param.h"
#include	"impl.h"
#include	"check.h"
#include	"log.h"

/*	Logical manipulations.
	The chunks are properly chained in the physical chain.
*/

privatedata mallink *free_list[MAX_FLIST];

public
link_free_chunk(ml)
	register mallink *ml;
{
	/*	The free chunk ml is inserted in its proper logical
		chain.
	*/
	register mallink **mlp = &free_list[-1];
	register size_type n = size_of(ml);
	register mallink *ml1;

	assert(n < (1L << LOG_MAX_SIZE));

	do {
		n >>= 1;
		mlp++;
	}
	while (n >= MIN_SIZE);

	ml1 = *mlp;
	set_log_prev(ml, MAL_NULL);
	set_log_next(ml, ml1);
	calc_checksum(ml);
	if (ml1) {
		/* link backwards
		*/
		set_log_prev(ml1, ml);
		calc_checksum(ml1);
	}
	*mlp = ml;
}

public
unlink_free_chunk(ml)
	register mallink *ml;
{
	/*	Unlinks a free chunk from (the middle of) the
		logical chain.
	*/
	register mallink *next = log_next_of(ml);
	register mallink *prev = log_prev_of(ml);

	if (!prev)	{
		/* it is the first in the chain */
		register mallink **mlp = &free_list[-1];
		register size_type n = size_of(ml);

		assert(n < (1L << LOG_MAX_SIZE));
		do {
			n >>= 1;
			mlp++;
		}
		while (n >= MIN_SIZE);
		*mlp = next;
	}
	else	{
		set_log_next(prev, next);
		calc_checksum(prev);
	}
	if (next) {
		set_log_prev(next, prev);
		calc_checksum(next);
	}
}

public mallink *
search_free_list(class, n)
	unsigned int n;
{
	/*	Searches the free_list[class] for a chunk of at least size n;
		since it is searching a slightly undersized list,
		such a block may not be there.
	*/
	register mallink *ml;
	
	for (ml = free_list[class]; ml; ml = log_next_of(ml))
		if (size_of(ml) >= n)
			return ml;
	return MAL_NULL;		/* nothing found */
}

public mallink *
first_present(class)
	int class;
{
	/*	Find the index i in free_list[] such that:
			i >= class && free_list[i] != MAL_NULL.
		Return MAL_NULL if no such i exists;
		Otherwise, return the first block of this list, after
		unlinking it.
	*/
	register mallink **mlp, *ml;

	for (mlp = &free_list[class]; mlp < &free_list[MAX_FLIST]; mlp++) {
		if ((ml = *mlp) != MAL_NULL)	{
	
			*mlp = log_next_of(ml);	/* may be MAL_NULL */
			if (*mlp) {
				/* unhook backward link
				*/
				set_log_prev(*mlp, MAL_NULL);
				calc_checksum(*mlp);
			}
			return ml;
		}
	}
	return MAL_NULL;
}

#ifdef	CHECK
public mallink *
free_list_entry(i)	{
	/*	To allow maldump.c access to log.c's private data.
	*/
	return free_list[i];
}
#endif	/* CHECK */
