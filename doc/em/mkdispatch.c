/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include "ip_spec.h"
#include <stdio.h>
#include "em_spec.h"
#include "em_flag.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

/* This program reads the human readable interpreter specification
   and produces a efficient machine representation that can be
   translated by a C-compiler.
*/

#define NOTAB   600    /* The max no of interpreter specs */
#define ESCAP1  256
#define ESCAP2	257

struct opform intable[NOTAB] ;
struct opform *lastform = intable-1 ;

int nerror = 0 ;
int atend  = 0 ;
int line   = 1 ;

extern char em_mnem[][4] ;
char esca1[] = "escape1" ;
char esca2[] = "escape2" ;
#define ename(no)       ((no)==ESCAP1?esca1:(no)==ESCAP2?esca2:em_mnem[(no)])

extern char em_flag[] ;

main(argc,argv) char **argv ; {
	if ( argc>1 ) {
		if ( freopen(argv[1],"r",stdin)==NULL) {
			fatal("Cannot open %s",argv[1]) ;
		}
	}
	if ( argc>2 ) {
		if ( freopen(argv[2],"w",stdout)==NULL) {
			fatal("Cannot create %s",argv[2]) ;
		}
	}
	if ( argc>3 ) {
		fatal("%s [ file [ file ] ]",argv[0]) ;
	}
	atend=0 ;
	readin();
	atend=1 ;
	checkall();
	if ( nerror==0 ) {
		writeout();
	}
	exit(nerror) ;
}

readin() {
	register struct opform *nextform ;
	char *ident();
	char *firstid ;

	for ( nextform=intable ;
		!feof(stdin) && nextform<&intable[NOTAB] ; ) {
		firstid=ident() ;
		if ( *firstid=='\n' || feof(stdin) ) continue ;
		lastform=nextform ;
		nextform->i_opcode = getmnem(firstid) ;
		nextform->i_flag   = decflag(ident()) ;
		switch ( nextform->i_flag&OPTYPE ) {
		case OPMINI:
		case OPSHORT:
			nextform->i_num = atoi(ident()) ;
			break ;
		}
		nextform->i_low    = atoi(ident())    ;
		if ( *ident()!='\n' ) {
			int c ;
			error("End of line expected");
			while ( (c=readchar())!='\n' && c!=EOF ) ;
		}
		nextform++ ;
	}
	if ( !feof(stdin) ) fatal("Internal table too small") ;
}

char *ident() {
	/* skip spaces and tabs, anything up to space,tab or eof is
	   a identifier.
	   Anything from # to end-of-line is an end-of-line.
	   End-of-line is an identifier all by itself.
	*/

	static char array[200] ;
	register int c ;
	register char *cc ;

	do {
		c=readchar() ;
	} while ( c==' ' || c=='\t' ) ;
	for ( cc=array ; cc<&array[(sizeof array) - 1] ; cc++ ) {
		if ( c=='#' ) {
			do {
				c=readchar();
			} while ( c!='\n' && c!=EOF ) ;
		}
		*cc = c ;
		if ( c=='\n' && cc==array ) break ;
		c=readchar() ;
		if ( c=='\n' ) {
			pushback(c) ;
			break ;
		}
		if ( c==' ' || c=='\t' || c==EOF ) break ;
	}
	*++cc=0 ;
	return array ;
}

int getmnem(str) char *str ; {
	char (*ptr)[4] ;

	for ( ptr = em_mnem ; *ptr<= &em_mnem[sp_lmnem-sp_fmnem][0] ; ptr++ ) {
		if ( strcmp(*ptr,str)==0 ) return (ptr-em_mnem) ;
	}
	error("Illegal mnemonic") ;
	return 0 ;
}

error(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	if ( !atend ) fprintf(stderr,"line %d: ",line) ;
	fprintf(stderr,str,a1,a2,a3,a4,a5,a6) ;
	fprintf(stderr,"\n");
	nerror++ ;
}

mess(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	if ( !atend ) fprintf(stderr,"line %d: ",line) ;
	fprintf(stderr,str,a1,a2,a3,a4,a5,a6) ;
	fprintf(stderr,"\n");
}

fatal(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	error(str,a1,a2,a3,a4,a5,a6) ;
	exit(1) ;
}

#define ILLGL   -1

check(val) int val ; {
	if ( val!=ILLGL ) error("Illegal flag combination") ;
}

