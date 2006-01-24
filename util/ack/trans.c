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

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
static char rcs_trans[] = RCS_TRANS ;
#endif

/****************************************************************************/
/*      Routines for transforming from one file type to another             */
/****************************************************************************/

static growstring head ;
static int        touch_head= NO ;
static growstring tail ;
static int        touch_tail= NO ;

char *headvar(),*tailvar() ;

int transform(phase) register trf *phase ; {
	int ok ;

	if ( !setfiles(phase) ) {
		disc_files(phase) ;
		return 0 ;
	}
	getcallargs(phase) ;
	ok= runphase(phase) ;
	if ( !ok ) rmfile(&out) ;
	/* Free the space occupied by the arguments,
	   except for the linker, since we are bound to exit soon
	   and do not foresee further need of memory space */
	if ( !phase->t_linker ) discardargs(phase) ;
	disc_files(phase) ;
	return ok ;
}

getmapflags(phase) register trf *phase ; {
	register path *l_in ;
	register list_elem *elem ;
	int scanned ;
	register char *ptr ;

	scanlist(l_first(flags),elem) {
		scanned= *(l_content(*elem))&NO_SCAN ;
		*(l_content(*elem)) &= ~NO_SCAN ;
		if ( mapflag(&(phase->t_mapf),l_content(*elem)) ) {
			scanned=NO_SCAN ;
#ifdef DEBUG
			if ( debug >=4 ) {
				vprint("phase %s, added mapflag for %s\n",
					phase->t_name,
					l_content(*elem) ) ;
			}
#endif
		}
		*(l_content(*elem)) |= scanned ;
	}
	if ( phase->t_linker ) {
		scanlist(l_first(phase->t_inputs),elem) {
			l_in = p_cont(*elem) ;
			if ( mapflag(&(phase->t_mapf),l_in->p_path) ) {
				ptr= keeps(getvar(LIBVAR)) ;
				clr_noscan(ptr) ;
#ifdef DEBUG
				if ( debug >=4 ) {
					vprint("phase %s, library %s(%s)\n",
					   phase->t_name,l_in->p_path,ptr) ;
				}
#endif
				if ( l_in->p_keeps) throws(l_in->p_path) ;
				l_in->p_path= ptr ;
				l_in->p_keeps=YES ;
			}
		}
		scanlist(l_first(flags),elem) {
			/* Get the flags remaining for the loader,
			   That is: all the flags neither eaten by ack nor
			   one of the subprograms called so-far.
			   The last fact is indicated by the NO_SCAN bit
			   in the first character of the flag.
			*/
			if ( !( *(l_content(*elem))&NO_SCAN ) ) {
				l_add(&(phase->t_flags),l_content(*elem)) ;
#ifdef DEBUG
				if ( debug >=4 ) {
					vprint("phase %s, added flag %s\n",
						phase->t_name,
						l_content(*elem) ) ;
				}
#endif
			}
		}
	}
}


do_Rflag(argp) char *argp ; {
	l_add(&R_list,argp) ;
}

char *headvar() {
	if ( !touch_head) return "" ;
	return gr_start(head) ;
}

add_head(str) char *str; {
	if ( !touch_head) {
		gr_init(&head) ;
		touch_head=YES ;
	}
	gr_cat(&head,str) ;
}

char *tailvar() {
	if ( !touch_tail ) return "" ;
	return gr_start(tail) ;
}

add_tail(str) char *str ; {
	if ( !touch_tail ) {
		gr_init(&tail) ;
		touch_tail=YES ;
	}
	gr_cat(&tail,str) ;
}


transini() {
	register list_elem *elem ;
	register trf *phase ;

	scanlist(l_first(tr_list), elem) {
		phase = t_cont(*elem) ;
		if ( !phase->t_linker ) getmapflags(phase);
	}
	scanlist(l_first(R_list), elem) {
		set_Rflag(l_content(*elem)) ;
	}
	l_clear(&R_list) ;
	setpvar(keeps(HEAD),headvar) ;
	setpvar(keeps(TAIL),tailvar) ;
}

