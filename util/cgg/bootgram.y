%{

#ifndef NORCSID
static char rcsid[]="$Header$";
#endif

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
 * Author: Hans van Staveren
 */

#ifdef vax | vax2 | vax4
#define BIG
#endif

#ifdef BIG
#define BORS(x,y) x
#else
#define BORS(x,y) y
#endif
/* Tunable constants */

#define MAXALLREG 5             /* Maximum number of allocates per rule */
#define MAXREGS BORS(36,32)     /* Total number of registers */
#define MAXREGVARS 8		/* Maximum regvars per type */
#define MAXPROPS 16             /* Total number of register properties */
#define MAXTOKENS BORS(75,32)   /* Different kind of tokens */
#define MAXSETS BORS(100,80)    /* Number of tokenexpressions definable */
#define MAXEMPATLEN 25		/* Maximum length of EM-pattern/replacement */
#define TOKENSIZE 5             /* Maximum number of fields in token struct */
#define MAXINSTANCE BORS(250,120) /* Maximum number of different tokeninstances */
#define MAXSTRINGS BORS(800,400)/* Maximum number of different codestrings */
#define MAXPATTERN BORS(7000,6000) /* Maximum number of bytes in pattern[] */
#define MAXNODES BORS(450,400)  /* Maximum number of expression nodes */
#define MAXMEMBERS 2            /* Maximum number of subregisters per reg */
#define NMOVES BORS(50,30)      /* Maximum number of move definitions */
#define MAXC1   20              /* Maximum of coercions type 1 */
#define MAXC2   20              /* Maximum of coercions type 2 */
#define MAXC3   20              /* Maximum of coercions type 3 */
#define MAXSPLIT 4              /* Maximum degree of split */
#define MAXNSTR 40		/* Maximum consecutive strings in coderule */

/* Derived constants */

#define SETSIZE ((MAXREGS+1+MAXTOKENS+15)>>4)
#define PROPSETSIZE ((MAXPROPS+15)>>4)

#define BMASK 0377
#define BSHIFT 8

#define TRUE    1
#define FALSE   0

#define MAXPATLEN 7             /* Maximum length of tokenpatterns */

typedef char byte;
typedef char * string;

char *malloc(),*myalloc();

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <em_spec.h>
#include <em_flag.h>
#include <em_reg.h>
#include <cg_pattern.h>

typedef struct list1str {
	struct list1str *l1next;
	string l1name;
} *list1;
typedef struct list2str {
	struct list2str *l2next;
	list1 l2list;
} *list2;
typedef struct list3str {
	struct list3str *l3next;
	list2 l3list;
} *list3;

typedef struct reginfo {
	string rname;
	string rrepr;
	int rsize;
	int rmembers[MAXMEMBERS];
	int rregvar;
	short rprop[PROPSETSIZE];
} *reginfo;

typedef struct tokeninfo {
	string t_name;
	list2 t_struct;
	struct {
		int t_type;
		string t_sname;
	} t_fields[TOKENSIZE-1];
	int t_size;
	cost_t t_cost;
	int t_format;
} token_t,*token_p;

typedef struct ident {
	struct ident *i_next;
	string i_name;
	int i_type;
#               define IREG 1
#               define IPRP 2
#               define ITOK 3
#               define IEXP 4
	union {
		int i_regno;
		int i_prpno;
		int i_tokno;
		int i_expno;
	} i_i;
} ident_t,*ident_p;

#define ITABSIZE 32
ident_p identtab[ITABSIZE];

#define LOOKUP          0
#define HALFWAY         1
#define ENTER           2
#define JUSTLOOKING     3


typedef struct expr {
	int expr_typ;
#               define TYPINT  1
#               define TYPREG  2
#               define TYPSTR  3
#               define TYPBOOL 4
	int expr_index;
} expr_t,*expr_p;

unsigned cc1=1,cc2=1,cc3=1,cc4=1;

node_t  nodes[MAXNODES];
node_p  lastnode=nodes+1;

string codestrings[MAXSTRINGS];
int ncodestrings;

int strar[MAXNSTR];
int nstr;

int pathash[256];

reginfo machregs[MAXREGS];
char stregclass[MAXREGS];
int nmachregs=1;
int nregclasses=1;
int maxmembers;
struct {
	ident_p propname;
	set_t	propset;
} machprops[MAXPROPS];
int nprops=0;
token_t machtokens[MAXTOKENS];
int nmachtokens=1;
set_t machsets[MAXSETS];
int nmachsets=0;
int patmnem[MAXEMPATLEN];
int empatlen;
int maxempatlen;
int empatexpr;
int maxrule=1;
int pattokexp[MAXPATLEN];
int tokpatlen;
int lookident=0;        /* lexical analyzer flag */
list3 structpool=0;
int nallreg;
int allreg[MAXALLREG];
int maxallreg;
int lino=0;
int nerrors=0;
int curtokexp;
expr_t arexp[TOKENSIZE];
int narexp;
inst_t arinstance[MAXINSTANCE];
int narinstance=1;
move_t machmoves[NMOVES];
int nmoves=0;
byte pattern[MAXPATTERN];
int npatbytes=0;
int prevind;
int rulecount;                  /* Temporary index for ... construct */
int ncoderules=0;
int codebytes=0;
FILE *cfile;
FILE *hfile;
int maxtokensize=0;
int dealflag;
int emrepllen;
int replmnem[MAXEMPATLEN];
int tokrepllen;
int replinst[MAXPATLEN];
int replexpr[MAXPATLEN];
c1_t c1coercs[MAXC1];
c2_t c2coercs[MAXC2];
c3_t c3coercs[MAXC3];
int nc1=0,nc2=0,nc3=0;
int maxsplit=0;
int wsize= -1;
int psize= -1;
int bsize= -1;
char *fmt=0;

int cchandled;
int ccspoiled;
int ccregexpr;
int ccinstanceno;
int cocopropno;
int cocosetno;
int allexpno;

int rvused;	/* regvars used */
int nregvar[4];	/* # of register variables of all kinds */
int rvnumbers[4][MAXREGVARS];	/* The register numbers */

#define chktabsiz(size,maxsize,which) if(size>=maxsize) tabovf(which)

#define MUST1BEINT(e) int exp1=e.expr_index;tstint(e)
#define MUST2BEINT(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstint(e1);tstint(e2)
#define MUST1BEBOOL(e) int exp1=e.expr_index;tstbool(e)
#define MUST2BEBOOL(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstbool(e1);tstbool(e2)

%}

%union {
	int yy_int;
	int *yy_intp;
	string yy_string;
	list1 yy_list1;
	list2 yy_list2;
	expr_t yy_expr;
	cost_t yy_cost;
	set_t yy_set;
	ident_p yy_ident;
	char yy_char;
	inst_t yy_instance;
}

%type <yy_list1> list1,structlistel
%type <yy_list2> structlist,structdecl
%type <yy_expr> expr optexpr
%type <yy_cost> optcost cost optcommacost
%type <yy_int> optboolexpr optnocoerc mnem emargno tokargno optprop
%type <yy_int> optcommabool optstack subreg tokenexpressionno optregvar
%type <yy_int> tokeninstanceno code stackreplacement optslashnumber
%type <yy_set> tokenexpression
%type <yy_instance> tokeninstance
%type <yy_string> optformat
%token <yy_string> IDENT TYPENAME
%token <yy_ident> RIDENT,PIDENT,TIDENT,EIDENT
%token <yy_string> LSTRING,STRING
%token <yy_int> NUMBER
%token <yy_intp> CIDENT
%token REGISTERHEAD TOKENHEAD EXPRESSIONHEAD CODEHEAD MOVEHEAD TESTHEAD STACKHEAD
%token REGVAR INREG LOOP POINTER FLOAT
%token TIMEFAC SIZEFAC FORMAT RETURN
%token MOVE ERASE ALLOCATE ELLIPS COST REMOVE STACK
%token SEP SAMESIGN SFIT UFIT ROM DEFINED TOSTRING LOWW HIGHW
%token NOCC SETCC SAMECC TEST NOCOERC
%token <yy_char> LCASELETTER
%start machinespec

%left OR2
%left AND2
%left CMPEQ,CMPNE
%left CMPLT,CMPLE,CMPGT,CMPGE
%left RSHIFT,LSHIFT
%left '+','-'
%left '*','/','%'
%nonassoc NOT,COMP,UMINUS
%nonassoc '$'
%%
machinespec
	: rcsid constants registersection tokensection
		{ inbetween(); }
	  expressionsection codesection movesection testsection stacksection
	;

rcsid
	: /* empty */
	| STRING
		{ strlookup($1); }
	;

constants
	: /* empty */
	| constants CIDENT '=' NUMBER
		{ *$2 = $4; }
	| constants SIZEFAC '=' NUMBER optslashnumber
		{ cc1 = $4; cc2 = $5; }
	| constants TIMEFAC '=' NUMBER optslashnumber
		{ cc3 = $4; cc4 = $5; }
	| constants FORMAT '=' STRING
		{ fmt = $4; }
	;
