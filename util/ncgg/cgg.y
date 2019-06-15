/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
%{
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "param.h"
#include "varinfo.h"
#include "lookup.h"
#include "set.h"
#include "iocc.h"
#include "instruct.h"
#include "expr.h"
#include "coerc.h"
#include "hall.h"
#include "subr.h"
#include "extern.h"
#include <cgg_cg.h>
#include <em_reg.h>

extern int lineno;
int instline,saveline;
int startline;
int npatterns;
int att_type;
int patindex[MAXPATTERNS];
extern set_t l_sets[];

int emhere=0;	/* lexical analyzer flag */
int optexact=0;	/* Inside "with exact" rule */
int optstack=0;	/* Inside with <blah> STACK rule */
int saferulefound=0;
int maxrule=0;
int in_em_repl=0;	/* set when in EM replacement part */
struct varinfo *defcost;
int Xstackflag=0; /* set in coercions, moves, and tests. %1 means something
		     different then.
		   */

struct varinfo *gen_inst(),*gen_move(),*gen_test(),*gen_preturn(),*gen_tlab();
struct varinfo *gen_label(), *make_erase();
expr_t make_expr(),ident_expr(),subreg_expr(),tokm_expr(),all_expr();
expr_t perc_ident_expr(),sum_expr(),regvar_expr();

set_t ident_to_set(),setproduct(),setsum(),setdiff();

iocc_t subr_iocc(),tokm_iocc(),ident_iocc(),all_iocc(),percident_iocc(), descr_iocc();

extern int narexpr;
extern expr_t arexp[];

extern void skipupto(int tok,char *str);
extern void outpatterns(void);
extern void dopattern(int stackcoerc, varinfo *kills, varinfo *allocates,
			varinfo *generates, varinfo *yields, varinfo *leaving);
extern int yylex(void);			
extern int argtyp(int mn);
extern void yyerror (char const *s);
			
int niops;
iocc_t iops[20];
%}

%union {
	int yy_int;
	char * yy_str;
	varinfo * yy_varinfo;
	set_t yy_set;
	operand *yy_oplist;
	expr_t yy_expr;
	iocc_t yy_iocc;
}

%token PROPERTIES
%token REGISTERS
%token TOKENS
%token SETS
%token MOVES
%token TESTS
%token STACKINGRULES COERCIONS
%token INSTRUCTIONS
%token TOPELTSIZE FALLTHROUGH LABELDEF
%token PROC CALL EXAMPLE
%token FROM TO
%token TEST MOVE STACK RETURN
%token PATTERNS PAT WITH EXACT KILLS USES REUSING GEN YIELDS LEAVING
%token DEFINED SAMESIGN SFIT UFIT ROM LOWW HIGHW ISROM
%token CMPEQ CMPNE CMPLT CMPGT CMPLE CMPGE OR2 AND2 LSHIFT RSHIFT NOT COMP
%token INREG REGVAR REGVAR_W REGVAR_D
%token REG_ANY REG_FLOAT REG_LOOP REG_POINTER
%token <yy_int> ADORNACCESS
%token <yy_int> ADORNCC
%token INT
%token ADDR
%token <yy_int> EMMNEM
%token <yy_int> NUMBER
%token <yy_int> DOLLAR PERCENT ALLREG
%token <yy_str> IDENT PERC_IDENT
%token <yy_str> STRING
%token TIMEFACTOR SIZEFACTOR
%token COST
%type <yy_varinfo> prop_list property ident_list ident_list_el
%type <yy_varinfo> att_list att_list_el att_list_el_list att_ident structdecl optcost optformat
%type <yy_varinfo> kills allocates yields leaving
%type <yy_varinfo> generates kill_list kill_list_el uselist uselist_el genlist yieldlist
%type <yy_varinfo> leavelist leavelist_el gen_instruction
%type <yy_varinfo> opt_erase_list erase_list erase_list_el
%type <yy_str> opt_par_string optstring
%type <yy_int> register propno att_list_el_type tokenset_no
%type <yy_int> adornlist optstar optuses optregvar regvartype optregvartype
%type <yy_int> emarg tokarg subreg allreg
%type <yy_expr> expr regvarexpr
%type <yy_iocc> tokeninstance
%type <yy_int> optexpr optexact optstack
%type <yy_set> tokenset
%type <yy_oplist> oplist oplist_el

