/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*  Core management for the EM assembler.
    two routines:
	getarea(size)
		returns a pointer to a free area of 'size' bytes.
	freearea(ptr,size)
		free's the area of 'size' bytes pointed to by ptr

    Free blocks are linked together and kept sorted.
    Adjacent free blocks are collapsed.
    Free blocks with a size smaller then the administration cannot
    exist.
    The algorithm is first fit.
*/

#include "ass00.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

#ifdef MEMUSE
static unsigned m_used = 0 ;
static unsigned m_free = 0 ;
#endif

struct freeblock {
	struct freeblock *f_next ;
	unsigned         f_size  ;
} ;

static struct freeblock freexx[2] = {
	{ freexx, 0 },
	{ freexx+1, 0 }
} ;

#define freehead freexx[1]

#define CHUNK 2048              /* Smallest chunk to be gotten from UNIX */

area_t getarea(size) unsigned size ; {
	register struct freeblock *c_ptr,*l_ptr ;
	register char *ptr ;
	unsigned rqsize ;

	size = ((size + (sizeof(int) - 1)) / sizeof(int)) * sizeof(int);
#ifdef MEMUSE
	m_used += size ;
	m_free -= size ;
#endif
	for(;;) {
		for ( l_ptr= &freehead, c_ptr= freehead.f_next ;
		      c_ptr!= &freehead ; c_ptr = c_ptr->f_next ) {
			if ( size==c_ptr->f_size ) {
				l_ptr->f_next= c_ptr->f_next ;
				return (area_t) c_ptr ;
			}
			if ( size+sizeof freehead <= c_ptr->f_size ) {
				c_ptr->f_size -= size ;
			   return (area_t) ((char *) c_ptr + c_ptr->f_size) ;
			}
			l_ptr = c_ptr ;
		}
		rqsize = size<CHUNK ? CHUNK : size ;
		for(;;){
			ptr = malloc( rqsize ) ;
			if ( ptr ) break ; /* request succesfull */
			rqsize /= 2 ;
			rqsize -= rqsize%sizeof (int) ;
			if ( rqsize < sizeof freehead ) {
				fatal("Out of memory") ;
			}
		}
		freearea((area_t)ptr,rqsize) ;
#ifdef MEMUSE
		m_used += rqsize ;
#endif
	}
	/* NOTREACHED */
}

freearea(ptr,size) register area_t ptr ; unsigned size ; {
	register struct freeblock *c_ptr, *l_ptr ;

	size = ((size + (sizeof(int) - 1)) / sizeof(int)) * sizeof(int);
#ifdef MEMUSE
	m_free += size ;
	m_used -= size ;
#endif
	for ( l_ptr= &freehead, c_ptr=freehead.f_next ;
	      c_ptr!= &freehead ; c_ptr= c_ptr->f_next ) {
		if ( (area_t)c_ptr>ptr ) break ;
		l_ptr= c_ptr ;
	}
	/* now insert between l_ptr and c_ptr */
	/* Beware they may both point to freehead */

#ifdef MEMUSE
	if ( ((char *)l_ptr)+l_ptr->f_size> (char *)ptr && (char *)l_ptr<=(char *)ptr )
		fatal("Double freed") ;
	if ( ((char *)ptr)+size > (char *)c_ptr && (char *)ptr<=(char *)c_ptr )
		fatal("Frreed double") ;
#endif
	/* Is the block before this one adjacent ? */
	if ( ((char *)l_ptr) + l_ptr->f_size == (char *) ptr ) {
		l_ptr->f_size += size ; /* yes */
	} else {
		/* No, create an entry */
		((struct freeblock *)ptr)->f_next = c_ptr ;
		((struct freeblock *)ptr)->f_size = size ;
		l_ptr->f_next = (struct freeblock *)ptr ;
		l_ptr = (struct freeblock *)ptr ;
	}
	/* Are the two entries adjacent ? */
	if ( (char *)l_ptr + l_ptr->f_size == (char *) c_ptr ) {
		/* the two entries are adjacent */
		l_ptr->f_next = c_ptr->f_next ;
		l_ptr->f_size += c_ptr->f_size ;
	}
}

#ifdef MEMUSE
memuse() {
	printf("Free %7u, Used %7u, Total %7u\n",m_free,m_used,m_free+m_used);
}
#endif