set_Rflag(argp) register char *argp ; {
	register char *eos ;
	register list_elem *prog ;
	register int length ;
	char *eq, *colon ;

	eos= strindex(&argp[2],'-');
	eq= strindex(&argp[2],EQUAL) ;
	colon= strindex(&argp[2],':');
	if ( !eos ) {
		eos= eq ;
	} else {
		if ( eq && eq<eos ) eos= eq ;
	}
	if ( colon && ( !eos || eos>colon ) ) eos= colon ;
	if ( !eos ) {
		if ( !(argp[0]&NO_SCAN) ) werror("Incorrect use of -R flag") ;
		return ;
	}
	length= eos - &argp[2] ;
	scanlist(l_first(tr_list), prog) {
		if ( strncmp(t_cont(*prog)->t_name, &argp[2], length )==0 &&
		     t_cont(*prog)->t_name[length]==0  /* Same name length */) {
			if ( *eos=='-' ) {
				if ( !(argp[0]&NO_SCAN) ) {
					/* If not already taken by a mapflag */
					l_add(&(t_cont(*prog)->t_flags),eos) ;
				}
			} else
			if ( *eos=='=' ) {
				t_cont(*prog)->t_prog= eos+1 ;
			} else {
				t_cont(*prog)->t_priority= atoi(eos+1) ;
			}
			argp[0] |= NO_SCAN ;
			return ;
		}
	}
	if ( !(argp[0]&NO_SCAN) ) werror("Cannot find program for %s",argp) ;
	return ;
}

/**************************************************************************/
/*                                                                        */
/*           The creation of arguments for exec for a transformation      */
/*                                                                        */
/**************************************************************************/

growstring scanb(line) char *line ; {
	/* Scan a line for backslashes, setting the NO_SCAN bit in characters
	   preceded by a backslash.
	*/
	register char *in_c ;
	register int  token ;
	growstring result ;
	enum { TEXT, ESCAPED } state = TEXT ;

	gr_init(&result) ;
	for ( in_c= line ; *in_c ; in_c++ ) {
		token= *in_c&0377 ;
		switch( state ) {
		case TEXT :
			if ( token==BSLASH ) {
				state= ESCAPED ;
			} else {
				gr_add(&result,token) ;
			}
			break ;
		case ESCAPED :
			gr_add(&result,token|NO_SCAN) ;
			state=TEXT ;
			break ;
		}
	}
	gr_add(&result,0) ;
	if ( state!=TEXT ) werror("flag line ends with %c",BSLASH) ;
	return result ;
}

growstring scanvars(line) char *line ; {
	/* Scan a line variable replacements started by S_VAR.
	   Two sequences exist: S_VAR name E_VAR, S_VAR name A_VAR text E_VAR.
	   neither name nor text may contain further replacements.
	   In the first form an error message is issued if the name is not
	   present in the variables, the second form produces text
	   in that case.
	   The sequence S_VAR S_VAR is transformed into S_VAR.
	   This to allow later recognition in mapflags, where B_SLASH
	   would be preventing any recognition.
	*/
	register char *in_c ;
	register int  token ;
	growstring result ;
	growstring name ;
	register char *tr ;
	enum { TEXT, FIRST, NAME, SKIP, COPY } state = TEXT ;

	gr_init(&result) ; gr_init(&name) ;
	for ( in_c= line ; *in_c ; in_c++ ) {
		token= *in_c&0377 ;
		switch( state ) {
		case TEXT :
			if ( token==S_VAR ) {
				state= FIRST ;
			} else {
				gr_add(&result,token) ;
			}
			break ;
		case FIRST :
			switch ( token ) {
			case S_VAR :
				state= TEXT ;
				gr_add(&result,token) ;
				break ;
			case A_VAR :
			case C_VAR :
				fatal("empty string variable name") ;
			default :
				state=NAME ;
				gr_add(&name,token) ;
				break ;
			}
			break ;
		case NAME:
			switch ( token ) {
			case A_VAR :
				gr_add(&name,0) ;
				if ( tr=getvar(gr_start(name)) ) {
					while ( *tr ) {
						gr_add(&result,*tr++) ;
					}
					state=SKIP ;
				} else {
					state=COPY ;
				}
				gr_throw(&name) ;
				break ;
			case C_VAR :
				gr_add(&name,0) ;
				if ( tr=getvar(gr_start(name)) ) {
					while ( *tr ) {
					   gr_add(&result,*tr++);
					}
				} else {
					werror("No definition for %s",
						gr_start(name)) ;
				}
				state=TEXT ;
				gr_throw(&name) ;
				break ;
			default:
				gr_add(&name,token) ;
				break ;
			}
			break ;
		case SKIP :
			if ( token==C_VAR ) state= TEXT ;
			break ;
		case COPY :
			if ( token==C_VAR ) state= TEXT ; else {
				gr_add(&result,token) ;
			}
			break ;
		}
	}
	gr_add(&result,0) ;
	if ( state!=TEXT ) {
		werror("flag line misses %c",C_VAR) ;
		gr_throw(&name) ;
	}
	return result ;
}

