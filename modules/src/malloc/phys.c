/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	"param.h"
#include	"impl.h"
#include	"check.h"
#include	"phys.h"

/*	Physical manipulations.
	The blocks concerned are not in any logical chain.
*/

public mallink *
create_chunk(p, n)
	char *p;
	unsigned int n;
{
	/*	The newly acquired piece of memory at p, of length n,
		is turned into a free chunk, properly chained in the
		physical chain.
		The address of the chunk is returned.
	*/
	register mallink *ml;
	/*	All of malloc memory is followed by a virtual chunk, the
		mallink of which starts mallink_size() bytes past the last
		byte in memory.
		Its use is prevented by testing for ml == ml_last first.
	*/
	register mallink *last = ml_last;
	
	assert(!last || p == (char *)phys_next_of(last) - mallink_size());
	ml = (mallink *)(p + mallink_size());	/* bump ml */
	new_mallink(ml);
	started_working_on(ml);
	set_free(ml, 1);
	set_phys_prev(ml, last);
	ml_last = ml;

	set_phys_next(ml, (mallink *)((char *)ml + n));
	calc_checksum(ml);
	assert(size_of(ml) + mallink_size() == n);
	if (last && free_of(last)) {
		coalesce_backw(ml, last);
		ml = last;
	}
	check_mallinks("create_chunk, phys. linked");
	return ml;
}

public
truncate(ml, size)
	register mallink *ml;
	unsigned int size;
{
	/*	The chunk ml is truncated.
		The chunk at ml is split in two.
		The remaining part is then freed.
	*/
	register mallink *new = (mallink *)((char *)ml + size);
	register mallink *ph_next = phys_next_of(ml);

	new_mallink(new);
	set_free(new, 1);
	set_phys_prev(new, ml);
	set_phys_next(new, ph_next);
	calc_checksum(new);
	if (! last_mallink(ml))	{
		set_phys_prev(ph_next, new);
		calc_checksum(ph_next);
		if (free_of(ph_next)) coalesce_forw(new, ph_next);
	}
	else	ml_last = new;
	set_phys_next(ml, new);
	calc_checksum(ml);

	started_working_on(new);
	link_free_chunk(new);
	stopped_working_on(new);
	check_mallinks("truncate");
}

public
combine_chunks(ml1, ml2)
	register mallink *ml1, *ml2;
{
	/*	The chunks ml1 and ml2 are combined.
	*/
	register mallink *ml3 = phys_next_of(ml2);

	set_phys_next(ml1, ml3);
	calc_checksum(ml1);
	if (!last_mallink(ml2))	{
		set_phys_prev(ml3, ml1);
		calc_checksum(ml3);
	}
	if (ml_last == ml2)
		ml_last = ml1;
}