optslashnumber
	: /* empty */
		{ $$ = 1; }
	| '/' NUMBER
		{ $$ = $2; }
	;

registersection
	: REGISTERHEAD registerdefs
	;
registerdefs
	: /* empty */
	| registerdefs registerdef
	;

registerdef
	: IDENT '=' '(' STRING ',' NUMBER list1 ')' optregvar list1 '.'
		{       register ident_p ip;
			register list1 l;
			register reginfo r;
			int i;

			r=(reginfo) myalloc(sizeof(struct reginfo));
			r->rname = $1;
			r->rrepr = $4;
			r->rsize = $6;
			if($9>=0 && $7!=0)
				yyerror("No subregisters allowed in regvar");
			for (i=0;i<MAXMEMBERS;i++)
				r->rmembers[i] = 0;
			i=0;
			for (l=$7;l!=0;l=l->l1next) {
				ip=ilookup(l->l1name,LOOKUP);
				if (ip->i_type != IREG)
					yyerror("Bad member of set");
				chktabsiz(i,MAXMEMBERS,"Member of register");
				r->rmembers[i++] = ip->i_i.i_regno;
			}
			maxmembers=max(maxmembers,i);
			r->rregvar=$9;
			if ($9>=0) {
				rvused=1;
				chktabsiz(nregvar[$9],MAXREGVARS,"Regvar");
				rvnumbers[$9][nregvar[$9]++] = nmachregs;
			}
			for(i=0;i<PROPSETSIZE;i++)
				r->rprop[i] = 0;
			ip=ilookup($1,ENTER);
			ip->i_type=IREG;
			ip->i_i.i_regno=nmachregs;
			for (l = $10; l!= 0; l=l->l1next) {
				ip = ilookup(l->l1name,HALFWAY);
				if (ip->i_type) {
					if (ip->i_type != IPRP)
						yyerror("Multiple defined symbol");
					else if(machprops[ip->i_i.i_prpno].propset.set_size != r->rsize)
						yyerror("property has more than 1 size");
				} else {
					chktabsiz(nprops,MAXPROPS,"Property");
					ip->i_type = IPRP;
					ip->i_i.i_prpno = nprops;
					machprops[nprops].propname = ip;
					machprops[nprops++].propset.set_size = r->rsize;
				}
				r->rprop[ip->i_i.i_prpno>>4] |= (1<<(ip->i_i.i_prpno&017));
			}
			chktabsiz(nmachregs,MAXREGS,"Register table");
			machregs[nmachregs++] = r;
		}
	| error '.'
	;

optregvar
	: /* nothing */
		{ $$ = -1; }
	| REGVAR
		{ $$ = reg_any; }
	| REGVAR '(' LOOP ')'
		{ $$ = reg_loop; }
	| REGVAR '(' POINTER ')'
		{ $$ = reg_pointer; }
	| REGVAR '(' FLOAT ')'
		{ $$ = reg_float; }
	;

tokensection
	: TOKENHEAD tkdefs
	;
tkdefs
	: /* empty */
	| tkdefs tkdef
	;
tkdef
	: IDENT '=' structdecl NUMBER optcost optformat
		{ register token_p tp;
		  register ident_p ip;

		  chktabsiz(nmachtokens,MAXTOKENS,"Token table");
		  tp = &machtokens[nmachtokens];
		  tp->t_name = $1;
		  tp->t_struct = $3;
		  tp->t_size = $4;
		  tp->t_cost = $5;
		  ip = ilookup($1,ENTER);
		  ip->i_type = ITOK;
		  ip->i_i.i_tokno = nmachtokens++;
		  maxtokensize=max(maxtokensize,structsize($3));
		  setfields(tp,$6);
		}
	| error
	;
structdecl
	: '{' structlist '}'
		{ $$ = lookstruct($2); }
	;
structlist
	: /* empty */
		{ $$=0; }
	| structlistel structlist
		{ $$=(list2) myalloc(sizeof(struct list2str));
		  $$->l2next = $2;
		  $$->l2list = $1;
		}
	;
structlistel
	: TYPENAME list1 ';'
		{ $$=(list1) myalloc(sizeof(struct list1str));
		  $$->l1next = $2;
		  $$->l1name = $1;
		}
	;

optcost : /* empty */
		{ $$.c_size = $$.c_time = 0; }
	| COST '=' '(' expr ',' expr ')'
		{ MUST2BEINT($4,$6);
		  $$.c_size = exp1;
		  $$.c_time = exp2;
		}
	;
optformat
	: /* empty */
		{ $$ = 0; }
	| STRING
	;

expressionsection
	: /* empty */
	| EXPRESSIONHEAD tokenexpressions
	;
tokenexpressions
	: tokenexpressionline
	| tokenexpressionline tokenexpressions
	;
tokenexpressionline
	: IDENT '=' tokenexpression
		{
		  {     register ident_p ip;

			chktabsiz(nmachsets,MAXSETS,"Expression table");
			machsets[nmachsets] = $3;
			ip=ilookup($1,ENTER);
			ip->i_type = IEXP;
			ip->i_i.i_expno = nmachsets++;
		  }
		}
	| error
	;
tokenexpression
	: PIDENT
		{ $$ = machprops[$1->i_i.i_prpno].propset; }
	| TIDENT
		{ register i;

		  for(i=0;i<SETSIZE;i++) $$.set_val[i]=0;
		  $$.set_val[($1->i_i.i_tokno+nmachregs+1)>>4] |=
			01<<(($1->i_i.i_tokno+nmachregs+1)&017);
		  $$.set_size = machtokens[$1->i_i.i_tokno].t_size;
		}
	| EIDENT
		{ $$=machsets[$1->i_i.i_expno]; }
	| tokenexpression '*' tokenexpression
		{ register i;

		  if (($$.set_size=$1.set_size)==0)
			$$.set_size = $3.set_size;
		  for (i=0;i<SETSIZE;i++)
			$$.set_val[i] = $1.set_val[i] & $3.set_val[i];
		}
	| tokenexpression '+' tokenexpression
		{ register i;

		  if ($1.set_size == -1)
			$$.set_size = $3.set_size;
		  else if ($3.set_size == -1)
			$$.set_size = $1.set_size;
		  else if ($1.set_size == $3.set_size)
			$$.set_size = $1.set_size;
		  else
			$$.set_size = 0;
		  for (i=0;i<SETSIZE;i++)
			$$.set_val[i] = $1.set_val[i] | $3.set_val[i];
		}
	| tokenexpression '-' tokenexpression
		{ register i;

		  if ($1.set_size == -1)
			$$.set_size = $3.set_size;
		  else if ($3.set_size == -1)
			$$.set_size = $1.set_size;
		  else if ($1.set_size == $3.set_size)
			$$.set_size = $1.set_size;
		  else
			$$.set_size = 0;
		  for (i=0;i<SETSIZE;i++)
			$$.set_val[i] = $1.set_val[i] & ~ $3.set_val[i];
		}
	| '(' tokenexpression ')'
		{ $$ = $2; }
	;

codesection
	: CODEHEAD coderules
	;
coderules
	: coderule
	| coderules coderule
	;
coderule
	: { nallreg=emrepllen=tokrepllen=0; }
		empattern SEP stackpattern SEP code SEP stackreplacement SEP
		emreplacement SEP cost
			{ int i;

			  if (emrepllen) {
				outbyte(DO_EMREPLACE+(emrepllen<<5));
				for (i=0;i<emrepllen;i++) {
					out(replmnem[i]);
					out(replexpr[i]);
				}
			  }
			  if ($8==0) {
				  outbyte(DO_TOKREPLACE+(tokrepllen<<5));
				  for(i=0;i<tokrepllen;i++)
					out(replinst[i]);
			  } else {
				static int warncount=0;
				if (!warncount++)
					fprintf(stderr,
		"WARNING: convert to stacksection, will disappear soon");
				outbyte(DO_TOKREPLACE);
			  }
			  if ($12.c_size!=0 || $12.c_time!=0) {
				  outbyte(DO_COST);
				  out($12.c_size);
				  out($12.c_time);
			  }
			  outbyte(empatlen==0? DO_RETURN : DO_NEXTEM);
			  fprintf(cfile,"\n");
			  ncoderules++;
			  maxallreg=max(maxallreg,nallreg);
			  if (empatlen==0) { /* coercion */
				if (tokrepllen<1 && $8==0)
					yyerror("No replacement in coercion");
				if (tokpatlen>1)
					yyerror("Token pattern too long");
				if ($8!=0) { /* stacking */
					c1_p cp;
					chktabsiz(nc1,MAXC1,"Coerc table 1");
					cp = &c1coercs[nc1++];
					cp->c1_texpno = pattokexp[1];
					cp->c1_prop = -1;
					cp->c1_codep = $6;
				} else if (tokrepllen>1) { /* splitting */
					c2_p cp;
					chktabsiz(nc2,MAXC2,"Coerc table 2");
					cp= &c2coercs[nc2++];
					cp->c2_texpno = pattokexp[1];
					cp->c2_nsplit = tokrepllen;
					maxsplit=max(maxsplit,tokrepllen);
					for (i=0;i<tokrepllen;i++)
						cp->c2_repl[i] = replinst[i];
					cp->c2_codep = $6;
					if (nallreg>0)
						yyerror("No allocates allowed here");
				} else { /* one to one coercion */
					c3_p cp;
					chktabsiz(nc3,MAXC3,"Coerc table 3");
					cp= &c3coercs[nc3++];
					if (tokpatlen)
						cp->c3_texpno = pattokexp[1];
					else
						cp->c3_texpno = 0;
					if (nallreg>1)
						yyerror("Too many allocates in coercion");
					cp->c3_prop = nallreg==0 ? 0 : allreg[0];
					cp->c3_repl = replinst[0];
					cp->c3_codep = $6;
				}
			  }
			}
	| error
	;