int decflag(str) char *str ; {
	int type ;
	int escape ;
	int range ;
	int wordm ;
	int notzero ;

	type=escape=range=wordm=notzero= ILLGL ;
	while ( *str ) switch ( *str++ ) {
	case 'm' :
		check(type) ; type=OPMINI ; break ;
	case 's' :
		check(type) ; type=OPSHORT ; break ;
	case '-' :
		check(type) ; type=OPNO ; break ;
	case '1' :
		check(type) ; type=OP8 ; break ;
	case '2' :
		check(type) ; type=OP16 ; break ;
	case '4' :
		check(type) ; type=OP32 ; break ;
	case '8' :
		check(type) ; type=OP64 ; break ;
	case 'u' :
		check(type) ; type=OP16U ; break ;
	case 'e' :
		check(escape) ; escape=0 ; break ;
	case 'N' :
		check(range) ; range= 2 ; break ;
	case 'P' :
		check(range) ; range= 1 ; break ;
	case 'w' :
		check(wordm) ; wordm=0 ; break ;
	case 'o' :
		check(notzero) ; notzero=0 ; break ;
	default :
		error("Unknown flag") ;
	}
	if ( type==ILLGL ) error("Type must be specified") ;
	switch ( type ) {
	case OP64 :
	case OP32 :
		if ( escape!=ILLGL ) error("Conflicting escapes") ;
		escape=ILLGL ;
	case OP16 :
	case OP16U :
	case OP8 :
	case OPSHORT :
	case OPNO :
		if ( notzero!=ILLGL ) mess("Improbable OPNZ") ;
		if ( type==OPNO && range!=ILLGL ) {
			mess("No operand in range") ;
		}
	}
	if ( escape!=ILLGL ) type|=OPESC ;
	if ( wordm!=ILLGL ) type|=OPWORD ;
	switch ( range) {
	case ILLGL : type|=OP_BOTH ;
		     if ( type==OPMINI || type==OPSHORT )
			     error("Minies and shorties must have P or N") ;
		     break ;
	case 1     : type|=OP_POS  ; break ;
	case 2     : type|=OP_NEG  ; break ;
	}
	if ( notzero!=ILLGL ) type|=OPNZ ;
	return type ;
}

/* ----------- checking --------------*/

int ecodes[256],codes[256],lcodes[256] ;
char eflags[256], flags[256], lflags[256] ;
int elows[256], lows[256], llows[256];

#define NMNEM   (sp_lmnem-sp_fmnem+1)
#define MUST    1
#define MAY     2
#define FORB    3

char negc[NMNEM], zc[NMNEM], posc[NMNEM], lnegc[NMNEM], lposc[NMNEM] ;

checkall() {
	register i,flag ;
	register struct opform *next ;
	int opc,low ;

	for ( i=0 ; i<NMNEM ; i++ ) negc[i]=zc[i]=posc[i]=0 ;
	for ( i=0 ; i<256 ; i++ ) lcodes[i]= codes[i]= ecodes[i]= -1 ;
	codes[254]=ESCAP1; codes[255]=ESCAP2;

	atend=0 ; line=0 ;
	for ( next=intable ; next<=lastform ; next++ ) {
		line++ ;
		flag = next->i_flag&0377 ;
		opc  = next->i_opcode&0377 ;
		low  = next->i_low&0377 ;
		chkc(flag,low,opc,low) ;
		switch(flag&OPTYPE) {
		case OPNO : zc[opc]++ ; break ;
		case OPMINI :
		case OPSHORT :
			for ( i=1 ; i<((next->i_num)&0377) ; i++ ) {
				chkc(flag,low+i,opc,low) ;
			}
			if ( !(em_flag[opc]&PAR_G) &&
			     (flag&OPRANGE)==OP_BOTH) {
	      mess("Mini's and shorties should have P or N");
			}
			break ;
		case OP8 :
			error("OP8 is removed") ;
			break ;
		case OP16 :
			if ( flag&OP_NEG )
				negc[opc]++ ;
			else if ( flag&OP_POS )
				posc[opc]++ ;
			break ;
		case OP32 :
			if ( flag&OP_NEG )
				lnegc[opc]++ ;
			else if ( flag&OP_POS )
				lposc[opc]++ ;
			break ;
		case OP16U :
			break ;
		default :
			error("Illegal type") ;
			break ;
		}
	}
	atend=1 ;
	for ( i=0 ; i<256 ; i++ ) if ( codes[i]== -1 ) {
		mess("interpreter opcode %d not used",i) ;
	}
	for ( opc=0 ; opc<NMNEM ; opc++ ) {
		switch(em_flag[opc]&EM_PAR) {
		case PAR_NO :
			ckop(opc,MUST,FORB,FORB) ;
			break ;
		case PAR_C:
		case PAR_D:
		case PAR_F:
		case PAR_B:
			ckop(opc,FORB,MAY,MAY) ;
			break ;
		case PAR_N:
		case PAR_G:
		case PAR_S:
		case PAR_Z:
		case PAR_O:
		case PAR_P:
			ckop(opc,FORB,MAY,FORB) ;
			break ;
		case PAR_R:
			ckop(opc,FORB,MAY,FORB) ;
			break ;
		case PAR_L:
			ckop(opc,FORB,MUST,MUST) ;
			break ;
		case PAR_W:
			ckop(opc,MUST,MAY,FORB) ;
			break ;
		default :
			error("Unknown instruction type of %s",ename(opc)) ;
			break ;
		}
	}
}

