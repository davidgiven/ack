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
#include "../../h/em_path.h"
#include "list.h"
#include "trans.h"
#include "grows.h"
#include "dmach.h"
#include "data.h"
#include <stdio.h>

/************************************************************************/
/*                                                                      */
/*           Read machine definitions and transformations               */
/*                                                                      */
/************************************************************************/

#define COMMENT '#'

#define VAR     "var"
#define PASS    "name"
#define IN      "from"
#define OUT     "to"
#define PROG    "program"
#define MAPF    "mapflag"
#define ARGS    "args"
#define PROP    "prop"
#define RUNT    "rts"
#define NEEDT	"need"
#define END     "end"

extern growstring scanb();
extern growstring scanvars();

int getline() ;
int getinchar() ;
static char *ty_name ;
static char *bol ;


static char *inname ;

setlist(name) char *name ; {
	/* Name is sought in the internal tables,
	   if not present, the a file of that name is sought
	   in first the current and then the EM Lib directory
	*/

	inname=name ;
	open_in(name) ;
	while ( getline() ) {
		if ( strcmp(VAR,ty_name)==0 ) {
			doassign(bol,(char *)0,0) ;
		} else
		if ( strcmp(PASS,ty_name)==0 ) {
			intrf() ;
		} else
			error("unknown keyword %s",ty_name) ;
	}
	close_in();
#ifdef DEBUG
	if ( debug>=3 ) vprint("End %s\n",name) ;
#endif
}

intrf() {
	register trf *new ;
	register char *ptr ;
	growstring bline, vline ;
	int twice ;

	new= (trf *)getcore(sizeof *new) ;
	new->t_name= keeps(bol) ;
	for (;;) {
		if ( !getline() ) {
			fuerror("unexpected EOF on %s",inname) ;
		}
		twice= NO ;
		if ( strcmp(ty_name,IN)==0 ) {
			if ( new->t_in ) twice=YES ;
			new->t_in= keeps(bol);
		} else
		if ( strcmp(ty_name,OUT)==0 ) {
			if ( new->t_out ) twice=YES ;
			new->t_out= keeps(bol);
		} else
		if ( strcmp(ty_name,PROG)==0 ) {
			if ( new->t_prog ) twice=YES ;
			bline= scanb(bol);                /* Scan for \ */
			vline= scanvars(gr_start(bline)); /* Scan for {} */
			gr_throw(&bline);
			new->t_prog= gr_final(&vline);
			clr_noscan(new->t_prog);
		} else
		if ( strcmp(ty_name,MAPF)==0 ) {
			/* First read the mapflags line
				and scan for backslashes */
			bline= scanb(bol) ;
			l_add(&new->t_mapf,gr_final(&bline)) ;
		} else
		if ( strcmp(ty_name,ARGS)==0 ) {
			if ( new->t_argd ) twice=YES ;
			bline= scanb(bol) ;
			new->t_argd= keeps(gr_start(bline)) ;
			gr_throw(&bline) ;
		} else
		if ( strcmp(ty_name,PROP)==0 ) {
			for ( ptr=bol ; *ptr ; ptr++ ) {
				switch( *ptr ) {
				case C_IN: new->t_stdin= YES ; break ;
				case C_OUT: new->t_stdout= YES ; break ;
				case 'P': new->t_isprep= YES ; break ;
				case 'p': new->t_prep= YES ; break ;
				case 'm': new->t_prep= MAYBE ; break ;
				case 'O': new->t_optim= YES ; break ;
				case 'C': new->t_combine= YES ; break ;
				default :
				  error("Unkown option %c in %s for %s",
					*ptr,new->t_name,inname) ;
				  break ;
				}
			}
		} else
		if ( strcmp(ty_name,RUNT)==0 ) {
			if ( new->t_rts ) twice=YES ;
			new->t_rts= keeps(bol) ;
		} else
		if ( strcmp(ty_name,NEEDT)==0 ) {
			if ( new->t_needed ) twice=YES ;
			new->t_needed= keeps(bol) ;
		} else
		if ( strcmp(ty_name,END)==0 ) {
			break ;
		} else {
			fuerror("illegal keyword %s %s",ty_name,bol);
		}
		if ( twice ) {
			werror("%s: specified twice for %s",
				ty_name, new->t_name) ;
		}
	}
	if ( ! ( new->t_name && new->t_out && new->t_prog ) ) {
		fuerror("insufficient specification for %s in %s",
			new->t_name,inname) ;
	}
	if ( ! new->t_argd ) new->t_argd="" ;
#ifdef DEBUG
	if ( debug>=3 ) {
		register list_elem *elem ;
		vprint("%s: from %s to %s '%s'\n",
			new->t_name,new->t_in,new->t_out,new->t_prog) ;
		vprint("\targs: ") ; prns(new->t_argd) ;
		scanlist( l_first(new->t_mapf), elem ) {
			vprint("\t%s\n",l_content(*elem)) ;
		}
		if ( new->t_rts ) vprint("\trts: %s\n",new->t_rts) ;
		if ( new->t_needed ) vprint("\tneeded: %s\n",new->t_needed) ;
	}
#endif
	l_add(&tr_list,(char *)new) ;
}