empattern
	: /* empty */
		{ empatlen=0; }
	| mnemlist optboolexpr
		{ register i;

		  empatexpr = $2;
		  patbyte(0);
		  patshort(prevind);
		  prevind = npatbytes - 3;
		  maxempatlen = max(empatlen,maxempatlen);
		  pat(empatlen);
		  for(i=1;i<=empatlen;i++)
			patbyte(patmnem[i]);
		  pat(empatexpr);
		  rulecount = npatbytes;
		  patbyte(1);   /* number of different rules with this pattern */
		  pat(codebytes);       /* first rule */
		}
	| ELLIPS
		{ pattern[rulecount]++;
		  maxrule= max(maxrule,pattern[rulecount]);
		  pat(codebytes);
		}
	;

mnemlist
	:       mnem
		{ empatlen = 1; patmnem[empatlen] = $1; }
	|       mnemlist mnem
		{ chktabsiz(empatlen+1,MAXEMPATLEN,"EM pattern");
		  patmnem[++empatlen] = $2;
		}
	;
mnem    :       IDENT
		{ if(strlen($1)!=3 || ($$=mlookup($1))==0)
			yyerror("not an EM-mnemonic");
		}
	;

stackpattern
	: optnocoerc tokenexpressionlist optstack
		{ register i;

		  if (tokpatlen != 0) {
			  outbyte(($1 ? ( $3 ? DO_XXMATCH: DO_XMATCH ) : DO_MATCH)+(tokpatlen<<5));
			  for(i=1;i<=tokpatlen;i++) {
				out(pattokexp[i]);
			  }
		  }
		  if ($3 && tokpatlen==0 && empatlen==0) {
			  outbyte(DO_COERC);
		  }
		  if ($3 && !$1 && empatlen!=0) {
			outbyte(DO_REMOVE);
			out(allexpno);
		  }
		}
	;

optnocoerc
	: /* empty */
		{ $$ = 0; }
	| NOCOERC ':'
		{ $$ = 1; }
	;

tokenexpressionlist
	: /* empty */
		{ tokpatlen = 0; }
	| tokenexpressionlist tokenexpressionno
		{ chktabsiz(tokpatlen+1,MAXPATLEN,"Token pattern");
		  pattokexp[++tokpatlen] = $2;
		  if (machsets[$2].set_size==0)
			yyerror("Various sized set in tokenpattern");
		}
	;

tokenexpressionno
	: tokenexpression
		{ $$ = exprlookup($1); }
	;

optstack
	:       /* empty */
		{ $$ = 0; }
	|       STACK
		{ $$ = 1; }
	;

code    :
		{ $$ = codebytes; cchandled=ccspoiled=0; }
	  initcode restcode
		{ if (cchandled==0 && ccspoiled!=0) {
			outbyte(DO_ERASE);
			out(ccregexpr);
		  }
		}
	;

initcode
	: /* empty */
	| initcode remove
	| initcode allocate
	;
remove
	: REMOVE '(' tokenexpressionno
		{ curtokexp = $3; }
	  optcommabool ')'
		{ outbyte(DO_REMOVE+ ($5!=0 ? 32 : 0));
		  out($3);
		  if ($5!=0) out($5);
		}
	| REMOVE '(' expr ')'
		{ if ($3.expr_typ != TYPREG)
			yyerror("Expression must be register");
		  outbyte(DO_RREMOVE);
		  out($3.expr_index);
		}
	;
optcommabool
	: /* empty */
		{ $$ = 0; }
	| ',' expr
		{ MUST1BEBOOL($2);
		  $$ = exp1;
		}
	;

restcode: /* empty */
	| restcode LSTRING expr
		{ outbyte(DO_LOUTPUT);
		  out(stringno($2));
		  free($2);
		  out($3.expr_index);
		  ccspoiled++;
		}
	| restcode stringlist
		{ int i;
		  for(i=0;nstr>0;i++,nstr--) {
			if (i%8==0) outbyte(DO_ROUTPUT+(nstr>7 ? 7 : nstr-1)*32);
			out(strar[i]);
		  }
		  ccspoiled++;
		}
	| restcode RETURN
		{ outbyte(DO_PRETURN); }
	| restcode move
	| restcode erase
	| restcode NOCC
		{ outbyte(DO_ERASE);
		  out(ccregexpr);
		  cchandled++;
		}
	| restcode SAMECC
		{ cchandled++; }
	| restcode SETCC '(' tokeninstanceno ')'
		{ outbyte(DO_MOVE);
		  out(ccinstanceno);
		  out($4);
		  cchandled++;
		}
	| restcode TEST '(' tokeninstanceno ')'
		{ outbyte(DO_MOVE);
		  out($4);
		  out(ccinstanceno);
		  ccspoiled=0;
		}
	;

stringlist
	: STRING
		{ nstr=1;
		  strar[0]=stringno($1);
		  free($1);
		}
	| stringlist STRING
		{ chktabsiz(nstr,MAXNSTR,"Consecutiv strings");
		  strar[nstr++] = stringno($2);
		  free($2);
		}
	;

move
	: MOVE '(' tokeninstanceno ',' tokeninstanceno ')'
		{ outbyte(DO_MOVE);
		  out($3);
		  out($5);
		}
	;

erase
	: ERASE '(' expr ')'
		{ outbyte(DO_ERASE);
		  out($3.expr_index);
		  if($3.expr_typ != TYPREG)
			yyerror("Bad argument of erase");
		}
	;

allocate
	: ALLOCATE { dealflag=0; } '(' alloclist ')'
		{ if (dealflag)
			outbyte(DO_REALLOCATE);
		}
	;


alloclist
	: allocel
	| alloclist optcomma allocel
	;

allocel
	: tokeninstanceno       /* deallocate */
		{ outbyte(DO_DEALLOCATE);
		  out($1);
		  dealflag++;
		}
	| PIDENT
		{ allreg[nallreg++] = $1->i_i.i_prpno;
		  outbyte(DO_ALLOCATE);
		  out($1->i_i.i_prpno);
		}
	| PIDENT '=' tokeninstanceno
		{ allreg[nallreg++] = $1->i_i.i_prpno;
		  outbyte(DO_ALLOCATE+32);
		  out($1->i_i.i_prpno);
		  out($3);
		}
	;

stackreplacement
	: /* empty */
		{ $$=0; }
	| STACK
		{ $$=1; }
	| '{' STACK '}'
		{ $$=1; }
	| stackrepllist
		{ $$=0; }
	;
stackrepllist
	: tokeninstanceno
		{ tokrepllen=1; replinst[0] = $1; }
	| stackrepllist tokeninstanceno
		{ chktabsiz(tokrepllen+1,MAXPATLEN,"Stack replacement");
		  replinst[tokrepllen++] = $2;
		}
	;

emreplacement
	: /* empty, normal case */
	| emrepllist
	;
emrepllist
	: mnem optexpr
		{ emrepllen=1;
		  replmnem[0]=$1;
		  replexpr[0]=$2.expr_index;
		}
	| emrepllist mnem optexpr
		{ chktabsiz(emrepllen+1,MAXEMPATLEN,"EM replacement");
		  replmnem[emrepllen]=$2;
		  replexpr[emrepllen]=$3.expr_index;
		  emrepllen++;
		}
	;

cost    : /* empty */
		{ $$.c_size = $$.c_time = 0;
		}
	| '(' expr ',' expr ')'
		{ MUST2BEINT($2,$4);
		  $$.c_size = exp1;
		  $$.c_time = exp2;
		}
	| cost '+' '%' '[' tokargno ']'
		{ $$.c_size = lookup(1,EX_PLUS,$1.c_size,
					lookup(0,EX_COST,$5,0));
		  $$.c_time = lookup(1,EX_PLUS,$1.c_time,
					lookup(0,EX_COST,$5,1));
		}
	;

