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
#include "../../h/em_path.h"
#include "../../h/local.h"
#include "data.h"
#include <signal.h>

static int sigs[] = { SIGINT, SIGHUP, SIGTERM, 0 } ;

extern  char    *getenv();

main(argc,argv) char **argv ; {
	register list_elem *elem ;
	register char *frontend ;
	register int *n_sig ;

	progname=argv[0];
	varinit();
	vieuwargs(argc,argv);
	if ( (frontend=getenv("ACKFE")) ) {
		setlist(frontend) ;
	} else {
		setlist(FRONTENDS);
	}
	setlist(machine);
	transini();
	scanneeds();
	template= mktemp(ACKNAME) ;
	if ( n_error && !k_flag ) return n_error ;

	for ( n_sig=sigs ; *n_sig ; n_sig++ ) {
		if ( signal(*n_sig,noodstop)==SIG_IGN ) {
			signal(*n_sig,SIG_IGN) ;
		}
	}
	scanlist ( l_first(arguments), elem ) {
		if ( !process(l_content(*elem)) && !k_flag ) return 1 ;
	}
	orig.p_path= (char *)0 ;

	if ( !combiner && !stopsuffix ) {
		/* Call combiner directly without any transformation */
		scanlist(l_first(tr_list),elem) {
			if ( t_cont(*elem)->t_combine ) {
				combiner= t_cont(*elem) ;
			}
		}
	}

	if ( !combiner || n_error ) return n_error ;

	if ( !do_combine() ) return 1 ;

	if ( g_flag ) {
		return do_run();
	}

	return 0 ;
}

char *srcvar() {
	return orig.p_path ;
}

varinit() {
	/* initialize the string variables */
	setsvar(keeps(HOME),keeps(EM_DIR)) ;
	setpvar(keeps(SRC),srcvar)  ;
}

/************************* flag processing ***********************/

vieuwargs(argc,argv) char **argv ; {
	register char *argp;
	register int nextarg ;
	register int eaten ;

	firstarg(argv[0]) ;

	nextarg= 1 ;

	while ( nextarg<argc ) {
		argp= argv[nextarg] ;
		nextarg++ ;
		if ( argp[0]!='-' || argp[1]=='l' ) {
			/* Not a flag, or a library */
			l_add(&arguments,argp) ;
			continue ;
		}

		/* Flags */
		eaten=0 ; /* Did not 'eat' tail of flag yet */
		switch ( argp[1] ) {
	   case 'm':    if ( machine ) fuerror("Two machines?") ;
			machine= &argp[2];
			eaten=1 ;
			break ;
	   case 'o':    if ( nextarg>=argc ) {
				fuerror("-o can't be the last flag") ;
			}
			if ( outfile ) fuerror("Two results?") ;
			outfile= argv[nextarg++] ;
			break ;
	   case 'O':    Optflag++ ;
			break ;
	   case 'v':    v_flag++ ;
			break ;
	   case 'g':    g_flag++ ;
			break ;
	   case 'c':    if ( stopsuffix ) fuerror("Two -c flags") ;
			stopsuffix= &argp[2]; eaten=1;
			if ( *stopsuffix && *stopsuffix!=SUFCHAR ) {
				fuerror("-c flag has invalid tail") ;
			}
			break ;
	   case 'k':    k_flag++ ;
			break ;
	   case 't':    t_flag++ ;
			break ;
	   case 'R':    do_Rflag(argp); eaten=1;
			break ;
	   case 'r':    if ( argp[2]!=SUFCHAR ) {
				error("-r must be followed by %c",SUFCHAR) ;
			}
			keeptail(&argp[2]); eaten=1 ;
			break ;
	   case '.':    if ( rts ) fuerror("Two run-time systems?") ;
			rts= &argp[1] ; eaten=1;
			keephead(rts) ; keeptail(rts) ;
			break ;
#ifdef DEBUG
	   case 'd':    debug++ ;
			break ;
#endif
	   case  0 :    nill_flag++ ; eaten++ ;
			break;
	   case 'w':    { register char *tokeep ;
			  w_flag++;
			  tokeep=keeps(argp) ;
			  *tokeep |= NO_SCAN ;
			  l_add(&flags,tokeep) ;
			}
			break ;
	   default:     /* The flag is not recognized,
			   put it on the list for the sub-processes
			*/
#ifdef DEBUG
			if ( debug ) {
				vprint("Flag %s: phase dependent\n",argp) ;
			}
#endif
			l_add(&flags,keeps(argp)) ;
			eaten=1 ;
		}
		if ( argp[2] && !eaten ) {
			werror("Unexpected characters at end of %s",argp) ;
		}
	}
	if ( !machine && ! (machine=getenv("ACKM")) ) {
#ifdef ACKM
		machine= ACKM;          /* The default machine */
#else
		fuerror("No machine specified") ;
#endif
	}
	return ;
}