growstring scanexpr(line) char *line ; {
	/* Scan a line for conditional or flag expressions,
	   dependent on the type. The format is
	   S_EXPR suflist M_EXPR suflist T_EXPR tail C_EXPR
	   the head and tail are passed to treat, together with the
	   growstring for futher treatment.
	   Nesting is not allowed.
	*/
	register char *in_c ;
	char *heads ;
	register int  token ;
	growstring sufs, tailval ;
	growstring result ;
	static list_head fsuff, lsuff ;
	enum { TEXT, FDOT, FSUF, LDOT, LSUF, FTAIL } state = TEXT ;

	gr_init(&result) ; gr_init(&sufs) ; gr_init(&tailval) ;
	for ( in_c= line ; *in_c ; in_c++ ) {
		token= *in_c&0377 ;
		switch( state ) {
		case TEXT :
			if ( token==S_EXPR ) {
				state= FDOT ;
				heads=in_c ;
			} else gr_add(&result,token) ;
			break ;
		case FDOT :
			if ( token==M_EXPR ) {
				state=LDOT ;
				break ;
			}
			token &= ~NO_SCAN ;
			if ( token!=SUFCHAR ) {
				error("Missing %c in expression",SUFCHAR) ;
			}
			gr_add(&sufs,token) ; state=FSUF ;
			break ;
		case FSUF :
			if ( token==M_EXPR || (token&~NO_SCAN)==SUFCHAR) {
				gr_add(&sufs,0) ;
				l_add(&fsuff,gr_final(&sufs)) ;
			}
			if ( token==M_EXPR ) {
				state=LDOT ;
			} else gr_add(&sufs,token&~NO_SCAN) ;
			break ;
		case LDOT :
			if ( token==T_EXPR ) {
				state=FTAIL ;
				break ;
			}
			token &= ~NO_SCAN ;
			if ( token!=SUFCHAR ) {
				error("Missing %c in expression",SUFCHAR) ;
			}
			gr_add(&sufs,token) ; state=LSUF ;
			break ;
		case LSUF :
			if ( token==T_EXPR || (token&~NO_SCAN)==SUFCHAR) {
				gr_add(&sufs,0) ;
				l_add(&lsuff,gr_final(&sufs)) ;
			}
			if ( token==T_EXPR ) {
				state=FTAIL ;
			} else gr_add(&sufs,token&~NO_SCAN) ;
			break ;
		case FTAIL :
			if ( token==C_EXPR ) {
				/* Found one !! */
				gr_add(&tailval,0) ;
				condit(&result,&fsuff,&lsuff,gr_start(tailval)) ;
				l_throw(&fsuff) ; l_throw(&lsuff) ;
				gr_throw(&tailval) ;
				state=TEXT ;
			} else gr_add(&tailval,token) ;
			break ;
		}
	}
	gr_add(&result,0) ;
	if ( state!=TEXT ) {
		l_throw(&fsuff) ; l_throw(&lsuff) ; gr_throw(&tailval) ;
		werror("flag line has unclosed expression starting with %6s",
			heads) ;
	}
	return result ;
}

