/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <string.h>
#include "ack.h"
#include "list.h"
#include "trans.h"
#include "data.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

static void start_scan(void);
static void try(list_elem *, const char *);
static void scan_found(void);
static int satisfy(trf *, const char *);
static enum f_path scan_end(trf **);
static void find_cpp(void);

enum f_path getpath(trf **first) {
	/* Try to find a transformation path */

	start_scan();
	/*
		The end result is the chaining of
		the consequtive phases with the t_next field.
		The list is scanned for possible transformations
		stopping at stopsuffix or the last transformation in the list.
		The scan flags are set by this process.
		When a transformation is found, it is compared with
		the last transformation found.
	*/
	try(l_first(tr_list),p_suffix);
	return scan_end(first);
}

/******************** data used only while scanning *******************/

static	int	last_pcount;	/* The added priority of
				   the best path so far */

static  int     last_ncount;    /* The # of non-optimizing transformations
				   in the best path sofar */

static  int     last_ocount;    /* The # of optimizing transformations in the
				   best path sofar */

static  int     suf_found;      /* Was the suffix at least recognized ? */

/********************       The hard work          ********************/

static void start_scan(void) {
	register list_elem *scan ;

	scanlist(l_first(tr_list),scan) {
		t_cont(*scan)->t_scan=NO ;
	}
	suf_found= 0 ;
#ifdef DEBUG
	if ( debug>=3 ) vprint("Scan_start\n");
#endif
	last_ncount= -1 ;
	last_ocount= 0 ;
}

static void try(list_elem *f_scan, const char *suffix) {
	register list_elem *scan ;
	register trf  *trafo ;
	/* Try to find a transformation path starting at f_scan for a
	   file with the indicated suffix.
	   If the suffix is already reached or a combiner is found
	   call scan_found() to OK the scan.
	   If a transformation is found it calls itself recursively
	   with as starting point the next transformation in the list.
	*/
	if ( stopsuffix && *stopsuffix && strcmp(stopsuffix,suffix)==0 ) {
		scan_found();
		return ;
	}
	if (! f_scan) return;
	scanlist(f_scan, scan) {
		trafo= t_cont(*scan) ;
		if ( satisfy(trafo,suffix) ) {
			/* Found a transformation */
			suf_found= 1;
#ifdef DEBUG
			if ( debug>=4 ) {
				vprint("Found %s for %s: result %s\n",
				       trafo->t_name,suffix,trafo->t_out);
			}
#endif
			trafo->t_scan=YES ;
			if ( trafo->t_prep ) {
				if ( !cpp_trafo ) {
					find_cpp() ;
				}
				if ( stopsuffix &&
				     strcmp(stopsuffix,
					cpp_trafo->t_out)==0 )
				{
					scan_found() ;
					return ;
				}
			}
			if ( trafo->t_next ) {
				/* We know what happens from this phase on,
				   so take a shortcut.
				*/
				register trf *sneak ;
				sneak= trafo ;
				while( (sneak=sneak->t_next) ) {
					sneak->t_scan=YES ;
				}
				scan_found() ;
				sneak= trafo ;
				while( (sneak=sneak->t_next) ) {
					sneak->t_scan=NO ;
				}
				return ;
			}
			if ( trafo->t_linker && stopsuffix && !*stopsuffix ) {
				trafo->t_scan=NO ;
				scan_found() ;
				return ;
			}
			if (! l_next(*scan) && !stopsuffix) {
				scan_found() ;
			} else {
				try(l_next(*scan),trafo->t_out);
			}
			trafo->t_scan= NO ;
		}
	}
}

