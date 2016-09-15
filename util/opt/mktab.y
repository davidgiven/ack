%{
#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "param.h"
#include "types.h"
#include "pattern.h"
#include <em_spec.h>
#include <em_mnem.h>
#include "optim.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#define op_CBO	(op_plast+1)

#define MAXNODES 1000
expr_t	nodes[MAXNODES];
expr_p	lastnode = nodes+1;
int	curind,prevind;
int	patlen,maxpatlen,rpllen;
int	lino = 1;
int	patno=1;
#define MAX	100
int patmnem[MAX],rplmnem[MAX],rplexpr[MAX];
byte	nparam[N_EX_OPS];
bool	nonumlab[N_EX_OPS];
bool	onlyconst[N_EX_OPS];
int	nerrors=0;
char	patid[128];

int CBO_instrs[] = {
	op_adi,
	op_adu,
	op_and,
	op_ior,
	op_xor
	/* don't add op_mli and op_mlu! */
};

int	patCBO;
int	rplCBO;
%}

%union {
	int	y_int;
}

%left OR2
%left AND2
%left OR1
%left XOR1
%left AND1
%left CMPEQ CMPNE
%left CMPLT CMPLE CMPGT CMPGE
%left RSHIFT LSHIFT
%left ARPLUS ARMINUS
%left ARTIMES ARDIVIDE ARMOD
%nonassoc NOT COMP UMINUS
%nonassoc '$'

%token SFIT UFIT NOTREG PSIZE WSIZE DEFINED SAMESIGN ROM ROTATE STRING
%token <y_int> MNEM
%token <y_int> NUMBER
%type <y_int> expr argno optexpr

%start patternlist

%%
patternlist
	:	/* empty */
	|	STRING '\n'
	|	patternlist '\n'
	|	patternlist pattern
	;
pattern	:
		mnemlist optexpr ':' replacement '\n'
			{
			  if (patCBO) {
				register int i;

			  	if (! rplCBO) {
					yyerror("No CBO in replacement");
				}
				for (i=0; i<sizeof(CBO_instrs)/sizeof(int); i++) {
					outpat($2, CBO_instrs[i]);
				}
			  }
			  else {
				outpat($2, 0);
			  }
			  patCBO = rplCBO = 0;
			}
	|	error '\n'
			{ yyerrok; }
	;
replacement
	:	expr	/* special optimization */
			{
#ifdef ALLOWSPECIAL
			  rpllen=1; rplmnem[0]=0; rplexpr[0]=$1;
#else
			  yyerror("No specials allowed");
#endif
			}
	|	repllist
	;
repllist:	/* empty */
			{ rpllen=0; }
	|	repllist repl
	;
repl	:	MNEM	optexpr
			{ rplmnem[rpllen] = $1; rplexpr[rpllen++] = $2;
			  if ($1 == op_CBO) {
				if (!patCBO) {
					yyerror("No CBO in pattern");
				}
				if (rplCBO) {
					yyerror("Only one CBO allowed in replacement");
				}
				rplCBO = 1;
			  }
			}
	;
mnemlist:	MNEM
			{ patlen=0; patmnem[patlen++] = $1;
			  if ($1 == op_CBO) {
				if (patCBO) {
					yyerror("Only one CBO allowed in pattern");
				}
				patCBO = 1;
			  }
			}
	|	mnemlist MNEM
			{ patmnem[patlen++] = $2;
			  if ($2 == op_CBO) {
				if (patCBO) {
					yyerror("Only one CBO allowed in pattern");
				}
				patCBO = 1;
			  }
			}
	;
optexpr	:	/* empty */
			{ $$ = 0; }
	|	expr
	;