firstarg(argp) register char *argp ; {
	register char *name ;

	name=rindex(argp,'/') ;
	if ( name && *(name+1) ) {
		name++ ;
	} else {
		name= argp ;
	}
	if ( strcmp(name,"ack")==0 ) return ;
	if ( strcmp(name,"acc")==0 || strcmp(name,"cc")==0 ) {
		rts= ".c" ; keephead(rts) ; keeptail(rts) ;
		return ;
	}
	if ( strcmp(name,"apc")==0 || strcmp(name,"pc")==0 ) {
		rts= ".p" ; keephead(rts) ; keeptail(rts) ;
		return ;
	}
	machine= name;
}

/************************* argument processing ***********************/

process(arg) char *arg ; {
	/* Process files & library arguments */
	register list_elem *elem ;
	register trf *phase ;
	int first=YES ;

#ifdef DEBUG
	if ( debug ) vprint("Processing %s\n",arg) ;
#endif
	if ( arg[0]=='-' ) { l_add(&c_arguments,keeps(arg)) ; return 1 ; }
	p_suffix= rindex(arg,SUFCHAR) ;
	if ( p_basename ) throws(p_basename) ;
	orig.p_keep= YES ;      /* Don't throw away the original ! */
	orig.p_path= arg ;
	p_basename= keeps(basename(arg)) ;
	if ( !p_suffix ) { l_add(&c_arguments,keeps(arg)) ; return 1 ; }
	/* Try to find a path through the transformations */
	switch( setpath() ) {
	case F_NOPATH :
		error("Cannot produce the desired file from %s",arg) ;
		l_add(&c_arguments,keeps(arg)) ;
		return 1 ;
	case F_NOMATCH :
		if ( stopsuffix ) werror("Unknown suffix in %s",arg) ;
		l_add(&c_arguments,keeps(arg)) ;
		return 1 ;
	case F_OK :
		break ;
	}
	orig.p_keeps= NO;
	in= orig ;
	scanlist(l_first(tr_list), elem) {
		phase= t_cont(*elem) ;
		if ( phase->t_do ) { /* perform this transformation */
			if ( first ) {
			   if ( !nill_flag ) {
				printf("%s\n",arg) ;
			   }
			}
			switch ( phase->t_prep ) {
			default :    if ( !mayprep() ) break ;
			case YES:    if ( !transform(cpp_trafo) ) {
					   n_error++ ;
#ifdef DEBUG
					   vprint("Pre-processor failed\n") ;
#endif
					   return 0 ;
				     }
			case NO :
				     break ;
			}
			if ( cpp_trafo && stopsuffix &&
			     strcmp(cpp_trafo->t_out,stopsuffix)==0 ) {
				break ;
			}
			if ( !transform(phase) ) {
				n_error++ ;
#ifdef DEBUG
				if ( debug ) {
					vprint("phase %s for %s failed\n",
					       phase->t_name,orig.p_path) ;
				}
#endif
				return 0 ;
			}
			first=NO ;
		}
	}
#ifdef DEBUG
	if ( debug ) vprint("Transformation complete for %s\n",orig.p_path) ;
#endif
	if ( !in.p_keep ) fatal("attempt to discard the result file") ;
	l_add(&c_arguments,keeps(in.p_path));
	disc_files() ;
	return 1 ;
}

mayprep() {
	int file ;
	char fc ;
	file=open(in.p_path,0);
	if ( file<0 ) return 0 ;
	if ( read(file,&fc,1)!=1 ) fc=0 ;
	close(file) ;
	return fc=='#' ;
}

keephead(suffix) char *suffix ; {
	l_add(&head_list, suffix) ;
}

keeptail(suffix) char *suffix ; {
	l_add(&tail_list, suffix) ;
}

scanneeds() {
	register list_elem *elem ;
	scanlist(l_first(head_list), elem) { setneeds(l_content(*elem),0) ; }
	l_clear(&head_list) ;
	scanlist(l_first(tail_list), elem) { setneeds(l_content(*elem),1) ; }
	l_clear(&tail_list) ;
}

setneeds(suffix,tail) char *suffix ; {
	register list_elem *elem ;
	register trf *phase ;

	p_suffix= suffix ;
	switch ( setpath() ) {
	case F_OK :
		scanlist( l_first(tr_list), elem ) {
			phase = t_cont(*elem) ;
			if ( phase->t_do ) {
				if ( phase->t_needed ) {
					if ( tail )
						add_tail(phase->t_needed) ;
					else
						add_head(phase->t_needed) ;
				}
			}
		}
		break ;
	case F_NOMATCH :
		werror("\"%s\": unrecognized suffix",suffix) ;
		break ;
	case F_NOPATH :
		werror("incomplete internal specification for %s files",
			suffix) ;
		break ;
	}
}
