/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm2.y	1.7 */

/*
 * delay inclusion of machine dependent stuff (see comm0.h)
 */
#define	_include	#include

%{
#include	"comm0.h"
#include	"comm1.h"

static item_t	*last_it, *o_it;
%}

/* ========== Machine independent Yacc definitions ========== */

%union {
	word_t	y_word;
	valu_t	y_valu;
	expr_t	y_expr;
	item_t	*y_item;
#ifdef ASLD
	char	*y_strp;
#endif
};

#ifdef ASLD
%token <y_strp> MODULE
#endif
%token STRING
%token <y_item> IDENT
%token <y_item> FBSYM
%token <y_valu> CODE1
%token <y_valu> CODE2
%token <y_valu> CODE4
%token NUMBER0		/* keep NUMBER* in this order */
%token NUMBER1
%token NUMBER2
%token NUMBER3
%token <y_valu> NUMBER
%token DOT
%token EXTERN
%token <y_word> DATA
%token <y_word> ASCII
%token SECTION
%token COMMON
%token BASE
%token SYMB
%token SYMD
%token ALIGN
%token ASSERT
%token SPACE
%token <y_word> LINE
%token FILe
%token <y_word> LIST
%token OP_EQ
%token OP_NE
%token OP_LE
%token OP_GE
%token OP_LL
%token OP_RR
%token OP_OO
%token OP_AA

%left OP_OO
%left OP_AA
%left '|'
%left '^'
%left '&'
%left OP_EQ OP_NE
%left '<' '>' OP_LE OP_GE
%left OP_LL OP_RR
%left '+' '-'
%left '*' '/' '%' 
%nonassoc '~'

%type <y_valu> absexp optabs1 optabs2
%type <y_expr> expr
%type <y_item> id_fb

/* ========== Machine dependent Yacc definitions ========== */

#include	"mach2.c"

%%

/* ========== Machine independent rules ========== */

#ifdef LISTING
#define	LISTLINE(n)	if (listflag) listline(n); \
			else if (listtemp) { listflag = listtemp; listeoln = 1; }
#else
#define	LISTLINE(n)	/* empty */
#endif /* LISTING */

#ifdef ASLD
#define	RELODONE	/* empty */
#else
#define	RELODONE	assert(relonami == 0)
#endif

program	:	/* empty */
#ifdef ASLD
	|	program MODULE /* not in PASS_1 */
			{	newmodule($2);}
#endif
	|	program IDENT ':'
			{	newident($2, DOTTYP); newlabel($2);}
	|	program NUMBER ':'
			{	if ($2 < 0 || $2 > 9) {
					serror("bad f/b label");
					$2 = 0;
				}
				newlabel(fb_shift((int)$2));
			}
	|	program CODE1
			{	emit1((int)$2); LISTLINE(0);}
	|	program CODE2
			{	emit2((int)$2); LISTLINE(0);}
	|	program CODE4
			{	emit4((long)$2); LISTLINE(0);}
	|	program operation ';'
	|	program operation '\n'
			{	lineno++; LISTLINE(1); RELODONE;}
	|	program '#' NUMBER STRING '\n'
			{	lineno = $3;
				if (modulename) strncpy(modulename, stringbuf, STRINGMAX-1);
				LISTLINE(1); RELODONE;
			}
	|	program error '\n'
			{	serror("syntax error"); yyerrok;
				lineno++; LISTLINE(1); RELODONE;
			}
	;
#undef LISTLINE
#undef RELODONE
operation
	:	/* empty */
	|	IDENT '=' expr
			{
#ifdef LISTING
				if (listflag & 1)
					listcolm += printx(VALWIDTH, $3.val);
#endif
				newequate($1, $3.typ);
				store($1, $3.val);
			}
#ifdef LISTING
	|	LIST
			{	if ($1)
					listtemp = listmode;
				else if ((dflag & 01000) == 0)
					listtemp = 0;
			}
#endif
	| 	SECTION IDENT
			{	newsect($2);}
	|	COMMON IDENT ',' absexp
			{	newcomm($2, $4);}
	|	BASE absexp
			{	if (pass == PASS_1) newbase($2);}
	|	ASSERT expr
			{	if ($2.val == 0 && pass == PASS_3)
					warning("assertion failed");
			}
	|	SYMB STRING ',' expr	{ o_it = last_it; }
		optabs2 optabs2
			{	if ((sflag & SYM_SMB) && PASS_SYMB) {
#ifndef ASLD
					if (
						pass == PASS_3
						&&
						($4.typ & S_TYP) == S_UND
					   ) {
						serror("expression undefined");
						relonami = -1;
					}
					if (
						PASS_SYMB
						&&
						($4.typ & S_COM)
					   ) {
						/* No value is known at
						   assembler time.
						   Generate reference to other
						   entry in name table
						*/
						$4.typ = S_CRS;
						$4.val = new_string(o_it->i_name);
						relonami = 0;
					}
#endif
					    
					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							($4.typ & (S_EXT|S_TYP))
							|
							((unsigned short)$6<<8)
						),
						(short)$7,
						$4.val
					);
				}
			}
	|	SYMD STRING ','  absexp ',' absexp
			{	if ((sflag & SYM_SMB) && PASS_SYMB) {
					newsymb(
						*stringbuf ? stringbuf : (char *) 0,
						(short)(
							(DOTTYP & (S_EXT|S_TYP))
							|
							((unsigned short)$4<<8)
						),
						(short)$6,
						(valu_t)DOTVAL
					);
				}
			}
	|	LINE optabs1
			{	if ((sflag & SYM_LIN) && PASS_SYMB) {
					if ($2)
						hllino = (short)$2;
					else
						hllino++;
					newsymb(
						(char *)0,
						(DOTTYP | S_LIN),
						hllino,
						(valu_t)DOTVAL
					);
				}
			}
	|	FILe STRING
			{	if ((sflag & SYM_LIN) && PASS_SYMB) {
					hllino = 0;
					newsymb(
						stringbuf,
						(DOTTYP | S_FIL),
						0,
						(valu_t)DOTVAL
					);
				}
			}
	|	EXTERN externlist
	|	ALIGN optabs1
			{	align($2);}
	|	SPACE absexp
			{	if (DOTSCT == NULL)
					nosect();
				DOTVAL += $2;
				DOTSCT->s_zero += $2;
			}
	|	DATA datalist
	|	ASCII STRING
			{	emitstr($1);}
	;
