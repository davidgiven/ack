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

#ifndef NORCSID
static char rcs_id[] = "$Header$" ;
#endif

char *add_u(part,ptr) char *ptr ; {
	if ( part>=26 ) {
		ptr=add_u(part/26-1,ptr) ;
	}
	*ptr= part%26 + 'a' ;
	return ptr+1 ;
}

char *unique() {
	/* Get the next unique part of the internal filename */
	static int u_next = 0 ;
	static char buf[10] ;
	register char *ptr ;

	ptr=add_u(u_next,buf) ;
	*ptr=0 ;
	u_next++ ;
	return buf ;
}

setfiles(phase) register trf *phase ; {
	/* Set the out structure according to the in structure,
	   the transformation and some global data */
	growstring pathname ;
	register list_elem *elem ;
	static int out_used= 0 ;

	if ( !phase->t_next && !phase->t_isprep && outfile ) {
		if ( out_used ) {
			fuerror("only one output file allowed when using the -o flag") ;
		} else {
			if ( !phase->t_keep ) fatal("Removing result file") ;
			phase->t_outfile=outfile ;
			out_used++ ;
		}
	}
	if ( phase->t_combine ) {
		in.p_path= (char *)0 ;
		in.p_keep=YES ;
		in.p_keeps=NO ;
	}
	if ( phase->t_outfile ) {
		out.p_path=phase->t_outfile ;
		out.p_keeps=NO ;
	} else {
		gr_init(&pathname) ;
		if ( !phase->t_keep && !t_flag ) {
			gr_cat(&pathname,TMP_DIR) ;
			gr_cat(&pathname,"/") ;
			gr_cat(&pathname,template) ;
			gr_cat(&pathname,unique()) ;
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
			error("attempt to overwrite %s",out.p_path) ;
			return 0 ;
		}
	}
	return 1 ;
}

disc_files(phase) trf *phase ; {
	path temp ;

	if ( !phase->t_combine ) {
		file_final(&in) ;
	} else {
		disc_inputs(phase) ;
	}
	temp=in ; in=out ; out=temp ;
}

file_final(file) path *file ; {
	if ( file->p_path ) {
		if ( !file->p_keep && t_flag<=1 ) {
			if ( unlink(file->p_path)!=0 ) {
				werror("couldn't unlink %s",file->p_path);
			}
		}
		if ( file->p_keeps ) throws(file->p_path) ;
	}
	file->p_path= (char *)0 ;
	file->p_keeps=NO ;
	file->p_keep=NO ;
}

disc_inputs(phase) trf *phase ; {
	/* Remove all the input files of this phase */
	/* Only for combiners */
	register path *l_in ;
	register list_elem *elem ;
	scanlist( l_first(phase->t_inputs), elem) {
		l_in= p_cont(*elem) ;
		file_final(l_in) ;
		freecore(l_in) ;
	}
	l_clear(&phase->t_inputs) ;
}

rmfile(file) path *file ; {
	/* Remove a file, do not complain when is does not exist */
	if ( file->p_path ) {
		if ( t_flag<=1 ) unlink(file->p_path) ;
		if ( file->p_keeps ) throws(file->p_path) ;
		file->p_path= (char *)0 ;
		file->p_keeps=NO ;
		file->p_keep=NO ;
	}
}

rmtemps() {
	/* Called in case of disaster, always remove the current output file!
	*/
	register list_elem *elem ;

	if ( t_flag>1 ) return ;
	rmfile(&out) ;
	file_final(&in) ;
	scanlist(l_first(tr_list),elem) {
		if ( t_cont(*elem)->t_combine && t_cont(*elem)->t_do ) {
			disc_inputs(t_cont(*elem)) ;
		}
	}
}

add_input(file,phase) path *file ; trf *phase ; {
	register path *store ;
#ifdef DEBUG
	if ( debug ) {
		vprint("Adding %s to inputs of %s\n",
			file->p_path,phase->t_name) ;
	}
#endif
	if ( !l_first(phase->t_inputs) ) {
		/* This becomes the first entry in the input list */
		phase->t_origname= orig.p_path ;
	}
	store= (path *) getcore(sizeof (path)) ;
	*store = *file ;
	l_add(&phase->t_inputs,(char *)store) ;
	/* The task of getting rid of the string is passed to 'phase',
	   as is the task to get rid of the file itself.
	*/
	file->p_keeps=NO ; file->p_keep=YES ;
}
