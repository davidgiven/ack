/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef NORCSID
# define RCS_SYMB	"$Id$"
#endif

#define NIL	0
#define TRUE	1
#define FALSE	0

#define DEFAULTTYPE	500
#define INTTYPE		501
#define FLOATTYPE	502
#define DOUBLETYPE	503
#define STRINGTYPE	504

#define ABSSYM		520
#define ASCSYM		521
#define ATNSYM		522
#define CDBLSYM		524
#define CHRSYM		525
#define CINTSYM		526
#define COSSYM		527
#define CSNGSYM		528
#define CVISYM		529
#define CVSSYM		530
#define CVDSYM		531
#define EOFSYM		532
#define EXPSYM		533
#define FIXSYM		534
#define FRESYM		535
#define HEXSYM		536
#define INPSYM		538
#define INSTRSYM	539
#define LEFTSYM		540
#define LENSYM		541
#define LOCSYM		542
#define LOGSYM		543
#define LPOSSYM		544
#define MKISYM		546
#define MKSSYM		547
#define MKDSYM		548
#define OCTSYM		549
#define PEEKSYM		550
#define POSSYM		551
#define RIGHTSYM	552
#define RNDSYM		553
#define SGNSYM		554
#define SINSYM		555
#define SPACESYM	556
#define SPCSYM		557
#define SQRSYM		558
#define STRSYM		559
#define STRINGSYM	560
#define TABSYM		561
#define TANSYM		562
#define VALSYM		564
#define VARPTRSYM	565
/* some stuff forgotten */
#define INTSYM		567
#define AUTOSYM		568
#define LISTSYM		569
#define LOADSYM		570
#define MERGESYM	571
#define TRONSYM		572
#define TROFFSYM	0     /* NIEUW : was 573, werkte als TRON */
	/* IMPSYM, EQVSYM en XORSYM zijn tokens geworden */
#define OUTSYM 577

#define MAXDIMENSIONS	10

typedef struct SYMBOL{
	char 	*symname;
	int	symalias;
	int	symtype;
	int	dimensions;		/* dimension array/function */
	int	dimlimit[MAXDIMENSIONS]; /* type of parameter */
	int	dimalias[MAXDIMENSIONS]; 
	struct	SYMBOL *nextsym;
	int	isfunction;
	int	parmsize;
	int	isparam;
} Symbol;

extern	Symbol *srchsymbol();