%left OR2
%left AND2
%left '|'
%left '^'
%left '&'
%left CMPEQ CMPNE
%left CMPLT CMPLE CMPGT CMPGE
%left RSHIFT LSHIFT
%left '+' '-'
%left '*' '/' '%'
%nonassoc NOT COMP UMINUS

%start machtable

%%
/*
 * The machine table consists of a number of sections, with their
 * own associated parsers.
 */
machtable
	: constants
	  properties
	  registers
		{ check_reglap(); }
	  tokens
		{ make_std_sets(); }
	  sets
	  instructions
	  moves
	  tests
	  stacks
	  coercs
	  code
	;

/*
 * Constants are parsed as name=value pairs
 */
constants
	: constdeflist
	;
constdeflist
	: /* empty */
	| constdeflist constdef
	;
constdef
	: IDENT'=' NUMBER
		{ n_const($1,$3); free($1); }
	| IDENT '=' STRING
		{ n_sconst($1,$3); free($1); free($3); }
	| TIMEFACTOR '=' NUMBER '/' NUMBER
		{ fc1 = $3; fc2 = $5; }
	| SIZEFACTOR '=' NUMBER '/' NUMBER
		{ fc3 = $3; fc4 = $5; }
	| error
	;

/*
 * Properties are parsed as a list of names optionally followed by their size
 */
properties
	: PROPERTIES  { make_const(); } prdef_list
	;
prdef_list
	: prdef_list_el
	| prdef_list optcomma prdef_list_el
	;
prdef_list_el
	: IDENT
		{ n_prop($1,wordsize); free($1); }
	| IDENT '(' NUMBER ')'
		{ n_prop($1,$3); free($1); }
	;

/*
 * Registers are rows of reglist:proplist pairs
 */
registers
	: REGISTERS regdef_list
	;
regdef_list
	: regdef_list_el
	| regdef_list regdef_list_el
	;
regdef_list_el
	: ident_list ':' prop_list optregvar '.'
		{ regline($1,$3,$4); free((char *) $1); free((char *) $3); }
	| error '.'
	;
optregvar
	: /* empty */
		{ $$ = -1; }
	| REGVAR
		{ $$ = reg_any; }
	| REGVAR '(' regvartype ')'
		{ $$ = $3; }
	;

regvartype
	: REG_ANY
		{ $$ = reg_any;}
	| REG_FLOAT
		{ $$ = reg_float;}
	| REG_LOOP
		{ $$ = reg_loop;}
	| REG_POINTER
		{ $$ = reg_pointer;}
	;

ident_list
	: ident_list_el
	| ident_list optcomma ident_list_el
		{ $3->vi_next = $1; $$ = $3; }
	;
ident_list_el
	: IDENT opt_par_string
		{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = n_reg($1,$2,0,0,0);
		  free($1); if($2!=0) free($2);
		}
	| IDENT opt_par_string '=' register
		{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = n_reg($1,$2,1,$4,0);
		  free($1); if($2!=0) free($2);
		}
	| IDENT opt_par_string '=' register '+' register
		{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = n_reg($1,$2,2,$4,$6);
		  free($1); if($2!=0) free($2);
		}
	;
opt_par_string
	: /* empty */
		{ $$ = 0; }
	| '(' STRING ')'
		{ $$ = $2; }
	;
register
	: IDENT
		{ register symbol *sy_p;

		  sy_p = lookup($1,symreg,mustexist);
		  $$ = sy_p->sy_value.syv_regno;
		  free($1);
		}
	;
prop_list
	: property
	| prop_list optcomma property
		{ $3->vi_next = $1; $$ = $3; }
	;
property
	: IDENT
		{ register symbol *sy_p;
		  sy_p = lookup($1,symprop,mustexist);
		  NEW($$,struct varinfo);
		  $$->vi_next=0;
		  $$->vi_int[0]=sy_p->sy_value.syv_propno;
		  free($1);
		}
	;
