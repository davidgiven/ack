/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	Algorithms to manipulate the doubly-linked lists of free
	chunks.
*/

public link_free_chunk(), unlink_free_chunk();
public mallink *first_present(), *search_free_list();

#ifdef STORE
#define in_store(ml)		((size_type)_log_prev_of(ml) & 01)
#define set_store(ml, e) \
	(_log_prev_of(ml) = (mallink *) \
		((e) ? (size_type) _log_prev_of(ml) | 01 : \
		       (size_type) _log_prev_of(ml) & ~01))
#define log_prev_of(ml)		(mallink *)((size_type)_log_prev_of(ml) & ~01)
#define set_log_prev(ml,e) \
	(_log_prev_of(ml) = (mallink *)((char *)e + in_store(ml)))
#else
#define	set_log_prev(ml,e)	(_log_prev_of(ml) = (e))
#define	log_prev_of(ml)		(mallink *) (_log_prev_of(ml))
#endif

#define	set_log_next(ml,e)	(_log_next_of(ml) = (e))
#define	log_next_of(ml)		(mallink *) (_log_next_of(ml))