movesection
	: MOVEHEAD movedefs
	;

movedefs
	: movedef
	| movedefs movedef
	;

movedef
	: '(' tokenexpressionno
		{ curtokexp = $2; }
	  optboolexpr ',' tokenexpressionno
		{ curtokexp = $6;
		  pattokexp[1] = $2;
		  pattokexp[2] = $6;
		  tokpatlen=2;
		}
	  optboolexpr ',' code optcommacost ')'
		{ register move_p mp;

		  outbyte(DO_RETURN);
		  fprintf(cfile,"\n");
		  chktabsiz(nmoves,NMOVES,"Move definition table");
		  mp = &machmoves[nmoves++];
		  mp->m_set1 = $2;
		  mp->m_expr1= $4;
		  mp->m_set2 = $6;
		  mp->m_expr2= $8;
		  mp->m_cindex=$10;
		  mp->m_cost = $11;
		}
	| error
	;

testsection
	: /* empty */
	| TESTHEAD testdefs
	;

testdefs: testdef
	| testdefs testdef
	;

testdef : '(' tokenexpressionno
		{ curtokexp = $2;
		  pattokexp[1] = $2;
		  pattokexp[2] = cocosetno;
		  tokpatlen=2;
		}
	  optboolexpr ',' code optcommacost ')'
		{ register move_p mp;

		  outbyte(DO_RETURN);
		  fprintf(cfile,"\n");
		  chktabsiz(nmoves,NMOVES,"Move definition table(tests)");
		  mp = &machmoves[nmoves++];
		  mp->m_set1 = $2;
		  mp->m_expr1 = $4;
		  mp->m_set2 = cocosetno;
		  mp->m_expr2 = 0;
		  mp->m_cindex = $6;
		  mp->m_cost = $7;
		}
	;

stacksection
	: STACKHEAD stackdefs
	| /* empty */
	;
stackdefs
	: stackdef
	| stackdefs stackdef
	;
stackdef
	: '(' tokenexpressionno
		{ curtokexp = $2;
		  pattokexp[1] = $2;
		  tokpatlen=1;
		}
	  optboolexpr ',' optprop ',' code optcommacost ')'
		{ register c1_p cp;

		  outbyte(DO_TOKREPLACE);
		  outbyte(DO_RETURN);
		  fprintf(cfile,"\n");
		  chktabsiz(nc1,MAXC1,"Stacking table");
		  cp = &c1coercs[nc1++];
		  cp->c1_texpno = $2;
		  cp->c1_expr = $4;
		  cp->c1_prop = $6;
		  cp->c1_codep = $8;
		  cp->c1_cost = $9;
		}
	;

optprop
	: /* empty */
		{ $$ = -1; }
	| PIDENT 
		{ $$ = $1->i_i.i_prpno; }
	;

optcommacost
	: /* empty */
		{ $$.c_size = 0; $$.c_time = 0;}
	| ',' cost
		{ $$ = $2; }
	;

list1   : /* empty */
		{ $$ = 0; }
	| optcomma IDENT list1
		{ $$=(list1) myalloc(sizeof(struct list1str));
		  $$->l1next = $3;
		  $$->l1name = $2;
		}
	;
optcomma: /* nothing */
	| ','
	;
emargno : NUMBER
		{ if ($1<1 || $1>empatlen)
			yyerror("Number after $ out of range");
		  $$ = $1;
		}
	;
tokargno
	: NUMBER
		{ if ($1<1 || $1>tokpatlen)
			yyerror("Number within %[] out of range");
		  $$ = $1;
		}
	;