condit(line,fsuff,lsuff,tailval) growstring *line ;
	list_head *fsuff, *lsuff;
	char *tailval ;
{
	register list_elem *first ;
	register list_elem *last ;

#ifdef DEBUG
	if ( debug>=4 ) vprint("Conditional for %s, ",tailval) ;
#endif
	scanlist( l_first(*fsuff), first ) {
		scanlist( l_first(*lsuff), last ) {
			if ( strcmp(l_content(*first),l_content(*last))==0 ) {
				/* Found */
#ifdef DEBUG
				if ( debug>=4 ) vprint(" matched\n") ;
#endif
				while ( *tailval) gr_add(line,*tailval++ ) ;
				return ;
			}
		}
	}
#ifdef DEBUG
	if ( debug>=4) vprint(" non-matched\n") ;
#endif
}

int mapflag(maplist,cflag) list_head *maplist ; char *cflag ; {
	/* Expand a flag expression */
	/* The flag "cflag" is checked for each of the mapflags.
	   A mapflag entry has the form
		-text NAME=replacement or -text*text NAME=replacement
	   The star matches anything as in the shell.
	   If the entry matches the assignment will take place
	   This replacement is subjected to argument matching only.
	   When a match took place the replacement is returned
	   when not, (char *)0.
	   The replacement sits in stable storage.
	*/
	register list_elem *elem ;

	scanlist(l_first(*maplist),elem) {
		if ( mapexpand(l_content(*elem),cflag) ) {
			return 1 ;
		}
	}
	return 0 ;
}

int mapexpand(mapentry,cflag)
	char *mapentry, *cflag ;
{
	register char *star ;
	register char *ptr ;
	register char *space ;
	int length ;

	star=strindex(mapentry,STAR) ;
	space=firstblank(mapentry) ;
	if ( star >space ) star= (char *)0 ;
	if ( star ) {
		length= space-star-1 ;
		if ( strncmp(mapentry,cflag,star-mapentry) ||
		     strncmp(star+1,cflag+strlen(cflag)-length,length) ) {
			return 0 ;
		}
		/* Match */
		/* Now set star to the first char of the star
		   replacement and length to its length
		*/
		length=strlen(cflag)-(star-mapentry)-length ;
		if ( length<0 ) return 0 ;
		star=cflag+(star-mapentry) ;
#ifdef DEBUG
		if ( debug>=6 ) {
			vprint("Starmatch (%s,%s) %.*s\n",
				mapentry,cflag,length,star) ;
		}
#endif
	} else {
		if ( strncmp(mapentry,cflag,space-mapentry)!=0 ||
		     cflag[space-mapentry] ) {
			return 0 ;
		}
	}
	ptr= skipblank(space) ;
	if ( *ptr==0 ) return 1 ;
	doassign(ptr,star,length) ;
	return 1 ;
}

doassign(line,star,length) char *line, *star ; {
	growstring varval, name, temp ;
	register char *ptr ;

	gr_init(&varval) ;
	gr_init(&name) ;
	ptr= line ;
	for ( ; *ptr && *ptr!=SPACE && *ptr!=TAB && *ptr!=EQUAL ; ptr++ ) {
		gr_add(&name,*ptr) ;
	}
	ptr= strindex(ptr,EQUAL) ;
	if ( !ptr ) {
		error("Missing %c in assignment %s",EQUAL,line);
		return ;
	}
	temp= scanvars(ptr+1) ;
	for ( ptr=gr_start(temp); *ptr; ptr++ ) switch ( *ptr ) {
	case STAR :
		if ( star ) {
			while ( length-- ) gr_add(&varval,*star++|NO_SCAN) ;
			break ;
		}
	default :
		gr_add(&varval,*ptr) ;
		break ;
	}
	gr_throw(&temp) ;
	gr_add(&name,0) ; gr_add(&varval,0) ;
	setsvar(gr_final(&name),gr_final(&varval)) ;
}

#define ISBLANK(c) ( (c)==SPACE || (c)==TAB )