propno
	: IDENT
		{ register symbol *sy_p;
		  sy_p = lookup($1,symprop,mustexist);
		  $$ = sy_p->sy_value.syv_propno;
		  free($1);
		}
	;

/* tokens are parsed as struct definitions
 * types in the struct can be register properties, ADDR or INT
 */

tokens
	: TOKENS tokdeflist
	;
tokdeflist
	: tokdeflist_el
	| tokdeflist tokdeflist_el
	;
tokdeflist_el
	: IDENT '=' structdecl NUMBER optcost optformat '.'
		{ n_tok($1,$3,$4,$5,$6);
		  free($1);
		  freevi($3);
		  freevi($5);
		  freevi($6);
		}
	| error '.'
	;
structdecl
	: '{' att_list '}'
		{ $$ = $2; }
	;
att_list
	: /* empty */
		{ $$ = 0; }
	| att_list_el att_list
		{ $$ = $1; while ($$->vi_next) $$ = $$->vi_next;
		  $$->vi_next = $2; $$ = $1;
		}
	;
att_list_el
	: att_list_el_type
		{ att_type = $1; }
	  att_list_el_list
	  ';'
		{ $$ = $3; }
	;
att_list_el_list
	: att_ident
	| att_ident ',' att_list_el_list
		{ $1->vi_next = $3; $$ = $1; }
	;