expr	
	:	'$' argno
			{ $$ = lookup(0,EX_ARG,$2,0); }
	|	NUMBER
			{ $$ = lookup(0,EX_CON,(int)(short)$1,0); }
	|	PSIZE
			{ $$ = lookup(0,EX_POINTERSIZE,0,0); }
	|	WSIZE
			{ $$ = lookup(0,EX_WORDSIZE,0,0); }
	|	DEFINED '(' expr ')'
			{ $$ = lookup(0,EX_DEFINED,$3,0); }
	|	SAMESIGN '(' expr ',' expr ')'
			{ $$ = lookup(1,EX_SAMESIGN,$3,$5); }
	|	SFIT '(' expr ',' expr ')'
			{ $$ = lookup(0,EX_SFIT,$3,$5); }
	|	UFIT '(' expr ',' expr ')'
			{ $$ = lookup(0,EX_UFIT,$3,$5); }
	|	ROTATE '(' expr ',' expr ')'
			{ $$ = lookup(0,EX_ROTATE,$3,$5); }
	|	NOTREG '(' expr ')'
			{ $$ = lookup(0,EX_NOTREG,$3,0); }
	|	ROM '(' argno ',' expr ')'
			{ $$ = lookup(0,EX_ROM,$3,$5); }
	|	'(' expr ')'
			{ $$ = $2; }
	|	expr CMPEQ expr
			{ $$ = lookup(1,EX_CMPEQ,$1,$3); }
	|	expr CMPNE expr
			{ $$ = lookup(1,EX_CMPNE,$1,$3); }
	|	expr CMPGT expr
			{ $$ = lookup(0,EX_CMPGT,$1,$3); }
	|	expr CMPGE expr
			{ $$ = lookup(0,EX_CMPGE,$1,$3); }
	|	expr CMPLT expr
			{ $$ = lookup(0,EX_CMPLT,$1,$3); }
	|	expr CMPLE expr
			{ $$ = lookup(0,EX_CMPLE,$1,$3); }
	|	expr OR2 expr
			{ $$ = lookup(0,EX_OR2,$1,$3); }
	|	expr AND2 expr
			{ $$ = lookup(0,EX_AND2,$1,$3); }
	|	expr OR1 expr
			{ $$ = lookup(1,EX_OR1,$1,$3); }
	|	expr XOR1 expr
			{ $$ = lookup(1,EX_XOR1,$1,$3); }
	|	expr AND1 expr
			{ $$ = lookup(1,EX_AND1,$1,$3); }
	|	expr ARPLUS expr
			{ $$ = lookup(1,EX_PLUS,$1,$3); }
	|	expr ARMINUS expr
			{ $$ = lookup(0,EX_MINUS,$1,$3); }
	|	expr ARTIMES expr
			{ $$ = lookup(1,EX_TIMES,$1,$3); }
	|	expr ARDIVIDE expr
			{ $$ = lookup(0,EX_DIVIDE,$1,$3); }
	|	expr ARMOD expr
			{ $$ = lookup(0,EX_MOD,$1,$3); }
	|	expr LSHIFT expr
			{ $$ = lookup(0,EX_LSHIFT,$1,$3); }
	|	expr RSHIFT expr
			{ $$ = lookup(0,EX_RSHIFT,$1,$3); }
	|	ARPLUS expr %prec UMINUS
			{ $$ = $2; }
	|	ARMINUS expr %prec UMINUS
			{ $$ = lookup(0,EX_UMINUS,$2,0); }
	|	NOT expr
			{ $$ = lookup(0,EX_NOT,$2,0); }
	|	COMP expr
			{ $$ = lookup(0,EX_COMP,$2,0); }
	;
argno	:	NUMBER
			{ if ($1<1 || $1>patlen) {
				YYERROR;
			  }
			  $$ = (int) $1;
			}
	;

%%

extern char em_mnem[][4];

#define HASHSIZE	(2*(sp_lmnem-sp_fmnem))

struct hashmnem {
	char h_name[3];
	byte h_value;
} hashmnem[HASHSIZE];

inithash() {
	register i;

	enter("lab",op_lab);
	enter("LLP",op_LLP);
	enter("LEP",op_LEP);
	enter("SLP",op_SLP);
	enter("SEP",op_SEP);
	enter("CBO",op_CBO);
	for(i=0;i<=sp_lmnem-sp_fmnem;i++)
		enter(em_mnem[i],i+sp_fmnem);
}

unsigned hashname(name) register char *name; {
	register unsigned h;

	h = (*name++)&BMASK;
	h = (h<<4)^((*name++)&BMASK);
	h = (h<<4)^((*name++)&BMASK);
	return(h);
}

