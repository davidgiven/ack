/*	Algorithms to manipulate the doubly-linked lists of free
	chunks.
*/

public link_free_chunk(), unlink_free_chunk();
public mallink *first_present(), *search_free_list();

#define	set_log_prev(ml,e)	(_log_prev_of(ml) = (e))
#define	log_prev_of(ml)		(mallink *) (_log_prev_of(ml))

#define	set_log_next(ml,e)	(_log_next_of(ml) = (e))
#define	log_next_of(ml)		(mallink *) (_log_next_of(ml))