att_ident
	: IDENT
		{ NEW($$, struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = att_type;
		  $$->vi_str[0] = $1;
		}
	;
att_list_el_type
	: INT
		{ $$ = -1; }
	| ADDR
		{ $$ = -2; }
	| propno
	;
optcost
	:
		{ if (defcost==VI_NULL)
		  	$$=VI_NULL;
		  else {
			NEW($$,struct varinfo);
			*$$ = *defcost;
		  }
	 	}
	| COST '(' NUMBER ',' NUMBER ')'
		{ NEW ($$,struct varinfo);
		  $$->vi_int[0] = $3;
		  $$->vi_int[1] = $5;
		}
	;
optformat
	:
		{ $$ = 0; }
	| STRING optformat
		{ NEW($$,struct varinfo);
		  $$->vi_next = $2;
		  $$->vi_int[0] = 0;
		  $$->vi_str[0] = $1;
		}
	| IDENT optformat
		{ NEW($$,struct varinfo);
		  $$->vi_next = $2;
		  $$->vi_int[0] = 1;
		  $$->vi_str[0] = $1;
		}
	;
optcomma
	: ','
	| /* empty */
	;

/* sets are parsed as ident = expression */

sets
	: SETS setdeflist
	;
setdeflist
	: setdeflist_el
	| setdeflist setdeflist_el
	;
setdeflist_el
	: IDENT '=' tokenset_no '.'
		{ n_set($1,$3); free($1); }
	| error '.'
	;
tokenset_no
	: tokenset
		{ $$ = setlookup($1); }
	;
tokenset
	: IDENT
		{ $$ = ident_to_set($1); free($1); }
	| tokenset '*' tokenset
		{ $$ = setproduct($1,$3); }
	| tokenset '+' tokenset
		{ $$ = setsum($1,$3); }
	| tokenset '-' tokenset
		{ $$ = setdiff($1,$3); }
	| '(' tokenset ')'
		{ $$ = $2; }
	;

instructions
	: INSTRUCTIONS optcost instdef_list
		{ defcost = $2; }
	;
instdef_list
	: instdef_list_el
	| instdef_list instdef_list_el
	;
instdef_list_el
	: IDENT optstring oplist opt_erase_list optcost '.'
		{ n_instr($1,$2,$3,$4,$5); freevi($5); }
	| error '.'
	;
oplist
	: /* empty */
		{ $$ = 0; }
	| oplist_el
	| oplist_el ',' oplist
		{ $$ = $1; $$->o_next = $3; }
	;
oplist_el
	: tokenset_no adornlist
		{ NEW($$,struct operand);
		  $$->o_next  = 0 ;
		  $$->o_setno = $1;
		  $$->o_adorn = $2;
		  checkprintformat($1);
		}
	;
adornlist
	: /* empty */
		{ $$ = 0; }
	| ADORNACCESS adornlist
		{ if ($2&AD_RWMASK)
			error("Only one of :ro,:wo,:rw allowed");
		  $$ = $1 | $2;
		}
	| ADORNCC adornlist
		{ $$ = $1|$2; }
	;
opt_erase_list
	: /* empty */
		{ $$ = VI_NULL;}
	| KILLS erase_list
		{ $$ = $2; }
	;
erase_list
	: erase_list_el
		{ $$ = $1; }
	| erase_list_el erase_list
		{ $1->vi_next = $2; $$ = $1; }
	;
erase_list_el
	: IDENT
		{ $$ = make_erase($1); }
	| ADORNCC
		{ NEW($$, struct varinfo);
		  $$->vi_int[0] = -1;
		  $$->vi_next = VI_NULL;
		}
	;

/* Now the moves */

moves
	: MOVES
	  movedeflist
	| /* empty */
	;
movedeflist
	: movedeflist_el
	| movedeflist movedeflist_el
	;
movedeflist_el
	: FROM
		{startline = lineno; }
	  tokenset_no
		{ Xstackflag = 1;
		  cursetno = $3;
		}
	  optexpr
		{ Xstackflag = 0;
		  cursetno = -1;
		}
	  TO tokenset_no
	  	{ cursetno = $8;
		  tokpatlen=2;
		  tokpatset[0] = $3;
		  tokpatset[1] = $8;
		  tokpatro[0] = 1;
		  Xstackflag = 1;
		}
	optexpr
		{ Xstackflag = 0;
		  cursetno = -1;
		}
	GEN genlist
		{ tokpatlen=0;
		  tokpatro[0]=0;
		  n_move($3,$5,$8,$10,$13);
		  freevi($13);
		}
	| error
	;

/* Now the test part */

tests
	: TESTS
		{ Xstackflag = 1; }
	  testdeflist
		{ Xstackflag = 0; }
	| /* empty */
	;
testdeflist
	: testdeflist_el
	| testdeflist testdeflist_el
	;
testdeflist_el
	: TO
		{ startline = lineno;}
 	  TEST tokenset_no
		{ cursetno = $4;
		  tokpatlen=1;
		  tokpatset[0]=$4;
		  tokpatro[0] = 1;
		}
	  optexpr GEN genlist
	  	{ tokpatlen=0;
		  tokpatro[0] = 0;
		  n_test($4,$6,$8);
		  freevi($8);
		  cursetno = -1;
		}
	| error
	;
	  
/* Now the stacks */

stacks
	: STACKINGRULES
		{ Xstackflag = 1; }
	  stackdeflist
		{ Xstackflag = 0; }
	;
stackdeflist
	: stackdeflist_el
	| stackdeflist stackdeflist_el
	;
stackdeflist_el
	: FROM
		{startline = lineno;}
	  tokenset_no
		{ cursetno = $3; 
		  tokpatlen=1; 
		  tokpatset[0] = $3; 
		  tokpatro[0] = 1;
		}
	  optexpr TO STACK optuses GEN genlist
		{ tokpatro[0] = 0;
		  n_stack($3,$5,$8,$10);
		  freevi($10);
		  cursetno = -1;
		}
	;
optuses
	: /* empty */
		{ $$ = -1; nallreg=0;}
	| USES propno
		{ $$ = $2; nallreg = 1; allreg[0] = $2; }
	;

/* Now the one-to-one coercion rules */

coercs
	: COERCIONS
		{ Xstackflag = 1; }
	  coercdeflist
		{ Xstackflag = 0; }
	;
coercdeflist
	: coercdeflist_el
	| coercdeflist coercdeflist_el
	;
coercdeflist_el
	: FROM
		{startline = lineno; tokpatlen=0; inithall();}
	  STACK allocates generates YIELDS tokeninstance
		{ checkhall();
		  n_coerc(0,0,$4,$5,(struct varinfo *) 0,$7);
		  freevi($4);
		  freevi($5);
		}
	| FROM
		{startline = lineno;}
	  tokenset_no
		{ cursetno = $3; 
		  tokpatlen=1; 
		  tokpatset[0]=$3; 
		  tokpatro[0] = 1;
		  inithall();
		}
	  optexpr allocates generates yields
	  	{ tokpatro[0] = 0;
		  checkhall();
		  n_coerc($3,$5,$6,$7,$8);
		  freevi($6);
		  freevi($7);
		  cursetno = -1;
		}
	;

/* Now the code part */

code
	: PATTERNS coderules
	;
coderules
	: coderule
	| coderules coderule
	;
coderule
	: PAT {emhere=1;} empattern {emhere=0;} optexpr
		{ empatexpr = $5;
		  npatterns = 0;
		  saferulefound=0;
		  if (empatlen>maxempatlen)
			maxempatlen=empatlen;
		}
	  patterns
		{ if (!saferulefound)
			error("Previous rule impossible on empty stack");
		  outpatterns();
		}
	| PROC IDENT example
		{ npatterns = 0; saferulefound=0; inproc=1; n_proc($2); }
	  patterns
		{ if (!saferulefound)
			error("Previous rule impossible on empty stack");
		  outpatterns(); inproc=0;
		}
	| error
		{ skipupto(PAT,"pat"); yyerrok; yyclearin; }
	;
example
	: /* empty */
		{ empatlen = 0; }
	| EXAMPLE {emhere=1;} empattern {emhere=0;}
	;
empattern
	: EMMNEM
		{ empatlen = 1; emmnem[0] = $1; }
	| empattern EMMNEM
		{ NEXT(empatlen,EMPATMAX,"Em pattern");
		  emmnem[empatlen-1] = $2;
		}
	;
patterns
	: onepattern
		{ saferulefound=1;
		  callproc=0;
		}
	| morepatterns
		{ callproc=0;
		  if (npatterns>maxrule)
		  	maxrule=npatterns;
		}
	| CALL IDENT '(' stringlist ')'
		{ register symbol *sy_p;
		  saferulefound=1;
		  sy_p=lookup($2,symproc,mustexist);
		  callproc=sy_p->sy_value.syv_procoff;
		  free($2);
		  if (nprocargs > maxprocargs) maxprocargs = nprocargs;
		}
	;

stringlist
	: STRING
		{ nprocargs = 1; procarg[0] = strlookup($1); free($1); }
	| stringlist ',' STRING
		{ NEXT(nprocargs, MAXPROCARG, "Procedure argument list");
		  procarg[nprocargs-1] = strlookup($3); free($3);
		}
	;

onepattern
	: 	{ inithall(); startline=lineno;
		  tokpatlen=0; optexact=0; optstack=0;
		}
	  kills allocates generates yields leaving
		{ 
		  patindex[npatterns++]=codeindex;
		  checkhall();
		  dopattern(0,$2,$3,$4,$5,$6);
		  freevi($2);
		  freevi($3);
		  freevi($4);
		  freevi($5);
		  freevi($6);
		}
	;
morepatterns
	: { inithall(); } pattern
	| morepatterns { inithall(); } pattern
	;

pattern
	: stackpattern kills allocates generates yields leaving
		{ patindex[NEXT(npatterns,MAXPATTERNS,"Patterns")]=codeindex;
		  if (hall() && !optexact) saferulefound=1;
		  dopattern(0,$2,$3,$4,$5,$6);
		  freevi($2);
		  freevi($3);
		  freevi($4);
		  freevi($5);
		  freevi($6);
		}
	;
stackpattern
	: WITH optexact
		{ startline = lineno; }
	  setlist optstack
		{ if (tokpatlen > maxtokpatlen) maxtokpatlen = tokpatlen; }
	;
optexact
	: /* empty */
		{ $$ = optexact = 0; }
	| EXACT
		{ $$ = optexact = 1; }
	;
optstack
	: /* empty */
		{ $$ = optstack = 0; }
	| STACK
		{ $$ = optstack = 1; }
	;

setlist
	: /* empty */
		{ tokpatlen = 0; }
	| setlist tokenset_no
		{ NEXT(tokpatlen,TOKPATMAX,"Stack pattern");
		  tokpatset[tokpatlen-1] = $2;
		  checkunstacking($2);
		  if (l_sets[$2].set_size == 0) {
			error("Tokenset is empty or has elements with different sizes");
		  }
		}
	;
kills
	: /* empty */
		{ $$ = 0; }
	| KILLS kill_list
		{ $$ = $2;
		}
	;
kill_list
	: kill_list_el
	| kill_list_el ',' kill_list
		{ $$=$1; $$->vi_next = $3; }
	;
kill_list_el
	: tokenset_no { cursetno=$1; } optexpr
		{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0]=$1;
		  $$->vi_int[1]=$3;
		  cursetno = -1;
		}
	| regvarexpr
	 	{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = -($1.ex_index + 1);
		  $$->vi_int[1] = 0;
		}
	; 

