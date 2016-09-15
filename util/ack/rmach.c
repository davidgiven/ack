/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "ack.h"
#include <em_path.h>
#include "list.h"
#include "trans.h"
#include "grows.h"
#include "dmach.h"
#include "data.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
static char rcs_dmach[] = RCS_DMACH ;
#endif

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
#define RES	"outfile"
#define PROG    "program"
#define MAPF    "mapflag"
#define ARGS    "args"
#define STD_IN	"stdin"
#define STD_OUT	"stdout"
#define PREP	"prep"
#define OPT	"optimizer"
#define LINKER	"linker"
#define COMBINER "combiner"
#define PRIO	"priority"
#define RUNT    "rts"
#define NEEDT	"need"
#define CALL	"callname"
#define END     "end"

extern growstring scanb();
extern growstring scanvars();

int getln() ;
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
	while ( getln() ) {
		if ( strcmp(VAR,ty_name)==0 ) {
			doassign(bol,(char *)0,0) ;
		} else
		if ( strcmp(CALL,ty_name)==0 ) {
			if ( callname && strcmp(bol,callname)==0 ) {
				callname= (char *)0 ;
#ifdef DEBUG
				if ( debug>=3 ) {
					vprint("found call name\n");
				}
#endif
			}
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

static int inoptlist(nm)
	char *nm ;
{
	register char *p=Optlist ;

	while ( p && *p ) {
		register char *q=nm ;

		while ( *q!='\0' && *q++==*p ) p++ ;
		if ( *q=='\0' && ( *p=='\0' || *p==',' ) ) return 1 ;
		while ( *p!='\0' && *p++!=',' ) /* nothing */ ;
	}
	return 0;
}

intrf() {
	register trf *new ;
	growstring bline ;
	int twice ;
	int name_seen=0 ;

	new= (trf *)getcore(sizeof *new) ;
	new->t_name= keeps(bol) ;
	for (;;) {
		if ( !getln() ) {
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
                      new->t_prog= gr_final(&bline);
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
		if ( strcmp(ty_name,STD_IN)==0 ) {
			if ( new->t_stdin ) twice=YES ;
			new->t_stdin= YES ;
		} else
		if ( strcmp(ty_name,STD_OUT)==0 ) {
			if ( new->t_stdout ) twice=YES ;
			new->t_stdout= YES ;
		} else
		if ( strcmp(ty_name,PREP)==0 ) {
			if ( strcmp(bol,"always")==0 ) {
				if ( new->t_prep ) twice=YES ;
				new->t_prep=YES ;
			} else
			if ( strcmp(bol,"cond")==0 ) {
				if ( new->t_prep ) twice=YES ;
				new->t_prep=MAYBE ;
			} else
			if ( strcmp(bol,"is")==0 ) {
				if ( new->t_isprep ) twice=YES ;
				new->t_isprep= YES ;
			} else
			{
				fuerror("illegal preprocessor spec in %s: %s",
					inname,bol) ;
			}
		} else
		if ( strcmp(ty_name,OPT)==0 ) {
			if ( new->t_optim ) twice=YES ;
			new->t_optim= atoi(bol) ;
			if (new->t_optim <= 0) new->t_optim = 1;
		} else
		if ( strcmp(ty_name,LINKER)==0 ) {
			if ( new->t_linker ) twice=YES ;
			new->t_linker= YES ;
			new->t_combine= YES ;
		} else
		if ( strcmp(ty_name,COMBINER)==0 ) {
			if ( new->t_combine ) twice=YES ;
			new->t_combine= YES ;
		} else
		if ( strcmp(ty_name,PRIO)==0 ) {
			new->t_priority= atoi(bol) ;
		} else
		if ( strcmp(ty_name,RUNT)==0 ) {
			if ( new->t_rts ) twice=YES ;
			new->t_rts= keeps(bol) ;
		} else
		if ( strcmp(ty_name,NEEDT)==0 ) {
			if ( new->t_needed ) twice=YES ;
			new->t_needed= keeps(bol) ;
		} else
		if ( strcmp(ty_name,RES)==0 ) {
			if ( new->t_outfile ) twice=YES ;
			new->t_outfile= keeps(bol) ;
		} else
		if ( strcmp(ty_name,CALL)==0 ) {
			if ( callname && strcmp(bol,callname)==0 ) {
				name_seen=1 ;
				callname= (char *)0 ;
#ifdef DEBUG
				if ( debug>=3 ) {
					vprint("found call name in %s\n",
						new->t_name) ;
				}
#endif
			}
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
	/* Warning, side effect */
	if ( name_seen && new->t_rts ) {
		if ( rts && strcmp(rts,new->t_rts)!=0 ) {
			error("Attempt to use two run-time systems, %s and %s",
				rts, new->t_rts) ;
		}
		rts= new->t_rts ;
		keephead(rts) ; keeptail(rts) ;
	}
#ifdef DEBUG
	if ( debug>=3 ) {
		register list_elem *elem ;
		vprint("%s: from %s to %s '%s'\n",
			new->t_name,new->t_in ? new->t_in : "(null)",new->t_out,new->t_prog) ;
		vprint("\targs: ") ; prns(new->t_argd) ;
		scanlist( l_first(new->t_mapf), elem ) {
			vprint("\t%s\n",l_content(*elem)) ;
		}
		if ( new->t_rts ) vprint("\trts: %s\n",new->t_rts) ;
		if ( new->t_needed ) vprint("\tneeded: %s\n",new->t_needed) ;
	}
#endif
	if ( new->t_optim && 
	     ( new->t_optim <= Optlevel || inoptlist(new->t_name) ) ) {
		new->t_optim = Optlevel;
	}
	l_add(&tr_list,(char *)new) ;
}

/************************** IO from core or file *******************/

static  int             incore ;
static  growstring      rline ;
static  FILE            *infile ;
static  char            *inptr ;
char			*em_dir = EM_DIR;

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
	/* Try to read EM_DIR/lib/MACH/descr */
	gr_cat(&rline,em_dir) ;
	gr_cat(&rline,"/lib/") ; gr_cat(&rline,name) ;
	gr_cat(&rline,"/descr") ;
	infile= fopen(gr_start(rline),"r") ;
	if ( !infile ) {
		gr_throw(&rline) ;
		gr_cat(&rline,em_dir) ; gr_cat(&rline,"/") ;
		gr_cat(&rline,ACK_PATH); gr_cat(&rline,"/") ;
		gr_cat(&rline,name) ;
		infile= fopen(gr_start(rline),"r") ;
	}
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
	register int token ;

	if ( incore ) {
		if ( *inptr==0 ) return EOF ;
		return *inptr++ ;
	}
	token= getc(infile) ;
	if ( (token>=0177 || token <=0 ) && token !=EOF ) {
		fuerror("Non-ascii character in description file %s",inname);
	}
	return token ;
}

int getln() {
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
