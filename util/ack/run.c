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
#include <signal.h>

#define ARG_MORE  40            /* The size of args chunks to allocate */

static char      **arglist ;    /* The first argument */
static unsigned  argcount ;     /* The current number of arguments */
static unsigned  argmax;        /* The maximum number of arguments so far */

int do_run() {
	fatal("-g flag not implemeted") ;
	/*NOTREACHED*/
	return 0 ;
}

int runphase(phase) register trf *phase ; {
	register list_elem *elem ;

	if ( v_flag || debug ) {
		if ( v_flag==1 && !debug ) {
			vprint("%s",phase->t_name) ;
			if ( !phase->t_combine ) {
				vprint(" %s%s\n",p_basename,
					rindex(in.p_path,SUFCHAR) ) ;
			} else {
				scanlist(l_first(c_arguments), elem) {
					vprint(" %s",l_content(*elem)) ;
				}
				vprint("\n") ;
			}
		} else {
			/* list all args */
			vprint("%s",phase->t_prog) ;
			scanlist(l_first(phase->t_flags), elem) {
				vprint(" %s",l_content(*elem)) ;
			}
			scanlist(l_first(phase->t_args), elem) {
				vprint(" %s",l_content(*elem)) ;
			}
			vprint("\n") ;
		}
	}
	argcount=0 ;
	x_arg(phase->t_name) ;
	scanlist(l_first(phase->t_flags), elem) {
		x_arg(l_content(*elem)) ;
	}
	scanlist(l_first(phase->t_args), elem) {
		x_arg(l_content(*elem)) ;
	}
	x_arg( (char *)0 ) ;
	return run_exec(phase) ;
}

int run_exec(phase) trf *phase ; {
	int status, child, waitchild ;

	do_flush();
	while ( (child=fork())== -1 ) ;
	if ( child ) {
		/* The parent */
		do {
			waitchild= wait(&status) ;
			if ( waitchild== -1 ) {
				fatal("missing child") ;
			}
		} while ( waitchild!=child) ;
		if ( status ) {
			if ( status&0200 && (status&0177)!=SIGQUIT &&
				!t_flag ) unlink("core") ;
			switch ( status&0177 ) {
			case 0 :
				break ;
			case SIGHUP:
			case SIGINT:
			case SIGQUIT:
			case SIGTERM:
				quit(-5) ;
			default:
				error("%s died with signal %d",
					phase->t_prog,status&0177) ;
			}
			/* The assumption is that processes voluntarely
			   dying with a non-zero status already produced
			   some sort of error message to the outside world.
			*/
			n_error++ ;
			return 0 ;
		}
		return 1 ; /* From the parent */
	}
	/* The child */
	if ( phase->t_stdin ) {
		if ( !in.p_path ) {
			fatal("no input file for %s",phase->t_name) ;
		}
		close(0) ;
		if ( open(in.p_path,0)!=0 ) {
			error("cannot open %s",in.p_path) ;
			exit(1) ;
		}
	}
	if ( phase->t_stdout ) {
		if ( !out.p_path ) {
			fatal("no output file for %s",phase->t_name) ;
		}
		close(1) ;
		if ( creat(out.p_path,0666)!=1 ) {
			close(1); dup(2);
			error("cannot open %s",out.p_path) ;
			exit(1) ;
		}
	}
	execv(phase->t_prog,arglist) ;
	if ( phase->t_stdout ) { close(1) ; dup(2) ; }
	error("Cannot execute %s",phase->t_prog) ;
	exit(1) ;
	/*NOTREACHED*/
}

x_arg(string) char *string ; {
	/* Add one execute argument to the argument vector */
	if ( argcount==argmax ) {
		if ( argmax==0 ) {
			argmax= 2*ARG_MORE ;
			arglist= (char **)getcore(argmax*sizeof (char *)) ;
		} else {
			argmax += ARG_MORE ;
			arglist= (char **)changecore((char *)arglist,
					argmax*sizeof (char *)) ;
		}
	}
	*(arglist+argcount++) = string ;
}