allocates
	: /* empty */
		{ $$ = 0; nallreg=0;}
	| USES uselist
		{ $$ = $2; setallreg($2); }
	;
uselist
	: uselist_el
		{ prophall($1->vi_int[0]); }
	| uselist_el ',' uselist
		{ prophall($1->vi_int[0]); $$=$1; $$->vi_next=$3; }
	;
uselist_el
	: property
		{ $$=$1; $$->vi_int[1] = 0; }
	| property '=' tokeninstance
		{ if (!existalmove($3,$$->vi_int[0]))
			error("No such move defined");
		  $$=$1; $$->vi_int[1] = $3.in_index; 
		}
	| REUSING tokeninstance
		{ NEW($$,struct varinfo);
		  $$->vi_next = 0;
		  $$->vi_int[0] = -1;
		  $$->vi_int[1] = $2.in_index;
		}
	;


generates
	: /* empty */
		{ $$ = 0; }
	| GEN genlist
		{ $$ = $2; }
	;
genlist
	: /* empty */
		{ $$ = 0; }
	| gen_instruction genlist
		{ if ($1!=0) {
			register struct varinfo *tvip;
			$$=tvip=$1;
			while (tvip->vi_next!=VI_NULL)
				tvip=tvip->vi_next;
		 	tvip->vi_next = $2;
		  } else {
			$$ = $2;
		  }
		}
	;