expr    : '$' emargno
		{ $$.expr_index = lookup(0,EX_ARG,$2,0); $$.expr_typ = argtyp(patmnem[$2]);
		}
	| NUMBER
		{ $$.expr_index = lookup(0,EX_CON,(int)($1&0177777),(int)($1>>16));
		  $$.expr_typ = TYPINT;
		}
	| STRING
		{ $$.expr_index = lookup(0,EX_STRING,strlookup($1),0);
		  $$.expr_typ = TYPSTR;
		}
	| RIDENT
		{ $$.expr_index = lookup(0,EX_REG,$1->i_i.i_regno,0);
		  $$.expr_typ = TYPREG;
		}
	| '%' '[' tokargno '.' IDENT ']'
		{ $$.expr_index = lookup(0,EX_TOKFIELD,$3,
		     findstructel(pattokexp[$3],$5,&$$.expr_typ));
		}
	| '%' '[' tokargno subreg ']'
		{ chkregexp(pattokexp[$3]);
		  $$.expr_index = lookup(0,EX_SUBREG,$3,$4);
		  $$.expr_typ = TYPREG;
		}
	| '%' '[' LCASELETTER subreg ']'
		{ if ($3 >= 'a'+nallreg)
			yyerror("Bad letter in %[x] construct");
		  $$.expr_index = lookup(0,EX_ALLREG,$3-'a'+1,$4);
		  $$.expr_typ = TYPREG;
		}
	| '%' '[' IDENT ']'
		{ $$.expr_index = lookup(0,EX_TOKFIELD,0,
		     findstructel(curtokexp,$3,&$$.expr_typ));
		}
	| TOSTRING '(' expr ')'
		{ MUST1BEINT($3);
		  $$.expr_index = lookup(0,EX_TOSTRING,exp1,0);
		  $$.expr_typ = TYPSTR;
		}
	| DEFINED '(' expr ')'
		{ $$.expr_index = lookup(0,EX_DEFINED,$3.expr_index,0);
		  $$.expr_typ = TYPBOOL;
		}
	| SAMESIGN '(' expr ',' expr ')'
		{ MUST2BEINT($3,$5);
		  $$.expr_index = lookup(1,EX_SAMESIGN,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| SFIT '(' expr ',' expr ')'
		{ MUST2BEINT($3,$5);
		  $$.expr_index = lookup(0,EX_SFIT,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| UFIT '(' expr ',' expr ')'
		{ MUST2BEINT($3,$5);
		  $$.expr_index = lookup(0,EX_UFIT,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| ROM '(' emargno ',' NUMBER ')'
		{ if ($5<1 || $5>3)
			yyerror("Second argument of rom must be >=1 and <=3");
		  $$.expr_index = lookup(0,EX_ROM,$3-1,$5-1);
		  $$.expr_typ = TYPINT;
		}
	| LOWW '(' emargno ')'
		{
		  $$.expr_index = lookup(0,EX_LOWW,$3-1,0);
		  $$.expr_typ = TYPINT;
		}
	| HIGHW '(' emargno ')'
		{
		  $$.expr_index = lookup(0,EX_HIGHW,$3-1,0);
		  $$.expr_typ = TYPINT;
		}
	| '(' expr ')'
		{ $$ = $2; }
	| expr CMPEQ expr
		{ switch(commontype($1,$3)) {
		  case TYPINT:
			$$.expr_index = lookup(1,EX_NCPEQ,$1.expr_index,$3.expr_index);
			break;
		  case TYPSTR:
			$$.expr_index = lookup(1,EX_SCPEQ,$1.expr_index,$3.expr_index);
			break;
		  case TYPREG:
			$$.expr_index = lookup(1,EX_RCPEQ,$1.expr_index,$3.expr_index);
			break;
		  }
		  $$.expr_typ = TYPBOOL;
		}
	| expr CMPNE expr
		{ switch(commontype($1,$3)) {
		  case TYPINT:
			$$.expr_index = lookup(1,EX_NCPNE,$1.expr_index,$3.expr_index);
			break;
		  case TYPSTR:
			$$.expr_index = lookup(1,EX_SCPNE,$1.expr_index,$3.expr_index);
			break;
		  case TYPREG:
			$$.expr_index = lookup(1,EX_RCPNE,$1.expr_index,$3.expr_index);
			break;
		  }
		  $$.expr_typ = TYPBOOL;
		}
	| expr CMPGT expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_NCPGT,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr CMPGE expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_NCPGE,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr CMPLT expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_NCPLT,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr CMPLE expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_NCPLE,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr OR2 expr
		{ MUST2BEBOOL($1,$3);
		  $$.expr_index = lookup(0,EX_OR2,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr AND2 expr
		{ MUST2BEBOOL($1,$3);
		  $$.expr_index = lookup(0,EX_AND2,exp1,exp2);
		  $$.expr_typ = TYPBOOL;
		}
	| expr '+' expr
		{ switch(commontype($1,$3)) {
		  case TYPINT:
			$$.expr_index = lookup(1,EX_PLUS,$1.expr_index,$3.expr_index);
			break;
		  case TYPSTR:
			$$.expr_index = lookup(0,EX_CAT,$1.expr_index,$3.expr_index);
			break;
		  default:
			yyerror("Bad types");
		  }
		  $$.expr_typ = $1.expr_typ;
		}
	| expr '-' expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_MINUS,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| expr '*' expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(1,EX_TIMES,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| expr '/' expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_DIVIDE,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| expr '%' expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_MOD,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| expr LSHIFT expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_LSHIFT,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| expr RSHIFT expr
		{ MUST2BEINT($1,$3);
		  $$.expr_index = lookup(0,EX_RSHIFT,exp1,exp2);
		  $$.expr_typ = TYPINT;
		}
	| NOT expr
		{ MUST1BEBOOL($2);
		  $$.expr_index = lookup(0,EX_NOT,exp1,0);
		  $$.expr_typ = TYPBOOL;
		}
	| COMP expr
		{ MUST1BEINT($2);
		  $$.expr_index = lookup(0,EX_COMP,exp1,0);
		  $$.expr_typ = TYPINT;
		}
	| INREG '(' expr ')'
		{ MUST1BEINT($3);
		  $$.expr_index = lookup(0,EX_INREG,exp1,0);
		  $$.expr_typ = TYPINT;
		}
	| REGVAR '(' expr ')'
		{ MUST1BEINT($3);
		  $$.expr_index = lookup(0,EX_REGVAR,exp1,0);
		  $$.expr_typ = TYPREG;
		}
/*
	| '-' expr %prec UMINUS
		{ MUST1BEINT($2);
		  $$.expr_index = lookup(0,EX_UMINUS,exp1,0);
		  $$.expr_typ = TYPINT;
		}
*/
	;

subreg  : /* empty */
		{ $$=0; }
	| '.' NUMBER
		{ $$=$2; }
	;

optboolexpr
	: /* empty */
		{ $$ = 0; }
	| expr
		{ MUST1BEBOOL($1);
		  $$=exp1;
		}
	;
optexpr
	: /* empty */
		{ $$.expr_typ=0;
		  $$.expr_index=0;
		}
	| expr
	;

tokeninstanceno
	: tokeninstance
		{ $$ = instno($1); }
	;

tokeninstance
	: '%' '[' tokargno subreg ']'
		{ register i;

		  if ($4!=0)
			  chkregexp(pattokexp[$3]);
		  $$.in_which = IN_COPY;
		  $$.in_info[0] = $3;
		  $$.in_info[1] = $4;
		  for (i=2;i<TOKENSIZE;i++)
			$$.in_info[i] = 0;
		}
	| '%' '[' tokargno '.' IDENT ']'
		{ int typ;
		  register i;
		  $$.in_which = IN_COPY;
		  $$.in_info[0] = $3;
		  $$.in_info[1] = findstructel(pattokexp[$3],$5,&typ);
		  if (typ != TYPREG)
			yyerror("Must be register");
		  for (i=2;i<TOKENSIZE;i++)
			$$.in_info[i] = 0;
		}
	| RIDENT
		{ register i;
		  $$.in_which = IN_RIDENT;
		  $$.in_info[0] = $1->i_i.i_regno;
		  for (i=1;i<TOKENSIZE;i++)
			$$.in_info[i] = 0;
		}
	| REGVAR '(' expr ')'
		{ register i;
		  MUST1BEINT($3);
		  $$.in_which = IN_REGVAR;
		  $$.in_info[0] = exp1;
		  for (i=1;i<TOKENSIZE;i++)
			$$.in_info[i] = 0;
		}
	| '%' '[' LCASELETTER subreg ']'
		{ register i;
		  if ($3 >= 'a'+nallreg)
			yyerror("Bad letter in %[x] construct");
		  $$.in_which = IN_ALLOC;
		  $$.in_info[0] = $3-'a';
		  $$.in_info[1] = $4;
		  for (i=2;i<TOKENSIZE;i++)
			$$.in_info[i] = 0;
		}
	| '{' TIDENT attlist '}'
		{ register i;
		  $$.in_which = IN_DESCR;
		  $$.in_info[0] = $2->i_i.i_tokno;
		  for(i=0;i<narexp;i++) {
			if (arexp[i].expr_typ !=
			      machtokens[$2->i_i.i_tokno].t_fields[i].t_type)
				yyerror("Attribute %d has wrong type",i+1);
			$$.in_info[i+1] = arexp[i].expr_index;
		  }
		  for (i=narexp+1;i<TOKENSIZE;i++) {
			if (machtokens[$2->i_i.i_tokno].t_fields[i-1].t_type!=0)
				yyerror("Too few attributes");
			$$.in_info[i] = 0;
		  }
		}
	;

attlist
	: /* empty */
		{ narexp = 0; }
	| attlist ',' expr
		{ arexp[narexp++] = $3; }
	;

%%

char * myalloc(n) {
	register char *p;

	p= (char*) malloc(n);
	if (p==0) {
		yyerror("Out of core");
		exit(1);
	}
	return(p);
}

tstint(e) expr_t e; {

	if(e.expr_typ != TYPINT)
		yyerror("Must be integer expression");
}

tstbool(e) expr_t e; {

	if(e.expr_typ != TYPBOOL)
		yyerror("Must be boolean expression");
}

structsize(s) register list2 s; {
	register list1 l;
	register sum;

	sum = 0;
	while ( s != 0 ) {
		l = s->l2list->l1next;
		while ( l != 0 ) {
			sum++;
			l = l->l1next;
		}
		s = s->l2next;
	}
	return(sum);
}

list2 lookstruct(ll) list2 ll; {
	list3 l3;
	list2 l21,l22;
	list1 l11,l12;

	for (l3=structpool;l3 != 0;l3=l3->l3next) {
		for (l21=l3->l3list,l22=ll;l21!=0 && l22!=0;
		     l21=l21->l2next,l22=l22->l2next) {
			for(l11=l21->l2list,l12=l22->l2list;
			    l11!=0 && l12!=0 && strcmp(l11->l1name,l12->l1name)==0;
			    l11=l11->l1next,l12=l12->l1next)
				;
			if (l11!=0 || l12!=0)
				goto contin;
		}
		if(l21==0 && l22==0)
			return(l3->l3list);
		contin:;
	}
	l3 = (list3) myalloc(sizeof(struct list3str));
	l3->l3next=structpool;
	l3->l3list=ll;
	structpool=l3;
	return(ll);
}

instno(inst) inst_t inst; {
	register i,j;

	for(i=1;i<narinstance;i++) {
		if (arinstance[i].in_which != inst.in_which)
			continue;
		for(j=0;j<TOKENSIZE;j++)
			if(arinstance[i].in_info[j] != inst.in_info[j])
				goto cont;
		return(i);
	cont:;
	}
	chktabsiz(narinstance,MAXINSTANCE,"Instance table");
	arinstance[narinstance] = inst;
	return(narinstance++);
}

string scopy(s) string s; {
	register string t;

	t = (char *) myalloc(strlen(s)+1);
	strcpy(t,s);
	return(t);
}

strlookup(s) string s; {
	register i;

	for(i=0;i<ncodestrings;i++)
		if(strcmp(s,codestrings[i])==0)
			return(i);
	chktabsiz(ncodestrings,MAXSTRINGS,"string table");
	codestrings[ncodestrings] = scopy(s);
	return(ncodestrings++);
}

stringno(s) register string s; {
	char buf[256];
	register char *p=buf;

	while(*s != 0) switch(*s) {
	default:
		*p++ = *s++;
		continue;
	case '$':
		s++;
		switch(*s) {
		default:
			yyerror("Bad character after $ in codestring");
		case '$':
			*p++ = *s++;
			continue;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			*p++ = argtyp(patmnem[*s-'0']) == TYPINT ?
				PR_EMINT : PR_EMSTR;
			*p++ = *s++ -'0';
			continue;
		}
	case '%':
		s++;
		if (*s != '[') {
			if(*s == '%') {
				*p++ = *s++;
				continue;
			} else
				yyerror("Bad character following %% in codestring");
		} else
			s++;
		if(isdigit(*s)) {
			int num;
			num = *s++ - '0';
			if (num<1 || num>tokpatlen)
				yyerror("Number within %[] out of range");
			if (*s == ']') {
				s++;
				*p++ = PR_TOK;
				*p++ = num;
			} else if (*s++ != '.')
				yyerror("Bad character following %%[digit in codestring");
			else {
				char field[256];
				register char *f=field;
				int type,offset;

				while( *s != ']' && *s != 0)
					*f++ = *s++;
				*f++ = 0;
				if (*s != ']')
					yyerror("Unterminated %[] construction in codestring");
				else
					s++;
				if (isdigit(field[0])) {
					chkregexp(pattokexp[num]);
					*p++ = PR_SUBREG;
					*p++ = num;
					*p++ = atoi(field);
				} else {
					offset = findstructel(pattokexp[num],field,&type);
					*p++ = PR_TOKFLD;
					*p++ = num;
					*p++ = offset;
				}
			}
		} else if (*s >= 'a' && *s < 'a'+nallreg) {
			int reg,subreg;
			reg = *s++ -'a'+1;
			if(*s == ']')
				subreg = 255;
			else {
				if (*s != '.')
					yyerror("Bad character following %%[x in codestring");
				s++;
				if(!isdigit(*s))
					yyerror("Bad character following %%[x. in codestring");
				subreg = *s - '0';
				s++;
				if(*s != ']')
					yyerror("Bad character following %%[x.y in codestring");
			}
			s++;
			*p++ = PR_ALLREG;
			*p++ = reg;
			*p++ = subreg;
		} else
			yyerror("Bad character following %%[ in codestring");
	}
	*p++ = 0;
	return(strlookup(buf));
}

tabovf(tablename) string tablename; {
	char buf[256];

	sprintf(buf,"%s overflow",tablename);
	yyerror(buf);
	exit(-1);
}

main(argc,argv) char *argv[]; {

	if (argc!=1) {
		fprintf(stderr,"%s is a filter, don't use arguments\n",argv[0]);
		exit(-1);
	}
	inithash();
	initio();
	inittables();
	yyparse();
	if (nerrors==0) {
		compueq();
		hashpatterns();
		finishio();
		verbose();
	}
	debug();
	exit(nerrors);
}

lookup(comm,operator,lnode,rnode) {
	register node_p p;

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

compueq() {
	register i,j;

	for (i=1;i<nmachregs;i++) {
		for (j=1;j<i;j++)
			if (eqregclass(i,j)) {
				stregclass[i] = stregclass[j];
				break;
			}
		if (j==i)
			stregclass[i] = nregclasses++;
	}
}

eqregclass(r1,r2) {
	register reginfo rp1,rp2;
	register i;
	short regbits[(MAXREGS+15)>>4];
	int member;

	rp1 = machregs[r1]; rp2 = machregs[r2];
	for (i=0;i<((nprops+15)>>4);i++)
		if (rp1->rprop[i] != rp2->rprop[i])
			return(0);
	for (i=0;i<((MAXREGS+15)>>4);i++)
		regbits[i] = 0;
	for (i=0;i<maxmembers;i++) {
		if (member = rp1->rmembers[i])
			regbits[member>>4] |= (1<<(member&017));
	}
	for (i=0;i<maxmembers;i++) {
		member = rp2->rmembers[i];
		if (regbits[member>>4]&(1<<(member&017)))
			return(0);
	}
	return(1);
}

unsigned hash(name) register string name; {
	register unsigned sum;
	register i;

	for (sum=i=0;*name;i+=3)
		sum ^= (*name++)<<(i&07);
	return(sum);
}

ident_p ilookup(name,enterf) string name; int enterf; {
	register ident_p p,*pp;

	pp = &identtab[hash(name)%ITABSIZE];
	while (*pp != 0) {
		if (strcmp((*pp)->i_name,name)==0)
			if (enterf != ENTER)
				return(*pp);
			else
				yyerror("Multiply defined symbol");
		pp = &(*pp)->i_next;
	}
	if (enterf == LOOKUP)
		yyerror("Undefined symbol");
	if (enterf == JUSTLOOKING)
		return(0);
	p = *pp = (ident_p) myalloc(sizeof(ident_t));
	p->i_name = name;
	p->i_next = 0;
	p->i_type = 0;
	return(p);
}

initio() {

	if ((cfile=fopen("tables.c","w"))==NULL) {
		fprintf(stderr,"Can't create tables.c\n");
		exit(-1);
	}
	if ((hfile=fopen("tables.h","w"))==NULL) {
		fprintf(stderr,"Can't create tables.h\n");
		exit(-1);
	}
	fprintf(cfile,"#include \"param.h\"\n");
	fprintf(cfile,"#include \"tables.h\"\n");
	fprintf(cfile,"#include \"types.h\"\n");
	fprintf(cfile,"#include <cg_pattern.h>\n");
	fprintf(cfile,"#include \"data.h\"\n");
	fprintf(cfile,"\nbyte coderules[] = {\n");
	patbyte(0);
}

exprlookup(sett) set_t sett; {
	register i,j,ok;

	for(i=0;i<nmachsets;i++) {
		ok= (sett.set_size == machsets[i].set_size);
		for(j=0;j<SETSIZE;j++) {
			if (sett.set_val[j] == machsets[i].set_val[j])
				continue;
			ok=0;
			break;
		}
		if (ok)
			return(i);
	}
	chktabsiz(nmachsets,MAXSETS,"Expression table");
	machsets[nmachsets] = sett;
	return(nmachsets++);
}

inittables() {
	register reginfo r;
	register i;
	inst_t inst;
	set_t sett;

	nodes[0].ex_operator=EX_CON;
	nodes[0].ex_lnode=0;
	nodes[0].ex_rnode=0;
	cocopropno=nprops++;
	r=(reginfo)myalloc(sizeof(struct reginfo));
	r->rname = "cc reg";
	r->rrepr = "CC";
	r->rsize = -1;
	r->rregvar= -1;
	for(i=0;i<MAXMEMBERS;i++)
		r->rmembers[i] = 0;
	for(i=0;i<PROPSETSIZE;i++)
		r->rprop[i] = 0;
	r->rprop[cocopropno>>4] |= (1<<(cocopropno&017));
	chktabsiz(nmachregs,MAXREGS,"Register table");
	machregs[nmachregs++]  = r;
	inst.in_which = IN_RIDENT;
	inst.in_info[0] = nmachregs-1;
	for(i=1;i<TOKENSIZE;i++)
		inst.in_info[i]=0;
	ccinstanceno=instno(inst);
	ccregexpr=lookup(0,EX_REG,nmachregs-1,0);
	sett.set_size=0;
	for (i=0;i<SETSIZE;i++)
		sett.set_val[i]=0;
	sett.set_val[nmachregs>>4] |= (01<<(nmachregs&017));
	cocosetno=exprlookup(sett);
}

outregs() {
	register i,j,k;
	static short rset[(MAXREGS+15)>>4];
	int t,ready;

	fprintf(cfile,"char stregclass[] = {\n");
	for (i=0;i<nmachregs;i++)
		fprintf(cfile,"\t%d,\n",stregclass[i]);
	fprintf(cfile,"};\n\nstruct reginfo machregs[] = {\n{0},\n");
	for (i=1;i<nmachregs;i++) {
		fprintf(cfile,"{%d,%d",strlookup(machregs[i]->rrepr),
			machregs[i]->rsize);
		if (maxmembers!=0) {
			fprintf(cfile,",{");
			for(j=0;j<maxmembers;j++)
				fprintf(cfile,"%d,",machregs[i]->rmembers[j]);
			/* now compute and print set of registers
			 * that clashes with this register.
			 * A register clashes with al its children (and theirs)
			 * and with all their parents.
			 */
			for (j=0;j<((MAXREGS+15)>>4);j++)
				rset[j]=0;
			rset[i>>4] |= (1<<(i&017));
			do {
			    ready=1;
			    for (j=1;j<nmachregs;j++)
				if (rset[j>>4]&(1<<(j&017)))
				    for (k=0;k<maxmembers;k++)
					if ((t=machregs[j]->rmembers[k])!=0) {
					    if ((rset[t>>4]&(1<<(t&017)))==0)
						ready=0;
					    rset[t>>4] |= (1<<(t&017));
					}
			} while (!ready);
			do {
			    ready=1;
			    for (j=1;j<nmachregs;j++)
				for (k=0;k<maxmembers;k++)
				    if ((t=machregs[j]->rmembers[k])!=0)
					if (rset[t>>4]&(1<<(t&017))) {
						if (rset[j>>4]&(1<<(j&017))==0)
						    ready=0;
						rset[j>>4] |= (1<<(j&017));
					}
			} while (!ready);
			fprintf(cfile,"},{");
			for (j=0;j<((nmachregs+15)>>4);j++)
				fprintf(cfile,"%d,",rset[j]);
			fprintf(cfile,"}");
		}
		if (machregs[i]->rregvar>=0)
			fprintf(cfile,",1");
		fprintf(cfile,"},\n");
	}
	fprintf(cfile,"};\n\n");
}

finishio() {
	register i;
	register node_p np;
	int j;
	int setsize;
	register move_p mp;

	fprintf(cfile,"};\n\n");
	if (wsize>0)
		fprintf(hfile,"#define EM_WSIZE %d\n",wsize);
	else
		yyerror("Wordsize undefined");
	if (psize>0)
		fprintf(hfile,"#define EM_PSIZE %d\n",psize);
	else
		yyerror("Pointersize undefined");
	if (bsize>=0)
		fprintf(hfile,"#define EM_BSIZE %d\n",bsize);
	else
		fprintf(hfile,"extern int EM_BSIZE;\n");
	if (fmt!=0)
		fprintf(hfile,"#define WRD_FMT \"%s\"\n",fmt);
	fprintf(hfile,"#define MAXALLREG %d\n",maxallreg);
	setsize = (nmachregs+1 + nmachtokens + 15)>>4;
	fprintf(hfile,"#define SETSIZE %d\n",setsize);
	fprintf(hfile,"#define NPROPS %d\n",nprops);
	fprintf(hfile,"#define NREGS %d\n",nmachregs);
	fprintf(hfile,"#define REGSETSIZE %d\n",(nmachregs+15)>>4);
	fprintf(hfile,"#define TOKENSIZE %d\n",maxtokensize);
	fprintf(hfile,"#define MAXMEMBERS %d\n",maxmembers);
	fprintf(hfile,"#define LONGESTPATTERN %d\n",maxempatlen);
	fprintf(hfile,"#define MAXRULE %d\n",maxrule);
	fprintf(hfile,"#define NMOVES %d\n",nmoves);
	fprintf(hfile,"#define NC1 %d\n",nc1);
	if (nc2) {
		assert(maxsplit!=0);
		fprintf(hfile,"#define NC2 %d\n",nc2);
		fprintf(hfile,"#define MAXSPLIT %d\n",maxsplit);
	}
	fprintf(hfile,"#define NC3 %d\n",nc3);
	outregs();
	fprintf(cfile,"tkdef_t tokens[] = {\n");
	for(i=0;i<nmachtokens;i++) {
		fprintf(cfile,"{%d,{%d,%d},{",machtokens[i].t_size,
				machtokens[i].t_cost.c_size,
				machtokens[i].t_cost.c_time);
		for(j=0;j<maxtokensize;j++)
			fprintf(cfile,"%d,",machtokens[i].t_fields[j].t_type);
		fprintf(cfile,"},%d},\n",machtokens[i].t_format);
	}
	fprintf(cfile,"};\n\nnode_t enodes[] = {\n");
	for(np=nodes;np<lastnode;np++)
		fprintf(cfile,"{%d,%d,%d},\n",np->ex_operator,np->ex_lnode,
				np->ex_rnode);
	fprintf(cfile,"};\n\nstring codestrings[] = {\n");
	for(i=0;i<ncodestrings;i++) {
		register char *p;
		p=codestrings[i];
		fprintf(cfile,"\t\"");
		while (*p) {
			fprintf(cfile, !isascii(*p) || iscntrl(*p) ? "\\%03o" : "%c", (*p)&BMASK);
			p++;
		}
		fprintf(cfile,"\",\n");
	}
	fprintf(cfile,"};\n\nset_t machsets[] = {\n");
	for(i=0;i<nmachsets;i++) {
		fprintf(cfile,"{%d,{",machsets[i].set_size);
		for(j=0;j<setsize;j++)
			fprintf(cfile,"0%o,",machsets[i].set_val[j]);
		fprintf(cfile,"}},\n");
	}
	fprintf(cfile,"};\n\ninst_t tokeninstances[] = {\n");
	for(i=0;i<narinstance;i++) {
		fprintf(cfile,"{ %d, {",arinstance[i].in_which);
		for(j=0;j<=maxtokensize;j++)
			fprintf(cfile,"%d,",arinstance[i].in_info[j]);
		fprintf(cfile,"}},\n");
	}
	fprintf(cfile,"};\n\nmove_t moves[] = {\n");
	for (i=0;i<nmoves;i++) {
		mp = &machmoves[i];
		fprintf(cfile,"{%d,%d,%d,%d,%d,{%d,%d}},\n",
			mp->m_set1, mp->m_expr1,
			mp->m_set2, mp->m_expr2,
			mp->m_cindex,
			mp->m_cost.c_size,mp->m_cost.c_time);
	}
	fprintf(cfile,"};\n\nbyte pattern[] = {\n");
	for (i=0;i<npatbytes;i++) {
		fprintf(cfile,"%3d,",pattern[i]&BMASK);
		if ((i%10)==9)
			fprintf(cfile,"\n");
	}
	fprintf(cfile,"\n};\n\nint pathash[256] = {\n");
	for(i=0;i<256;i++) {
		fprintf(cfile,"%6d,",pathash[i]);
		if((i&07)==07)
			fprintf(cfile,"\n");
	}
	fprintf(cfile,"};\n\nc1_t c1coercs[] = {\n");
	for (i=0;i<nc1;i++)
		fprintf(cfile,"{%d,%d,%d,%d,{%d,%d}},\n",
			c1coercs[i].c1_texpno,
			c1coercs[i].c1_expr,
			c1coercs[i].c1_prop,
			c1coercs[i].c1_codep,
			c1coercs[i].c1_cost.c_size,
			c1coercs[i].c1_cost.c_time);
	if (nc2)
		fprintf(cfile,"};\n\nc2_t c2coercs[] = {\n");
	for (i=0;i<nc2;i++) {
		fprintf(cfile,"{%d,%d,{",
			c2coercs[i].c2_texpno,
			c2coercs[i].c2_nsplit);
		for (j=0;j<maxsplit;j++)
			fprintf(cfile,"%d,",c2coercs[i].c2_repl[j]);
		fprintf(cfile,"},%d},\n",c2coercs[i].c2_codep);
	}
	fprintf(cfile,"};\n\nc3_t c3coercs[] = {\n");
	for (i=0;i<nc3;i++)
		fprintf(cfile,"{%d,%d,%d,%d},\n",
			c3coercs[i].c3_texpno,
			c3coercs[i].c3_prop,
			c3coercs[i].c3_repl,
			c3coercs[i].c3_codep);
	fprintf(cfile,"};\n\n");
	for (i=0;i<nprops;i++) {
		fprintf(cfile,"struct reginfo *rlist%02d[] = {\n",i);
		for (j=2;j<=nmachregs;j++) {
			if (machregs[j-1]->rregvar<0 && 
			    (machprops[i].propset.set_val[j>>4]&(1<<(j&017))))
				fprintf(cfile,"\t&machregs[%d],\n",j-1);
		}
		fprintf(cfile,"\t0\n};\n");
	}
	fprintf(cfile,"struct reginfo **reglist[] = {\n");
	for (i=0;i<nprops;i++) {
		fprintf(cfile,"\trlist%02d,\n",i);
	}
	fprintf(cfile,"};\n");
	fprintf(cfile,"unsigned cc1 = %u;\n",cc1);
	fprintf(cfile,"unsigned cc2 = %u;\n",cc2);
	fprintf(cfile,"unsigned cc3 = %u;\n",cc3);
	fprintf(cfile,"unsigned cc4 = %u;\n",cc4);
	if (rvused)
		outregvar();
}

outregvar() {
	register i,j;

	fprintf(hfile,"#define REGVARS\n");
	fprintf(cfile,"#include \"regvar.h\"\n");
	fprintf(cfile,"int nregvar[4] = { ");
	for (i=0;i<4;i++) fprintf(cfile,"%d, ",nregvar[i]);
	fprintf(cfile,"};\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(cfile,"struct regassigned ratar%d[%d];\n",
					i,nregvar[i]);
	for (i=0;i<4;i++) if (nregvar[i]>0) {
		fprintf(cfile,"int rvtar%d[] = {",i);
		for (j=0;j<nregvar[i];j++)
			fprintf(cfile,"%d,",rvnumbers[i][j]);
		fprintf(cfile,"};\n");
	}
	fprintf(cfile,"\nint *rvnumbers[] = {\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(cfile,"\trvtar%d,\n",i);
		else
			fprintf(cfile,"\t0,\n");
	fprintf(cfile,"};\n\nstruct regassigned *regassigned[] = {\n");
	for (i=0;i<4;i++)
		if (nregvar[i]>0)
			fprintf(cfile,"\tratar%d,\n",i);
		else
			fprintf(cfile,"\t0,\n");
	fprintf(cfile,"};\n");
}

verbose() {

	fprintf(stderr,"Codebytes %d\n",codebytes);
	fprintf(stderr,"Registers %d(%d)\n",nmachregs,MAXREGS);
	fprintf(stderr,"Properties %d(%d)\n",nprops,MAXPROPS);
	fprintf(stderr,"Tokens %d(%d)\n",nmachtokens,MAXTOKENS);
	fprintf(stderr,"Sets %d(%d)\n",nmachsets,MAXSETS);
	fprintf(stderr,"Tokeninstances %d(%d)\n",narinstance,MAXINSTANCE);
	fprintf(stderr,"Strings %d(%d)\n",ncodestrings,MAXSTRINGS);
	fprintf(stderr,"Enodes %d(%d)\n",lastnode-nodes,MAXNODES);
	fprintf(stderr,"Patbytes %d(%d)\n",npatbytes,MAXPATTERN);
}

inbetween() {
	register ident_p ip;
	register i,j;
	register move_p mp;

	lookident=1;    /* for lexical analysis */

	chktabsiz(nmachsets+1,MAXSETS,"Expressiontable");
	for (i=0;i<SETSIZE;i++)
		machsets[nmachsets].set_val[i] = 0xFFFF;
	machsets[nmachsets].set_val[0] &= ~1;
	machsets[nmachsets].set_size = 0;
	ip=ilookup("SCRATCH",ENTER);
	ip->i_type=IEXP;
	ip->i_i.i_expno = nmachsets++;

	for (i=0;i<SETSIZE;i++)
		machsets[nmachsets].set_val[i] = 0xFFFF;
	machsets[nmachsets].set_size = 0;
	ip=ilookup("ALL",ENTER);
	ip->i_type=IEXP;
	allexpno = ip->i_i.i_expno = nmachsets++;
	mp = &machmoves[nmoves++];
	mp->m_set1 = cocosetno;
	mp->m_expr1 = 0;
	mp->m_set2 = nmachsets-1;
	mp->m_expr2 = 0;
	mp->m_cindex = 0;
	mp->m_cost.c_size = 0;
	mp->m_cost.c_time = 0;

	/*
	 * Create sets of registers per property
	 */

	for (i=0;i<nprops;i++) {
		short *sp = machprops[i].propset.set_val;

		sp[0] |= 1;
		for (j=2;j<=nmachregs;j++)
			if (machregs[j-1]->rprop[i>>4]&(1<<(i&017)))
				sp[j>>4] |= (1<<(j&017));
	}
}

formconversion(p,tp) register char *p; register token_p tp; {
	char buf[256];
	register char *q=buf;
	char field[256];
	register char *f;
	int i;

	if (p==0)
		return(0);
	while (*p) switch(*p) {
	default: *q++ = *p++; continue;
	case '%':
		p++;
		if(*p == '%') {
			*q++ = *p++;
			continue;
		}
		if (*p == '[')
			p++;
		else
			yyerror("Bad character after % in format");
		f=field;
		while (*p != 0 && *p != ']')
			*f++ = *p++;
		*f++ = 0;
		if (*p == ']')
			p++;
		else
			yyerror("Unterminated %[] construct in format");
		for (i=0;i<TOKENSIZE-1;i++)
			if (strcmp(field,tp->t_fields[i].t_sname)==0)
				break;
		if (i==TOKENSIZE-1)
			yyerror("Unknown field in %[] construct in format");
		*q++ = i+1;
	}
	*q++ = 0;
	return(strlookup(buf));
}

setfields(tp,format) register token_p tp; string format; {
	register i;
	list2 ll;
	register list1 l;
	int type;

	for(i=0;i<TOKENSIZE-1;i++)
		tp->t_fields[i].t_type = 0;
	i=0;
	for(ll=tp->t_struct;ll!=0;ll=ll->l2next) {
		l=ll->l2list;
		if(strcmp(l->l1name,"REGISTER")==0)
			type = TYPREG;
		else if (strcmp(l->l1name,"INT")==0)
			type = TYPINT;
		else    type = TYPSTR;
		for(l=l->l1next;l!=0;l=l->l1next) {
			tp->t_fields[i].t_type = type;
			tp->t_fields[i].t_sname = l->l1name;
			i++;
		}
	}
	if (format != 0)
		tp->t_format = formconversion(format,tp);
	else
		tp->t_format = -1;
}

chkregexp(number) {
	register i;

	for(i=nmachregs+1;i<nmachregs+1+nmachtokens;i++)
		if(machsets[number].set_val[i>>4]&(01<<(i&017)))
			yyerror("No tokens allowed in this set");
}

findstructel(number,name,t) string name; int *t; {
	register i;
	register token_p tp;
	register list2 structdecl;
	int offset;

	for(i=1;i<=nmachregs;i++)
		if (machsets[number].set_val[i>>4]&(01<<(i&017)))
			yyerror("No registers allowed in this set");
	structdecl = 0;
	for (i=nmachregs+1;i<nmachregs+1+nmachtokens;i++) {
		if (machsets[number].set_val[i>>4]&(01<<(i&017))) {
			if (structdecl == 0) {
				structdecl = machtokens[i-(nmachregs+1)].t_struct;
				tp = &machtokens[i-(nmachregs+1)];
			} else if(structdecl != machtokens[i-(nmachregs+1)].t_struct)
					yyerror("Multiple structs in this set");
		}
	}
	if (structdecl == 0) {
		yyerror("No structs in this set");
		return(0);
	}
	for(offset=0;offset<TOKENSIZE-1;offset++)
		if(tp->t_fields[offset].t_type != 0 &&
		   strcmp(tp->t_fields[offset].t_sname,name)==0) {
			*t = tp->t_fields[offset].t_type;
			return(offset+1);
		}
	yyerror("No such field in this struct");
	return(0);
}

extern char em_flag[];

argtyp(mn) {

	switch(em_flag[mn-sp_fmnem]&EM_PAR) {
	case PAR_W:
	case PAR_S:
	case PAR_Z:
	case PAR_O:
	case PAR_N:
	case PAR_L:
	case PAR_F:
	case PAR_R:
	case PAR_C:
		return(TYPINT);
	default:
		return(TYPSTR);
	}
}

commontype(e1,e2) expr_t e1,e2; {

	if(e1.expr_typ != e2.expr_typ)
		yyerror("Type incompatibility");
	return(e1.expr_typ);
}

extern char em_mnem[][4];

#define HASHSIZE        (2*(sp_lmnem-sp_fmnem))

struct hashmnem {
	char h_name[3];
	byte h_value;
} hashmnem[HASHSIZE];

inithash() {
	register i;

	for(i=0;i<=sp_lmnem-sp_fmnem;i++)
		enter(em_mnem[i],i+sp_fmnem);
}

enter(name,value) char *name; {
	register unsigned h;

	h=hash(name)%HASHSIZE;
	while (hashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	strncpy(hashmnem[h].h_name,name,3);
	hashmnem[h].h_value = value;
}

int mlookup(name) char *name; {
	register unsigned h;

	h = hash(name)%HASHSIZE;
	while (strncmp(hashmnem[h].h_name,name,3) != 0 &&
	       hashmnem[h].h_name[0] != 0)
		h = (h+1)%HASHSIZE;
	return(hashmnem[h].h_value&BMASK);      /* 0 if not found */
}

hashpatterns() {
	short index;
	register byte *bp,*tp;
	register short i;
	unsigned short hashvalue;
	int patlen;

	index = prevind;
	while (index != 0) {
		bp = &pattern[index];
		tp = &bp[PO_MATCH];
		i = *tp++&BMASK;
		if (i==BMASK) {
			i = *tp++&BMASK;
			i |= (*tp++&BMASK)<<BSHIFT;
		}
		patlen = i;
		hashvalue = 0;
		switch(patlen) {
		default:        /* 3 or more */
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 2:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 1:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		}
		assert(hashvalue!= ILLHASH);
		i=index;
		index = (bp[PO_NEXT]&BMASK)|(bp[PO_NEXT+1]<<BSHIFT);
		bp[PO_HASH] = hashvalue>>BSHIFT;
		hashvalue &= BMASK;
		bp[PO_NEXT] = pathash[hashvalue]&BMASK;
		bp[PO_NEXT+1] = pathash[hashvalue]>>BSHIFT;
		pathash[hashvalue] = i;
	}
}

debug() {
	register i,j;

	for(i=0;i<ITABSIZE;i++) {
		register ident_p ip;
		for(ip=identtab[i];ip!=0;ip=ip->i_next)
			printf("%-14s %1d %3d\n",ip->i_name,
				ip->i_type,ip->i_i.i_regno);
	}

	for(i=2;i<nmachregs;i++) {
		register reginfo rp;

		rp=machregs[i];
		printf("%s = (\"%s\", %d",rp->rname,rp->rrepr,rp->rsize);
		for(j=0;j<MAXMEMBERS;j++)
			if(rp->rmembers[j] != 0)
				printf(", %s",machregs[rp->rmembers[j]]->rname);
		printf(")");
		for(j=0;j<nprops;j++)
			if(rp->rprop[j>>4]&(1<<(j&017)))
				printf(", %s",machprops[j].propname->i_name);
		printf(".\n");
	}
}

out(n) {

	assert(n>=0);
	if (n<128)
		outbyte(n);
	else {
		outbyte(n/256+128);
		outbyte(n%256);
	}
}

outbyte(n) {

	fprintf(cfile,"%d, ",n&BMASK);
	codebytes++;
}

pat(n) {

	assert(n>=0);
	if (n<128)
		patbyte(n);
	else {
		patbyte(n/256+128);
		patbyte(n%256);
	}
}

patshort(n) {

	patbyte(n&BMASK);
	patbyte(n>>BSHIFT);
}

patbyte(n) {

	chktabsiz(npatbytes,MAXPATTERN,"Pattern table");
	pattern[npatbytes++] = n;
}

max(a,b) {

	if (a>b)
		return(a);
	return(b);
}

#include "bootlex.c"