unravel(line,action) char *line ; int (*action)() ; {
	/* Unravel the line, get arguments a la shell */
	/* each argument is handled to action */
	/* The input string is left intact */
	register char *in_c ;
	register int  token ;
	enum { BLANK, ARG } state = BLANK ;
	growstring argum ;

	in_c=line ;
	for (;;) {
		token= *in_c&0377 ;
		switch ( state ) {
		case BLANK :
			if ( token==0 ) break ;
			if ( !ISBLANK(token) ) {
				state= ARG ;
				gr_init(&argum) ;
				gr_add(&argum,token&~NO_SCAN) ;
			}
			break ;
		case ARG :
			if ( ISBLANK(token) || token==0 ) {
				gr_add(&argum,0) ;
				(*action)(gr_start(argum)) ;
				gr_throw(&argum) ;
				state=BLANK ;
			} else {
				gr_add(&argum,token&~NO_SCAN) ;
			}
			break ;
		}
		if ( token == 0 ) break ;
		in_c++ ;
	}
}

char *c_rep(string,place,rep) char *string, *place, *rep ; {
	/* Produce a string in stable storage produced from 'string'
	   with the character at place replaced by rep
	*/
	growstring name ;
	register char *nc ;
	register char *xc ;

	gr_init(&name) ;
	for ( nc=string ; *nc && nc<place ; nc++ ) {
		gr_add(&name,*nc) ;
	}
#ifdef DEBUG
	if ( *nc==0 ) fatal("Place is not in string") ;
#endif
	for ( xc=rep ; *xc ; xc++ ) gr_add(&name,*xc|NO_SCAN) ;
	gr_add(&name,0) ;
	gr_cat(&name,nc+1) ;
	return gr_final(&name) ;
}

static list_head *curargs ;
static list_head *comb_args ;

addargs(string) char *string ; {
	register char *temp, *repc ;
	register list_elem *elem ;

	repc=strindex(string,C_IN) ;
	if ( repc ) {
		/* INPUT FILE TOKEN seen, replace it and scan further */
		if ( repc==string && string[1]==0 ) {
			if ( in.p_path ) { /* All but combiner */
				l_add(curargs,keeps(in.p_path)) ;
			} else {
				scanlist( l_first(*comb_args), elem ) {
					l_add(curargs,
					      keeps(p_cont(*elem)->p_path)) ;
				}
			}
			return ;
		}
		if ( in.p_path ) { /* Not for the combiners */
			temp=c_rep(string,repc,in.p_path) ;
			addargs(temp) ;
			throws(temp) ;
		} else {           /* For the combiners */
			scanlist( l_first(*comb_args), elem ) {
				temp=c_rep(string,repc,p_cont(*elem)->p_path);
				addargs(temp) ;
				throws(temp) ;
			}
		}
		return ;
	}
	repc=strindex(string,C_OUT) ;
	if ( repc ) {
		/* replace the outfile token as with the infile token */
#ifdef DEBUG
		if ( !out.p_path ) fatal("missing output filename") ;
#endif
		temp=c_rep(string,repc,out.p_path) ;
		addargs(temp) ;
		throws(temp) ;
		return ;
	}
	temp= keeps(string) ;
	clr_noscan(temp) ;
	l_add(curargs,temp) ;
}

getcallargs(phase) register trf *phase ; {
	growstring arg1, arg2 ;

	arg1= scanvars(phase->t_argd) ;
#ifdef DEBUG
	if ( debug>=3 ) { vprint("\tvars: ") ; prns(gr_start(arg1)) ; }
#endif
	arg2= scanexpr(gr_start(arg1)) ;
#ifdef DEBUG
	if ( debug>=3 ) { vprint("\texpr: ") ; prns(gr_start(arg2)) ; }
#endif
	gr_throw(&arg1) ;
	curargs= &phase->t_args ;
	if (phase->t_combine) comb_args = &phase->t_inputs ;
	unravel( gr_start(arg2), addargs ) ;
	gr_throw(&arg2) ;
}

discardargs(phase) register trf *phase ; {
	l_throw(&phase->t_args) ;
}