gen_instruction
	: {instline = lineno; } IDENT optstar gen_oplist
		{ saveline =lineno; lineno=instline;
		  $$ = gen_inst($2,$3); free($2);
		  lineno = saveline;
		}
	| NUMBER ':'
		{ $$ = gen_tlab($1); }
	| MOVE tokeninstance ',' tokeninstance
		{ $$ = gen_move($2,$4); }
	| TEST tokeninstance
		{ $$ = gen_test($2);}
	| LABELDEF emarg
		{ $$ = gen_label($2-1); use_tes++; }
	| RETURN
		{ $$ = gen_preturn(); }
	;
optstar
	: /* empty */
		{ $$=0; }
	| '*'
		{ $$=1; }
	| '[' NUMBER ']'
		{ $$=$2; }
	;
gen_oplist
	: '.' /* empty gives conflicts */
		{ niops=0; }
	| tokeninstance
		{ niops=1;iops[0]=$1; }
	| gen_oplist ',' tokeninstance
		{ iops[niops++] = $3; }
	;

yields
	: /* empty */
		{ $$ = 0; }
	| YIELDS { tokrepllen = 0; } yieldlist
		{ $$ = $3; if (tokrepllen > maxtokrepllen) maxtokrepllen = tokrepllen; }
	;
yieldlist
	: /* empty */
		{ $$ = 0; }
	| tokeninstance yieldlist
		{ checkstacking($1.in_set);
		  NEW($$,struct varinfo);
		  $$->vi_next = $2;
		  $$->vi_int[0] = $1.in_index;
		  tokrepllen++;
		}
	;

leaving
	: /* empty */
		{ $$ = 0; }
	| LEAVING {emhere=1; in_em_repl=1; emrepllen = 0; } leavelist
		{ emhere=0; in_em_repl=0; $$ = $3;
		  if (emrepllen > maxemrepllen) maxemrepllen = emrepllen;
		}
	;
leavelist
	: leavelist_el
	| leavelist_el leavelist
		{ $$=$1; $$->vi_next=$2; }
	;
leavelist_el
	: EMMNEM optexpr
		{ NEW($$,struct varinfo);
		  $$->vi_next=0;
		  $$->vi_int[0] = $1;
		  $$->vi_int[1] = $2;
		  emrepllen++;
		}
	;

