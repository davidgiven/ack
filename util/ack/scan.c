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

#include "ack.h"
#include "list.h"
#include "trans.h"
#include "data.h"

enum f_path setpath() { /* Try to find a transformation path */

	start_scan();
	/*
		The end result is the setting of the t_do flags
		in the transformation list.
		The list is scanned for possible transformations
		stopping at stopsuffix or a combine transformation.
		The scan flags are set by this process.
		When a transformation is found, it is compared with
		the last transformation found, if better (or the first)
		the scan bits are copied to the t_do bits, except for
		the combiner which is remembered in a global pointer.
		At the end of all transformations for all files, the combiner
		is called, unless errors occurred.
	*/
	try(l_first(tr_list),p_suffix);
	return scan_end();
}

/******************** data used only while scanning *******************/

static  int     last_ncount;    /* The # of non-optimizing transformations
				   in the best path sofar */

static  int     last_ocount;    /* The # of optimizing transformations in the
				   best path sofar */
static  int     com_err;        /* Complain only once about multiple linkers*/

static  trf     *final;         /* The last non-combining transformation */

static  int     suf_found;      /* Was the suffix at least recognized ? */

/********************       The hard work          ********************/

start_scan() {
	register list_elem *scan ;

	scanlist(l_first(tr_list),scan) {
		t_cont(*scan)->t_do=NO ; t_cont(*scan)->t_scan=NO ;
		t_cont(*scan)->t_keep=NO ;
	}
	final= (trf *)0 ;
	suf_found= 0 ;
#ifdef DEBUG
	if ( debug>=3 ) vprint("Scan_start\n");
#endif
	last_ncount= -1 ;
	last_ocount= 0 ;
}

try(f_scan,suffix) list_elem *f_scan; char *suffix; {
	register list_elem *scan ;
	register trf  *trafo ;
	/* Try to find a transformation path starting at f_scan for a
	   file with the indicated suffix.
	   If the suffix is already reached or the combiner is found
	   call scan_found() to OK the scan.
	   If a transformation is found it calls itself recursively
	   with as starting point the next transformation in the list.
	*/
	if ( stopsuffix && *stopsuffix && strcmp(stopsuffix,suffix)==0 ) {
		scan_found();
		return ;
	}
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
			if ( trafo->t_combine ) {
				if ( stopsuffix ) {
					trafo->t_scan=NO;
					if ( *stopsuffix ) return ;
				} else {
					if( combiner &&
					    combiner!=trafo && !com_err ){
					       com_err++ ;
werror("Multiple linkers present %s and %s",
	trafo->t_name,combiner->t_name) ;
					} else {
						combiner=trafo;
					}
				}
				scan_found() ;
			} else {
				try(l_next(*scan),trafo->t_out);
			}
			trafo->t_scan= NO ;
		}
	}
}

scan_found() {
	register list_elem *scan;
	int ncount, ocount ;
	register trf *keepit ;

	keepit= (trf *)0 ;
	suf_found= 1;
#ifdef DEBUG
	if ( debug>=3 ) vprint("Scan found\n") ;
#endif
	/* Gather data used in comparison */
	ncount=0; ocount=0;
	scanlist(l_first(tr_list),scan) {
		if (t_cont(*scan)->t_scan) {
#ifdef DEBUG
			if ( debug>=4 ) vprint("%s-",t_cont(*scan)->t_name) ;
#endif
			if( t_cont(*scan)->t_optim ) ocount++ ;else ncount++ ;
			if ( !(t_cont(*scan)->t_combine) ) {
				keepit= t_cont(*scan) ;
			}
		}
	}
#ifdef DEBUG
	if ( debug>=4 ) vprint("\n");
#endif
	/* Is this transformation better then any found yet ? */
#ifdef DEBUG
	if ( debug>=3 ) {
		vprint("old n:%d, o:%d - new n:%d, o:%d\n",
			last_ncount,last_ocount,ncount,ocount) ;
	}
#endif
	if ( last_ncount== -1 ||                /* None found yet */
	     last_ncount>ncount ||              /* Shorter nec. path */
	     (last_ncount==ncount &&            /* Same nec. path, optimize?*/
		(Optflag? last_ocount<ocount : last_ocount>ocount ) ) ) {
		/* Yes it is */
#ifdef DEBUG
		if ( debug>=3 ) vprint("Better\n");
#endif
		scanlist(l_first(tr_list),scan) {
			t_cont(*scan)->t_do=t_cont(*scan)->t_scan;
		}
		last_ncount=ncount; last_ocount=ocount;
		if ( keepit ) final=keepit ;
	}
}

int satisfy(trafo,suffix) register trf *trafo; char *suffix ; {
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
		l_char=index(f_char+1,SUFCHAR);
		if ( l_char ? strncmp(f_char,suffix,l_char-f_char)==0 :
			      strcmp(f_char,suffix)==0 ) {
			return 1 ;
		}
	}
	return 0 ;
}

enum f_path scan_end() {    /* Finalization */
	/* Return value indicating whether a transformation was found */
	/* Set the flags for the transformation up to, but not including,
	   the combiner
	*/

#ifdef DEBUG
	if ( debug>=3 ) vprint("End_scan\n");
#endif
	if ( last_ncount== -1 ) return suf_found ? F_NOPATH : F_NOMATCH ;
#ifdef DEBUG
	if ( debug>=2 ) vprint("Transformation found\n");
#endif
	if ( cpp_trafo && stopsuffix &&
	     strcmp(stopsuffix,cpp_trafo->t_out)==0 ) {
		final= cpp_trafo ;
	}
	/* There might not be a final when the file can be eaten
	   by the combiner
	*/
	if ( final ) final->t_keep=YES ;
	if ( combiner ) {
		if ( !combiner->t_do ) error("Combiner YES/NO");
		combiner->t_do=NO ;
	}
	return F_OK ;
}

find_cpp() {
	register list_elem *elem ;
	scanlist( l_first(tr_list), elem ) {
		if ( t_cont(*elem)->t_isprep ) {
			if ( cpp_trafo ) fuerror("Multiple cpp's present") ;
			cpp_trafo= t_cont(*elem) ;
		}
	}
	if ( !cpp_trafo ) fuerror("No cpp present") ;
}
