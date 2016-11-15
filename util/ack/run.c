/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ack.h"
#include "list.h"
#include "trans.h"
#include "grows.h"
#include "data.h"
#include <signal.h>

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

#define ARG_MORE  40            /* The size of args chunks to allocate */

static int run_exec(trf *, const char *);
static void x_arg(char *);

static char      **arglist ;    /* The first argument */
static unsigned  argcount ;     /* The current number of arguments */
static unsigned  argmax;        /* The maximum number of arguments so far */

int runphase(trf *phase) {
	register list_elem *elem ;
	char *prog ; int result ;

	prog=phase->t_prog ;
	if ( v_flag || debug ) {
		if ( v_flag==1 && !debug ) {
			vprint("%s",phase->t_name) ;
			if ( !phase->t_combine ) {
				vprint(" %s%s\n",p_basename,
					strrchr(in.p_path,SUFCHAR) ) ;
			} else {
				scanlist(l_first(phase->t_inputs), elem) {
					vprint(" %s",p_cont(*elem)->p_path);
				}
				vprint("\n") ;
			}
		} else {
			/* list all args */
                      vprint("%s",prog) ;
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
	result=run_exec(phase,prog) ;
	return result ;
}

static int run_exec(trf *phase, const char *prog) {
	int status, child, waitchild ;

	fflush(stdout) ;
	fflush(stderr) ;
	child= fork() ;
	if ( child== - 1) {
		fatal("Cannot fork %s", prog) ;
	}
	if ( child ) {
		/* The parent */
		do {
			waitchild= wait(&status) ;
			if ( waitchild== -1 ) {
				fatal("missing child") ;
			}
		} while ( waitchild!=child) ;
		if ( status ) {
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
                                      prog,status&0177) ;
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
			error("cannot create %s",out.p_path) ;
			exit(1) ;
		}
	}
      execv(prog,arglist) ;
	if ( phase->t_stdout ) { close(1) ; dup(2) ; }
      error("Cannot execute %s",prog) ;
	exit(1) ;
	/*NOTREACHED*/
}

static void x_arg(char *string) {
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