optstring
	: /* empty */
		{ $$ = 0; }
	| STRING
	;
optexpr
	: /* empty */
		{ $$ = 0; }
	| expr
		{ $$ = $1.ex_index; }	/* type checking ? */
	;

tokeninstance
	: tokarg subreg
		{ $$ = subr_iocc($1,$2); }
	| tokarg '.' IDENT
		{ $$ = tokm_iocc($1,$3); free($3); }
	| IDENT
		{ $$ = ident_iocc($1); free($1);}
	| allreg subreg
		{ $$ = all_iocc($1,$2); }
	| PERC_IDENT
		{ if (cursetno < 0) {
			error("%%<ident> not allowed here");
		  }
		  $$ = percident_iocc($1);
		  free($1);
		}
	| '{' IDENT attlist '}'
		{ $$ = descr_iocc($2); free($2); }
	;
attlist
	: /* empty */
		{ narexpr = 0; }
	| attlist ',' expr
		{ arexp[narexpr++] = $3; }
	;

emarg
	: DOLLAR
		{ if ($1<1 || $1>empatlen)
			error("Only %d instructions in pattern",empatlen);
		  $$ = $1;
		}
	;
tokarg
	: PERCENT
		{ $$ = $1;
		  if (in_em_repl)
			error("No token references allowed in EM replacement part");
		  if ($1<1 || $1>tokpatlen) {
			error("Only %d tokens in stackpattern",tokpatlen);
			$$ =1;
		  }
		  if (Xstackflag) $$ = 0;
		}
	;
subreg
	: /* empty */
		{ $$ = 0; }
	| '.' NUMBER
		{ if ($2<1 || $2>2) {
			error("Only 2 subregisters allowed");
			$$ = 1;
		  } else {
			$$ = $2;
		  }
		}
	;
allreg
	: ALLREG
		{ if ($1>=nallreg)
			fatal("Only %d registers allocated",nallreg);
		  $$ = $1;
		}
	;

expr
	: NUMBER
		{ $$ = make_expr(TYPINT,EX_CON, (int) ($1 & 0xFFFF), (int) ($1>>16));
		}
	| emarg
		{ $$ = make_expr(argtyp(emmnem[$1-1]),EX_ARG,$1,0); }
	| STRING
		{ $$ = make_expr(TYPADDR,EX_STRING,strlookup($1),0); free($1); }
	| IDENT
		{ $$ = ident_expr($1); free($1); }
	| tokarg subreg
		{ $$ = subreg_expr($1,$2); }
	| tokarg '.' IDENT
		{ $$ = tokm_expr($1,$3); free($3); }
	| allreg subreg
		{ $$ = all_expr($1,$2); }
	| PERC_IDENT
		{ if (cursetno < 0) {
			error("%%<ident> not allowed here");
		  }
		  $$ = perc_ident_expr($1);
		  free($1);
		}
	| DEFINED '(' expr ')'
		{ $$ = make_expr(TYPBOOL,EX_DEFINED,i_expr($3),0); }
	| SAMESIGN '(' expr ',' expr ')'
		{ $$ = make_expr(TYPBOOL,EX_SAMESIGN,i_expr($3),i_expr($5)); }
	| TOPELTSIZE '(' emarg ')'
		{ $$ = make_expr(TYPINT,EX_TOPELTSIZE,$3-1,0); }
	| FALLTHROUGH '(' emarg ')'
		{ $$ = make_expr(TYPBOOL,EX_FALLTHROUGH,$3-1,0); }
	| SFIT '(' expr ',' expr ')'
		{ $$ = make_expr(TYPBOOL,EX_SFIT,i_expr($3),i_expr($5)); }
	| UFIT '(' expr ',' expr ')'
		{ $$ = make_expr(TYPBOOL,EX_UFIT,i_expr($3),i_expr($5)); }
	| ROM '(' emarg ',' NUMBER ')'
		{ $$ = make_expr(TYPINT,EX_ROM,$3-1,chkincl($5,1,3)-1); }
	| ISROM '(' emarg ')'
		{ $$ = make_expr(TYPBOOL,EX_ISROM,$3-1,0); }
	| LOWW '(' emarg ')'
		{ $$ = make_expr(TYPINT,EX_LOWW,$3-1,0); }
	| HIGHW '(' emarg ')'
		{ $$ = make_expr(TYPINT,EX_HIGHW,$3-1,0); }
