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
#include "grows.h"
#include "data.h"
#include <em_path.h>

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

static char *add_u(int part, char *ptr) {
	if ( part>=26 ) {
		ptr=add_u(part/26-1,ptr) ;
	}
	*ptr= part%26 + 'a' ;
	return ptr+1 ;
}

static char *unique(void) {
	/* Get the next unique part of the internal filename */
	static int u_next = 0 ;
	static char buf[10] ;
	register char *ptr ;

	ptr=add_u(u_next,buf) ;
	*ptr=0 ;
	u_next++ ;
	return buf ;
}

int setfiles(trf *phase) {
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
	if ( phase->t_outfile && phase->t_keep ) {
		out.p_path=phase->t_outfile ;
		out.p_keeps=NO ;
		out.p_keep=YES ;
	} else {
		gr_init(&pathname) ;
		if ( !phase->t_keep && !t_flag ) {
			gr_cat(&pathname,TMP_DIR) ;
			gr_cat(&pathname,"/") ;
			gr_cat(&pathname,template) ;
			gr_cat(&pathname,unique()) ;
			out.p_keep=NO ;
		} else {
			if ( !p_basename ) {
				gr_cat(&pathname,"Ack") ;
				gr_cat(&pathname,unique()) ;
				p_basename=keeps(gr_start(pathname)) ;
				werror("Output written on %s%s",
					p_basename,phase->t_out) ;
			} else {
				gr_cat(&pathname,p_basename) ;
			}
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

static void file_final(path *file) {
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

void disc_files(trf *phase) {
	path temp ;

	if ( !phase->t_combine ) {
		file_final(&in) ;
	} else {
		disc_inputs(phase) ;
	}
	temp=in ; in=out ; out=temp ;
}

void disc_inputs(trf *phase) {
	/* Remove all the input files of this phase */
	/* Only for combiners */
	register path *l_in ;
	register list_elem *elem ;
	scanlist( l_first(phase->t_inputs), elem) {
		l_in= p_cont(*elem) ;
		file_final(l_in) ;
		freecore((char *)l_in) ;
	}
	l_clear(&phase->t_inputs) ;
}

void rmfile(path *file) {
	/* Remove a file, do not complain when is does not exist */
	if ( file->p_path ) {
		if ( t_flag<=1 ) unlink(file->p_path) ;
		if ( file->p_keeps ) throws(file->p_path) ;
		file->p_path= (char *)0 ;
		file->p_keeps=NO ;
		file->p_keep=NO ;
	}
}

void rmtemps(void) {
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

void add_input(path *file, trf *phase) {
	register path *store ;
#ifdef DEBUG
	if ( debug ) {
		vprint("Adding %s to inputs of %s\n",
			file->p_path,phase->t_name) ;
	}
#endif
	phase->t_do=YES ;
	if ( !phase->t_origname && orig.p_path[0]!='-' ) {
		/* This entry decides the name of the result */
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