enter(name,value) char *name; {
	register unsigned h;

	h=hashname(name)%HASHSIZE;
	while (hashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	strncpy(hashmnem[h].h_name,name,3);
	hashmnem[h].h_value = value;
}

int mlookup(name) char *name; {
	register unsigned h;

	h = hashname(name)%HASHSIZE;
	while (strncmp(hashmnem[h].h_name,name,3) != 0 &&
	       hashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	return(hashmnem[h].h_value&BMASK);	/* 0 if not found */
}

main() {

	inithash();
	initio();
	yyparse();
	if (nerrors==0)
		printnodes();
	return nerrors;
}

int yywrap(void) {
	return 1;
}

yyerror(s) char *s; {

	fprintf(stderr,"line %d: %s\n",lino,s);
	nerrors++;
}

lookup(comm,operator,lnode,rnode) {
	register expr_p p;

	for (p=nodes+1;p<lastnode;p++) {
		if (p->ex_operator != operator)
			continue;
		if (!(p->ex_lnode == lnode && p->ex_rnode == rnode ||
		    comm && p->ex_lnode == rnode && p->ex_rnode == lnode))
			continue;
		return(p-nodes);
	}
	if (lastnode >= &nodes[MAXNODES])
		yyerror("node table overflow");
	lastnode++;
	p->ex_operator = operator;
	p->ex_lnode = lnode;
	p->ex_rnode = rnode;
	return(p-nodes);
}

printnodes() {
	register expr_p p;

	printf("};\n\nshort lastind = %d;\n\nexpr_t enodes[] = {\n",prevind);
	for (p=nodes;p<lastnode;p++)
		printf("/* %3d */\t%3d,%6u,%6u,\n",
			p-nodes,p->ex_operator,p->ex_lnode,p->ex_rnode);
	printf("};\n\niarg_t iargs[%d];\n", (maxpatlen>0 ? maxpatlen : 1));
	if (patid[0])
		printf("static char rcsid[] = %s;\n",patid);
}

initio() {
	register i;

	printf("#include \"param.h\"\n#include \"types.h\"\n");
	printf("#include \"pattern.h\"\n\n");
	for(i=0;i<N_EX_OPS;i++) {
		nparam[i]=2;
		nonumlab[i]=TRUE;
		onlyconst[i]=TRUE;
	}
	nparam[EX_POINTERSIZE] = 0;
	nparam[EX_WORDSIZE] = 0;
	nparam[EX_CON] = 0;
	nparam[EX_ROM] = 0;
	nparam[EX_ARG] = 0;
	nparam[EX_DEFINED] = 0;
	nparam[EX_OR2] = 1;
	nparam[EX_AND2] = 1;
	nparam[EX_UMINUS] = 1;
	nparam[EX_NOT] = 1;
	nparam[EX_COMP] = 1;
	nparam[EX_NOTREG] = 1;
	nonumlab[EX_CMPEQ] = FALSE;
	nonumlab[EX_CMPNE] = FALSE;
	onlyconst[EX_CMPEQ] = FALSE;
	onlyconst[EX_CMPNE] = FALSE;
	onlyconst[EX_CMPLE] = FALSE;
	onlyconst[EX_CMPLT] = FALSE;
	onlyconst[EX_CMPGE] = FALSE;
	onlyconst[EX_CMPGT] = FALSE;
	onlyconst[EX_PLUS] = FALSE;
	onlyconst[EX_MINUS] = FALSE;
	printf("byte nparam[] = {");
	for (i=0;i<N_EX_OPS;i++) printf("%d,",nparam[i]);
	printf("};\nbool nonumlab[] = {");
	for (i=0;i<N_EX_OPS;i++) printf("%d,",nonumlab[i]);
	printf("};\nbool onlyconst[] = {");
	for (i=0;i<N_EX_OPS;i++) printf("%d,",onlyconst[i]);
	printf("};\n\nbyte pattern[] = { 0\n");
	curind = 1;
}

outpat(exprno, instrno)
{
	register int i;

  	outbyte(0); outshort(prevind); prevind=curind-3;
  	out(patlen);
  	for (i=0;i<patlen;i++) {
		if (patmnem[i] == op_CBO) outbyte(instrno);
		else outbyte(patmnem[i]);
	}
  	out(exprno);
  	out(rpllen);
  	for (i=0;i<rpllen;i++) {
		if (rplmnem[i] == op_CBO) outbyte(instrno);
		else outbyte(rplmnem[i]);
		out(rplexpr[i]);
  	}
#ifdef DIAGOPT
  	outshort(patno);
#endif
  	patno++;
  	printf("\n");
  	if (patlen>maxpatlen) maxpatlen=patlen;
}

outbyte(b) {

	printf(",%3d",b);
	curind++;
}

outshort(s) {

	outbyte(s&0377);
	outbyte((s>>8)&0377);
}

out(w) {

	if (w<255) {
		outbyte(w);
	} else {
		outbyte(255);
		outshort(w);
	}
}