/* Excluded, because it causes a shift-reduce conflict
   (problems with a tokenset_no followed by an optexpr)
	| '-' expr %prec UMINUS
		{ $$ = make_expr(TYPINT,EX_CON, 0, 0);
		  $$ = make_expr(TYPINT,EX_MINUS,i_expr($$),i_expr($2)); 
		}
*/
	| '(' expr ')'
		{ $$ = $2; }
	| expr CMPEQ expr
		{ $$ = make_expr(TYPBOOL,eq2expr($1,$3),$1.ex_index,$3.ex_index); }
	| expr CMPNE expr
		{ $$ = make_expr(TYPBOOL,ne2expr($1,$3),$1.ex_index,$3.ex_index); }
	| expr CMPLT expr
		{ $$ = make_expr(TYPBOOL,EX_NCPLT,i_expr($1),i_expr($3)); }
	| expr CMPGT expr
		{ $$ = make_expr(TYPBOOL,EX_NCPGT,i_expr($1),i_expr($3)); }
	| expr CMPLE expr
		{ $$ = make_expr(TYPBOOL,EX_NCPLE,i_expr($1),i_expr($3)); }
	| expr CMPGE expr
		{ $$ = make_expr(TYPBOOL,EX_NCPGE,i_expr($1),i_expr($3)); }
	| expr OR2 expr
		{ $$ = make_expr(TYPBOOL,EX_OR2,b_expr($1),b_expr($3)); }
	| expr AND2 expr
		{ $$ = make_expr(TYPBOOL,EX_AND2,b_expr($1),b_expr($3)); }
	| expr '|' expr
		{ $$ = make_expr(TYPINT,EX_OR,i_expr($1),i_expr($3)); }
	| expr '^' expr
		{ $$ = make_expr(TYPINT,EX_XOR,i_expr($1),i_expr($3)); }
	| expr '&' expr
		{ $$ = make_expr(TYPINT,EX_AND,i_expr($1),i_expr($3)); }
	| expr '+' expr
		{ $$ = sum_expr($1,$3); }
	| expr '-' expr
		{ $$ = make_expr(TYPINT,EX_MINUS,i_expr($1),i_expr($3)); }
	| expr '*' expr
		{ $$ = make_expr(TYPINT,EX_TIMES,i_expr($1),i_expr($3)); }
	| expr '/' expr
		{ $$ = make_expr(TYPINT,EX_DIVIDE,i_expr($1),i_expr($3)); }
	| expr '%' expr
		{ $$ = make_expr(TYPINT,EX_MOD,i_expr($1),i_expr($3)); }
	| expr LSHIFT expr
		{ $$ = make_expr(TYPINT,EX_LSHIFT,i_expr($1),i_expr($3)); }
	| expr RSHIFT expr
		{ $$ = make_expr(TYPINT,EX_RSHIFT,i_expr($1),i_expr($3)); }
	| NOT expr
		{ $$ = make_expr(TYPBOOL,EX_NOT,b_expr($2),0); }
	| COMP expr
		{ $$ = make_expr(TYPINT,EX_COMP,i_expr($2),0); }
	| INREG '(' expr ')'
		{ $$ = make_expr(TYPINT,EX_INREG,i_expr($3),0); }
	| regvartype
		{ $$ = make_expr(TYPINT,EX_CON, $1+1, 0); }
	| regvarexpr
	;

regvarexpr
	: REGVAR '(' expr optregvartype ')'
		{ $$ = regvar_expr($3,$4,-1); }
	| REGVAR_W '(' expr optregvartype ')'
		{ $$ = regvar_expr($3,$4,wordsize); }
	| REGVAR_D '(' expr optregvartype ')'
		{ $$ = regvar_expr($3,$4,2*wordsize); }
	;

optregvartype
	: /* empty */
		{ $$ = reg_any; }
	| ',' regvartype
		{ $$ = $2; }
	;
%%