/************************** IO from core or file *******************/

static  int             incore ;
static  growstring      rline ;
static  FILE            *infile ;
static  char            *inptr ;

open_in(name) register char *name ; {
	register dmach *cmac ;

	gr_init(&rline) ;
	for ( cmac= massoc ; cmac->ma_index!= -1 ; cmac++ ) {
		if ( strcmp(name,cmac->ma_name)==0 ) {
			incore=YES ;
			inptr= &intable[cmac->ma_index] ;
			return ;
		}
	}
	/* Not in core */
	incore= NO ;
#ifdef NEW
	gr_cat(&rline,EM_DIR) ;
	gr_cat(&rline,"/lib/n_ack/") ;
#else
	gr_cat(&rline,ACK_DIR); gr_cat(&rline,"/") ;
#endif
	gr_cat(&rline,name) ;
	infile= fopen(gr_start(rline),"r") ;
#ifdef NEW
	if ( !infile ) {
		/* Try to read EM_DIR/lib/MACH/plan */
		gr_throw(&rline) ;
		gr_cat(&rline,EM_DIR) ;
		gr_cat(&rline,"/lib/") ; gr_cat(&rline,name) ;
		gr_cat(&rline,"/plan") ;
		infile= fopen(gr_start(rline),"r") ;
	}
#endif
	if ( !infile ) {
		infile= fopen(name,"r") ;
	}
	if ( infile==NULL ) {
		fuerror("Cannot find description for %s",name) ;
	}
}

close_in() {
	if ( !incore ) fclose(infile) ;
	gr_throw(&rline) ;
}

char *readline() {
	/* Get a line from the input,
	   return 0 if at end,
	   The line is stored in a volatile buffer,
	   a pointer to the line is returned.
	*/
	register int nchar ;
	enum { BOL, ESCAPE, SKIPPING, MOL } state = BOL ;

	gr_throw(&rline) ;
	for (;;) {
		nchar= getinchar() ;
		if ( nchar==EOF ) {
			if ( state!=BOL ) {
				werror("incomplete line in %s", inname) ;
			}
			return 0 ;
		}
		if ( state==SKIPPING ) {
			if ( nchar=='\n' ) {
				state= MOL ;
			} else {
				continue ;
			}
		}
		if ( state==ESCAPE ) {
			switch( nchar ) {
			case '\n' :
				break ;
			default :
				gr_add(&rline,BSLASH) ;
			case COMMENT :
			case BSLASH :
				gr_add(&rline,nchar) ;
				break ;
			}
			state= MOL ;
			continue ;
		}
		switch ( nchar ) {
		case '\n' :     gr_add(&rline,0) ;
				return gr_start(rline) ;
		case COMMENT :  state= SKIPPING ;
				break ;
		case BSLASH :   state= ESCAPE ;
				break ;
		default :       gr_add(&rline,nchar) ;
				state= MOL ;
		}
	}
}

int getinchar() {
	if ( incore ) {
		if ( *inptr==0 ) return EOF ;
		return *inptr++ ;
	}
	return getc(infile) ;
}

int getline() {
	register char *c_ptr ;

	do {
		if ( (c_ptr=readline())==(char *)0 ) return 0 ;
		ty_name= skipblank(c_ptr) ;
	} while ( *ty_name==0 ) ;
	c_ptr= firstblank(ty_name) ;
	if ( *c_ptr ) {
		*c_ptr++ =0 ;
		c_ptr= skipblank(c_ptr) ;
	}
	bol= c_ptr ;
	return 1 ;
}