static void scan_found(void) {
	register list_elem *scan;
	int ncount, ocount, pcount ;

	suf_found= 1;
#ifdef DEBUG
	if ( debug>=3 ) vprint("Scan found\n") ;
#endif
	/* Gather data used in comparison */
	ncount=0; ocount=0; pcount=0;
	scanlist(l_first(tr_list),scan) {
		if (t_cont(*scan)->t_scan) {
#ifdef DEBUG
			if ( debug>=4 ) vprint("%s-",t_cont(*scan)->t_name) ;
#endif
			if( t_cont(*scan)->t_optim ) ocount++ ;else ncount++ ;
			if( t_cont(*scan)->t_optim>Optlevel ) pcount-- ;
			pcount += t_cont(*scan)->t_priority ;
		}
	}
#ifdef DEBUG
	if ( debug>=4 ) vprint("\n");
#endif
	/* Is this transformation better then any found yet ? */
#ifdef DEBUG
	if ( debug>=3 ) {
		vprint("old n:%d, o:%d, p:%d - new n:%d, o:%d, p:%d\n",
			last_ncount,last_ocount,last_pcount,
			ncount,ocount,pcount) ;
	}
#endif
	if ( last_ncount== -1 ||                /* None found yet */
	     last_pcount<pcount ||		/* Better priority */
	     ( last_pcount==pcount &&		/* Same prio, and */
	        ( last_ncount>ncount ||              /* Shorter nec. path */
	          (last_ncount==ncount &&            /* Same nec. path, optimize?*/
		    (Optlevel? last_ocount<ocount : last_ocount>ocount ))))) {
		/* Yes it is */
#ifdef DEBUG
		if ( debug>=3 ) vprint("Better\n");
#endif
		scanlist(l_first(tr_list),scan) {
			t_cont(*scan)->t_bscan=t_cont(*scan)->t_scan;
		}
		last_ncount=ncount; last_ocount=ocount; last_pcount=pcount;
	}
}

static int satisfy(trf *trafo, const char *suffix) {
	register char *f_char, *l_char ;
	/* Check whether this transformation is present for
	   the current machine and the parameter suffix is among
	   the input suffices. If so, return 1. 0 otherwise
	*/
	if ( trafo->t_isprep ) return 0 ;
	l_char=trafo->t_in ;
	while ( l_char ) {
		f_char= l_char ;
		if ( *f_char!=SUFCHAR || ! *(f_char+1) ) {
			fuerror("Illegal input suffix entry for %s",
				trafo->t_name) ;
		}
		l_char=strchr(f_char+1,SUFCHAR);
		if ( l_char ) *l_char = 0;
		if ( strcmp(f_char,suffix)==0 ) {
			if ( l_char ) *l_char = SUFCHAR;
			return 1;
		}
		if ( l_char ) *l_char = SUFCHAR;
	}
	return 0 ;
}

static enum f_path scan_end(trf **first) {    /* Finalization */
	/* Return value indicating whether a transformation was found */
	/* Set the flags for the transformation up to, but not including,
	   the combiner
	*/
	register trf *prev, *curr ;
	register list_elem *scan;

#ifdef DEBUG
	if ( debug>=3 ) vprint("End_scan\n");
#endif
	if ( last_ncount== -1 ) return suf_found ? F_NOPATH : F_NOMATCH ;
#ifdef DEBUG
	if ( debug>=2 ) vprint("Transformation found\n");
#endif
	prev= (trf *)0 ; *first= prev ;
	scanlist(l_first(tr_list),scan) {
		curr= t_cont(*scan) ;
		if ( curr->t_bscan ) {
			if ( prev ) {
				prev->t_next= curr ;
				if ( curr->t_linker ) prev->t_keep=YES ;
			} else {
				*first= curr ;
			}
			if ( curr->t_next ) {
				return F_TRANSFORM ;
			}
			prev=curr ;
		}
	}
	if ( cpp_trafo && stopsuffix &&
	     strcmp(stopsuffix,cpp_trafo->t_out)==0 ) {
		cpp_trafo->t_keep=YES ;
	}
	if ( prev ) {
		prev->t_keep=YES ;
	}
	return F_TRANSFORM ;
}

static void find_cpp(void) {
	register list_elem *elem ;
	scanlist( l_first(tr_list), elem ) {
		if ( t_cont(*elem)->t_isprep ) {
			if ( cpp_trafo ) fuerror("Multiple cpp's present") ;
			cpp_trafo= t_cont(*elem) ;
		}
	}
	if ( !cpp_trafo ) fuerror("No cpp present") ;
}