chkc(flag,icode,emc,low) {
	if ( flag&OPESC ) {
		if ( ecodes[icode]!=-1 ) {
			mess("Escaped opcode %d used by %s and %s",
				icode,ename(emc),ename(ecodes[icode])) ;
		}
		ecodes[icode]=emc;
		eflags[icode]=flag;
		elows[icode]=low;
	} else switch ( flag&OPTYPE ) {
	default:
		if ( codes[icode]!=-1 ) {
			mess("Opcode %d used by %s and %s",
				icode,ename(emc),ename(codes[icode])) ;
		}
		codes[icode]=emc;
		flags[icode]=flag;
		lows[icode]=low;
		break ;
	case OP32:
	case OP64:
		if ( lcodes[icode]!=-1 ) {
			mess("Long opcode %d used by %s and %s",
				icode,ename(emc),ename(lcodes[icode])) ;
		}
		lcodes[icode]=emc;
		lflags[icode]=flag;
		llows[icode]=low;
		break ;
	}
}

ckop(emc,zf,pf,nf) {
	if ( zc[emc]>1 ) mess("More then one OPNO for %s",ename(emc)) ;
	if ( posc[emc]>1 ) mess("More then one OP16(pos) for %s",ename(emc)) ;
	if ( negc[emc]>1 ) mess("More then one OP16(neg) for %s",ename(emc)) ;
	if ( lposc[emc]>1 ) mess("More then one OP32(pos) for %s",ename(emc)) ;
	if ( lnegc[emc]>1 ) mess("More then one OP32(neg) for %s",ename(emc)) ;
	switch(zf) {
	case MUST:
		if ( zc[emc]==0 ) mess("No OPNO for %s",ename(emc)) ;
		break ;
	case FORB:
		if ( zc[emc]==1 ) mess("Forbidden OPNO for %s",ename(emc)) ;
		break ;
	}
	switch(pf) {
	case MUST:
		if ( posc[emc]==0 ) mess("No OP16(pos) for %s",ename(emc)) ;
		break ;
	case FORB:
		if ( posc[emc]==1 )
			mess("Forbidden OP16(pos) for %s",ename(emc)) ;
		break ;
	}
	switch(nf) {
	case MUST:
		if ( negc[emc]==0 ) mess("No OP16(neg) for %s",ename(emc)) ;
		break ;
	case FORB:
		if ( negc[emc]==1 )
			mess("Forbidden OP16(neg) for %s",ename(emc)) ;
		break ;
	}
}

static int pushchar ;
static int pushf ;

int readchar() {
	int c ;

	if ( pushf ) {
		pushf=0 ;
		c = pushchar ;
	} else {
		if ( feof(stdin) ) return EOF ;
		c=getc(stdin) ;
	}
	if ( c=='\n' ) line++ ;
	return c ;
}

pushback(c) {
	if ( pushf ) {
		fatal("Double pushback") ;
	}
	pushf++ ;
	pushchar=c ;
	if ( c=='\n' ) line-- ;
}

writeout() {
	register int i;

	printf("DISPATCH1");
	for (i = 0; i < 256;) {
		if (!(i % 8)) printf("\n%d", i);
		printf("\t%s", ename(codes[i]));
		if (i < 254) {
			prx(flags[i],lows[i],i);
		}
		i++;
	}

	printf("\nDISPATCH2");
	for (i = 0; i < 256;) {
		if (ecodes[i] != -1) {
			if (!(i % 8)) printf("\n%d", i);
			printf("\t%s", ename(ecodes[i]));
			prx(eflags[i],elows[i],i);
		}
		else break;
		i++;
	}

	printf("\nDISPATCH3");
	i = 0;
	while (lcodes[i] != -1) {
		if (!(i % 8)) printf("\n%d", i);
		printf("\t%s", ename(lcodes[i]));
			prx(lflags[i],llows[i],i);
		i++;
	}
	while (i++ % 8) putchar('\t');
	putchar('\n');
}

prx(flg,low,opc)
	register int flg;
{
	int arg = opc - low;

	putchar('.');
	switch(flg&OPTYPE) {
	case OPNO:
		putchar('z');
		break;
	case OP16U:
		putchar('u');
		break;
	case OP16:
		if (flg&OP_POS) putchar('p');
		else if (flg&OP_NEG) putchar('n');
		else putchar('l');
		if (flg&OPWORD) putchar('w');
		break;
	case OP32:
		if (flg&OP_POS) putchar('P');
		else if (flg&OP_NEG) putchar('N');
		else putchar('L');
		if (flg&OPWORD) putchar('w');
		break;
	case OPSHORT:
		if (flg & OPWORD) putchar('w');
		else putchar('s');
		/* fall through */
	case OPMINI:
		if (flg & OPNZ) arg++;
		if (flg & OP_NEG) arg = -arg - 1;
		printf("%d",arg);
		if((flg&OPTYPE) == OPMINI && (flg & OPWORD)) putchar('W');
	}
}
