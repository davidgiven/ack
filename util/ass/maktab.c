/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include "ip_spec.h"
#include <stdio.h>
#include <em_spec.h>
#include <em_flag.h>

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

/* This program reads the human readable interpreter specification
   and produces a efficient machine representation that can be
   translated by a C-compiler.
*/

#define NOTAB   600    /* The max no of interpreter specs */
#define ESCAP   256

struct opform intable[NOTAB] ;
struct opform *lastform = intable-1 ;

int nerror = 0 ;
int atend  = 0 ;
int line   = 1 ;
int maxinsl= 0 ;

extern char em_mnem[][4] ;
char esca[] = "escape" ;
#define ename(no)       ((no)==ESCAP?esca:em_mnem[(no)])

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
	exit(nerror);
}

readin() {
	register struct opform *nextform ;
	char *ident();
	char *firstid ;
	register maxl ;

	maxl = 0 ;
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
		if ( oplength(nextform)>maxl ) maxl=oplength(nextform) ;
		nextform++ ;
	}
	if ( !feof(stdin) ) fatal("Internal table too small") ;
	maxinsl = maxl ;
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
	case 'u':
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

writeout() {
	register struct opform *next ;
	int elem[sp_lmnem-sp_fmnem+1+1] ;
		/* for each op points to first of descr. */
	register int i,currop ;
	int nch ;
	int compare() ;

	qsort(intable,(lastform-intable)+1,sizeof intable[0],compare) ;

	printf("int\tmaxinsl\t= %d ;\n",maxinsl) ;
	currop= -1 ; nch=0 ;
	printf("char opchoice[] = {\n") ;
	for (next=intable ; next<=lastform ; next++ ) {
		if ( (next->i_opcode&0377)!=currop ) {
			for ( currop++ ;
				currop<(next->i_opcode&0377) ; currop++ ) {
				elem[currop]= nch ;
				error("Missing opcode %s",em_mnem[currop]) ;
			}
			elem[currop]= nch ;
		}
		printf("%d, %d,",next->i_flag&0377,next->i_low&0377) ;
		nch+=2 ;
		switch ( next->i_flag&OPTYPE ) {
		case OPMINI :
		case OPSHORT :
			printf("%d,",next->i_num&0377) ; nch++ ;
		}
		printf("\n") ;
	}
	for ( currop++ ; currop<=sp_lmnem-sp_fmnem ; currop++ ) {
		elem[currop]= nch ;
		error("Missing opcode %s",em_mnem[currop]) ;
	}
	elem[sp_lmnem-sp_fmnem+1]=nch ;
	printf("0 } ;\n\nchar *opindex[] = {\n");
	for ( i=0 ; i<=sp_lmnem-sp_fmnem+1 ; i++ ) {
		printf(" &opchoice[%d],\n",elem[i]) ;
	}
	printf("} ;\n") ;
}

int compare(a,b) struct opform *a,*b ; {
	if ( a->i_opcode!=b->i_opcode ) {
		return (a->i_opcode&0377)-(b->i_opcode&0377) ;
	}
	return oplength(a)-oplength(b) ;
}

int oplength(a) struct opform *a ; {
	int cnt ;

	cnt=1 ;
	if ( a->i_flag&OPESC ) cnt++ ;
	switch( a->i_flag&OPTYPE ) {
	case OPNO    :
	case OPMINI  : break ;
	case OP8     :
	case OPSHORT : cnt++ ; break ;
	case OP16U   :
	case OP16    : cnt+=2 ; break ;
	case OP32    : cnt+=5 ; break ;
	case OP64    : cnt+=9 ; break ;
	}
	return cnt ;
}

/* ----------- checking --------------*/

int ecodes[256],codes[256],lcodes[256] ;

#define NMNEM   (sp_lmnem-sp_fmnem+1)
#define MUST    1
#define MAY     2
#define FORB    3

char negc[NMNEM], zc[NMNEM], posc[NMNEM] ;

checkall() {
	register i,flag ;
	register struct opform *next ;
	int opc,low ;

	for ( i=0 ; i<NMNEM ; i++ ) negc[i]=zc[i]=posc[i]=0 ;
	for ( i=0 ; i<256 ; i++ ) lcodes[i]= codes[i]= ecodes[i]= -1 ;
	codes[254]=codes[255]=ESCAP;

	atend=0 ; line=0 ;
	for ( next=intable ; next<=lastform ; next++ ) {
		line++ ;
		flag = next->i_flag&0377 ;
		opc  = next->i_opcode&0377 ;
		low  = next->i_low&0377 ;
		chkc(flag,low,opc) ;
		switch(flag&OPTYPE) {
		case OPNO : zc[opc]++ ; break ;
		case OPMINI :
		case OPSHORT :
			for ( i=1 ; i<((next->i_num)&0377) ; i++ ) {
				chkc(flag,low+i,opc) ;
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
		case OP16U :
		case OP32 :
		case OP64 :
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

chkc(flag,icode,emc) {
	if ( flag&OPESC ) {
		if ( ecodes[icode]!=-1 ) {
			mess("Escaped opcode %d used by %s and %s",
				icode,ename(emc),ename(ecodes[icode])) ;
		}
		ecodes[icode]=emc;
	} else switch ( flag&OPTYPE ) {
	default:
		if ( codes[icode]!=-1 ) {
			mess("Opcode %d used by %s and %s",
				icode,ename(emc),ename(codes[icode])) ;
		}
		codes[icode]=emc;
		break ;
	case OP32:
	case OP64:
		if ( lcodes[icode]!=-1 ) {
			mess("Long opcode %d used by %s and %s",
				icode,ename(emc),ename(codes[icode])) ;
		}
		lcodes[icode]=emc;
		break ;
	}
}

ckop(emc,zf,pf,nf) {
	if ( zc[emc]>1 ) mess("More then one OPNO for %s",ename(emc)) ;
	if ( posc[emc]>1 ) mess("More then one OP16(pos) for %s",ename(emc)) ;
	if ( negc[emc]>1 ) mess("More then one OP16(neg) for %s",ename(emc)) ;
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
