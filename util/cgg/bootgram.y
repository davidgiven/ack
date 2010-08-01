%{

#ifndef NORCSID
static char rcsid[]="$Id$";
#endif

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <em_spec.h>
#include <em_flag.h>
#include <em_reg.h>

#define extern
#include "booth.h"
#undef extern

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

%type <yy_list1> list1 structlistel
%type <yy_list2> structlist structdecl
%type <yy_expr> expr optexpr
%type <yy_cost> optcost cost optcommacost
%type <yy_int> optboolexpr optnocoerc mnem emargno tokargno optprop
%type <yy_int> optcommabool optstack subreg tokenexpressionno optregvar
%type <yy_int> tokeninstanceno code stackreplacement optslashnumber
%type <yy_set> tokenexpression
%type <yy_instance> tokeninstance
%type <yy_string> optformat
%token <yy_string> IDENT TYPENAME
%token <yy_ident> RIDENT PIDENT TIDENT EIDENT
%token <yy_string> LSTRING STRING
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
%left CMPEQ CMPNE
%left CMPLT CMPLE CMPGT CMPGE
%left RSHIFT LSHIFT
%left '+' '-'
%left '*' '/' '%'
%nonassoc NOT COMP UMINUS
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
		{ $<yy_int>$ = codebytes; cchandled=ccspoiled=0; }
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
