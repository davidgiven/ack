/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	Algorithms to manipulate the doubly-linked list of physical
	chunks.
*/
publicdata mallink *ml_last;

#define FREE_BIT		01
#ifdef STORE
#define STORE_BIT		02
#define BITS			(FREE_BIT|STORE_BIT)
#else
#define BITS			(FREE_BIT)
#endif

#define __bits(ml)		((int)((size_type)_phys_prev_of(ml) & BITS))
#define	__free_of(ml)		((int)((size_type)_phys_prev_of(ml) & FREE_BIT))
#define __phys_prev_of(ml)	((mallink *)((size_type)_phys_prev_of(ml) & ~BITS))
#define prev_size_of(ml)	((char *)(ml) - \
				 (char *)__phys_prev_of(ml) - \
				 mallink_size() \
				)
#define	set_phys_prev(ml,e) \
	(_phys_prev_of(ml) = (mallink *) ((char *)e + __bits(ml)))

#ifdef	CHECK
public Error();
#define	phys_prev_of(ml)	(mallink *) \
	(first_mallink(ml) ? \
		(char *)Error("phys_prev_of first_mallink %lx", "somewhere", (long)ml) : \
		(char *)__phys_prev_of(ml) \
	)
#else	/* ndef	CHECK */
#define	phys_prev_of(ml)	__phys_prev_of(ml)
#endif	/* CHECK */

#define	first_mallink(ml)	(int) (__phys_prev_of(ml) == 0)
#define	last_mallink(ml)	(int) ((ml) == ml_last)

/*	There is an ambiguity in the semantics of phys_next_of: sometimes
	one wants it to return MAL_NULL if there is no next chunk, at
	other times one wants the address of the virtual chunk at the
	end of memory.  The present version returns the address of the
	(virtual) chunk and relies on the user to test last_mallink(ml)
	first.
*/
#define size_of(ml)		(_this_size_of(ml) - mallink_size())
#define	set_phys_next(ml,e) \
	(_this_size_of(ml) = (size_type)((char *)(e) - (char *)(ml)))
#define	phys_next_of(ml)	(mallink *) ((char *)(ml) + _this_size_of(ml))

#define	set_free(ml,e) \
	(_phys_prev_of(ml) = (mallink *) \
		((e) ? (size_type) _phys_prev_of(ml) | FREE_BIT : \
		       (size_type) _phys_prev_of(ml) & ~FREE_BIT))
#define	free_of(ml)		(__free_of(ml))

#define coalesce_forw(ml,nxt)	( unlink_free_chunk(nxt), \
				  combine_chunks((ml), (nxt)))

#define coalesce_backw(ml,prv)	( unlink_free_chunk(prv), \
				  stopped_working_on(ml), \
				  combine_chunks((prv), (ml)), \
				  started_working_on(prv))

#ifdef	CHECK
#define	set_print(ml,e)		(_print_of(ml) = (e))
#define	print_of(ml)		(_print_of(ml))
#endif	/* CHECK */

public truncate(), combine_chunks();
public mallink *create_chunk();
