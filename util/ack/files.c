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
#include "grows.h"
#include "data.h"
#include "../../h/em_path.h"

setfiles(phase) register trf *phase ; {
	/* Set the out structure according to the in structure,
	   the transformation and some global data */
	growstring pathname ;
	register list_elem *elem ;

	if ( phase->t_combine ) {
		out.p_keep=YES ;
		out.p_path=outfile ;
		out.p_keeps=NO ;
		in.p_path= (char *)0 ;
		in.p_keep=YES ;
		in.p_keeps=NO ;
	} else {
		gr_init(&pathname) ;
		if ( !phase->t_keep && !t_flag ) {
			gr_cat(&pathname,TMP_DIR) ;
			gr_cat(&pathname,"/") ;
			gr_cat(&pathname,template) ;
			out.p_keep=NO ;
		} else {
			gr_cat(&pathname,p_basename) ;
			out.p_keep=YES ;
		}
		gr_cat(&pathname,phase->t_out) ;
		out.p_path= gr_final(&pathname) ;
		out.p_keeps= YES ;
	}
	scanlist( l_first(arguments), elem) {
		if ( strcmp(l_content(*elem),out.p_path)==0 ) {
			error("attempt to overwrite argument file") ;
			return 0 ;
		}
	}
	return 1 ;
}

disc_files() {
	if ( in.p_path ) {
		if ( !in.p_keep ) {
			if ( unlink(in.p_path)!=0 ) {
				werror("couldn't unlink %s",in.p_path);
			}
		}
		if ( in.p_keeps ) throws(in.p_path) ;
	}
	in=out ;
	out.p_path= (char *)0 ;
	out.p_keeps=NO ;
	out.p_keep=NO ;
}

rmtemps() {
	/* Called in case of disaster, always remove the current output file!
	*/
	if ( out.p_path ) {
		unlink(out.p_path) ;
		if ( out.p_keeps ) throws(out.p_path) ;
		out.p_path= (char *)0 ;
		out.p_keeps=NO ;
		out.p_keep=NO ;
	}
	if ( !in.p_keep && in.p_path ) {
		unlink(in.p_path) ;
		if ( in.p_keeps ) throws(in.p_path) ;
		in.p_path= (char *)0 ;
		out.p_keeps= NO ;
		out.p_keep=NO ;
	}
}