externlist
	:	IDENT
			{	$1->i_type |= S_EXT;}
	|	externlist ',' IDENT
			{	$3->i_type |= S_EXT;}
	;
datalist
	:	expr
			{
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($1.typ, (int)$<y_word>0|MACHREL_BWR);
#endif
				emitx($1.val, (int)$<y_word>0);
			}
	|	datalist ',' expr
			{
#ifdef RELOCATION
				if (rflag != 0 && PASS_RELO)
					newrelo($3.typ, (int)$<y_word>0|MACHREL_BWR);
#endif
				emitx($3.val, (int)$<y_word>0);
			}
	;
expr	:	error
			{	serror("expr syntax err");
				$$.val = 0; $$.typ = S_UND;
			}
	|	NUMBER
			{	$$.val = $1; $$.typ = S_ABS;}
	|	id_fb
			{	$$.val = load($1); 
				last_it = $1;
				$$.typ = $1->i_type & ~S_EXT;
			}
	|	STRING
			{	if (stringlen != 1)
					serror("too many chars");
				$$.val = stringbuf[0];
				$$.typ = S_ABS;
			}
	|	ASC_LPAR expr ASC_RPAR
			{	$$ = $2;}
	|	expr OP_OO expr
			{	$$.val = ($1.val || $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr OP_AA expr
			{	$$.val = ($1.val && $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '|' expr
			{	$$.val = ($1.val | $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '^' expr
			{	$$.val = ($1.val ^ $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '&' expr
			{	$$.val = ($1.val & $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr OP_EQ expr
			{	$$.val = ($1.val == $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr OP_NE expr
			{	$$.val = ($1.val != $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr '<' expr
			{	$$.val = ($1.val < $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr '>' expr
			{	$$.val = ($1.val > $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr OP_LE expr
			{	$$.val = ($1.val <= $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr OP_GE expr
			{	$$.val = ($1.val >= $3.val);
				$$.typ = combine($1.typ, $3.typ, '>');
			}
	|	expr OP_RR expr
			{	$$.val = ($1.val >> $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr OP_LL expr
			{	$$.val = ($1.val << $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '+' expr
			{	$$.val = ($1.val + $3.val);
				$$.typ = combine($1.typ, $3.typ, '+');
			}
	|	expr '-' expr
			{	$$.val = ($1.val - $3.val);
				$$.typ = combine($1.typ, $3.typ, '-');
			}
	|	expr '*' expr
			{	$$.val = ($1.val * $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '/' expr
			{	if ($3.val == 0) {
					if (pass == PASS_3)
						serror("divide by zero");
					$$.val = 0;
				} else
					$$.val = ($1.val / $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	expr '%' expr
			{	if ($3.val == 0) {
					if (pass == PASS_3)
						serror("divide by zero");
					$$.val = 0;
				} else
					$$.val = ($1.val % $3.val);
				$$.typ = combine($1.typ, $3.typ, 0);
			}
	|	'+' expr %prec '*'
			{	$$.val = $2.val;
				$$.typ = combine(S_ABS, $2.typ, 0);
			}
	|	'-' expr %prec '*'
			{	$$.val = -$2.val;
				$$.typ = combine(S_ABS, $2.typ, 0);
			}
	|	'~' expr
			{	$$.val = ~$2.val;
				$$.typ = combine(S_ABS, $2.typ, 0);
			}
	|	DOT
			{	$$.val = DOTVAL;
				$$.typ = DOTTYP|S_DOT;
			}
	;
id_fb	:	IDENT
	|	FBSYM
	;
absexp	:	expr
			{	if (($1.typ & ~S_EXT) != S_ABS)
					serror("must be absolute");
				$$ = $1.val;
			}
	;
optabs1
	:	/* empty */
			{	$$ = 0;}
	|	absexp
			{	$$ = $1;}
	;
optabs2
	:	/* empty */
			{	$$ = 0;}
	|	',' absexp
			{	$$ = $2;}
	;

/* ========== Machine dependent rules ========== */

#include	"mach4.c"

%%
