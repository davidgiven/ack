/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	OCCAM		*/
{
#include <stdlib.h>
#include "token.h"
#include "symtab.h"
#include "expr.h"
#include "code.h"
#include "sizes.h"
#include <system.h>
#include <em.h>

#define MAXERRORS	10	/* Maximum number of insert/delete errors */

static void nonconst(), nonpositive(), rep_cleanup(), check_assoc();
void init_builtins();
char *strcpy();

extern int lineno, LLsymb;
union type_info info, none;
}
%token	AFTER, ALLOCATE, ALT, AND, ANY, BYTE, CHAN, DEF, FALSE, FOR, IF, LOAD;
%token	NOT, NOW, OR, PAR, PLACED, PORT, PRI, PROC, SEQ, SKIP, TABLE, TRUE;
%token	VALUE, VAR, WAIT, WHILE;
%token	IDENTIFIER, NUMBER, CHAR_CONST, STRING;
%token	AS, LE, GE, NE, LS, RS, BA, BO, BX, BS;

%start	occam, program;

program	: 			{	init_builtins();
					header();
				}
	  process
	;

process	: primitive
	| construct
	|			{	sym_down(); }
	  declaration ':' process
				{	sym_up(); }
	;

primitive { struct expr *e; } :
	  statement(&e)		{	if (!valueless(e))
						report("primitive may not have a value");
					code_void(e);
					destroy(e);
				}
	| WAIT val_expr(&e)	{	int BEGIN=0, END=0, TEST=0;
					check_wait(e);
					no_deadlock();
					branch(&TEST);
					Label(new_label(&BEGIN));
					resumenext();
					Label(TEST);
					code_bool(e, positive, &END, &BEGIN);
					Label(END);
					destroy(e);
				}
	| SKIP
	;

guard(register *F;)		{	struct expr *e1, *e2;
					register full_guard=0;
					int T=0;
		static char EXPECT_INP[]="input process expected as guard";
				} :
	  expression(&e1)
	  [	  '&'		{	full_guard=1;
					if (!valued(e1))
						report("boolean part of guard has no value");
					code_bool(e1, positive, &T, F);
					Label(T);
				}
		  [	  statement(&e2)
				{	if (!input_process(e2))
						report(EXPECT_INP);
					code_any(e2, F);
					destroy(e2);
				}
			  | WAIT val_expr(&e2)
				{	check_wait(e2);
					code_bool(e2, positive, &T, F);
					Label(T);
					destroy(e2);
				}
			  | SKIP
		  ]
	  ]?
				{	if (!full_guard) {
						if (!input_process(e1))
							report(EXPECT_INP);
						code_any(e1, F);
					}
					destroy(e1);
				}
	| WAIT val_expr(&e1)
				{	check_wait(e1);
					code_bool(e1, positive, &T, F);
					Label(T);
					destroy(e1);
				}
	| SKIP
	;

guarded_process(register *END;)	{	struct symbol *v;
					struct expr *e1, *e2;
					struct replicator to_test;
					register line, oind;
					int F=0;
				} :
	  guard(&F) process	{	branch(END);
					Label(F);
				}
	| ALT			{	line=lineno; oind=ind; }
	  [	  %if (line==lineno)
		  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test); }
		  guarded_process(END)
				{	rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
		|
		  [ %while (tabulated(oind, ind)) guarded_process(END) ]*
	  ]
	;

conditional(register *END; )	{	struct symbol *v;
					struct expr *e1, *e2;
					struct replicator to_test;
					register line, oind;
					int T=0, F=0;
				} :
	  val_expr(&e1)		{	if (!valued(e1))
						report("conditional needs valued expression");
					code_bool(e1, positive, &T, &F);
					Label(T);
					destroy(e1);
				}
	  process
				{	branch(END);
					Label(F);
				}
	| IF			{	line=lineno; oind=ind; }
	  [	  %if (line==lineno)
		  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test); }
		  conditional(END)
				{	rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
		|
		  [ %while (tabulated(oind, ind)) conditional(END) ]*
	  ]
	;

replicator(register struct symbol **s; register struct expr **e1; register struct expr **e2; )
				{	register char *index; }:
	  IDENTIFIER		{	index=token.t_sval; }
	  '=' '[' val_expr(e1) FOR val_expr(e2) ']'
				{	if (!valued(*e1) || !valued(*e2))
						report("replicator needs valued expressions");
					sym_down();
					var_memory(&info, T_VAR, 1);
					*s=insert(index,
					T_VAR|T_REP|T_USED|T_ASSIGNED, 1, &info);
				}
	;

construct			{	struct symbol *v;
					struct expr *e1, *e2;
					struct replicator to_test;
					register line, oind;
					int BEGIN=0, END=0, NONZERO;
				}:
	  SEQ			{	line=lineno; oind=ind; }
	  [	  %if (line==lineno)
		  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test); }
		  process
				{	rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
		|
		  [ %while (tabulated(oind, ind)) process ]*
	  ]
	| PRI ?
	  [	  PAR		{	line=lineno; oind=ind;
					par_begin();
				}
		  [	  %if (line==lineno)
			  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test);
					NONZERO=0;
					par_fork(&NONZERO);
				}
			  process
				{	branch(&END);
					Label(NONZERO);
					rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
			|
			  [ %while (tabulated(oind, ind))
				{	NONZERO=0;
					par_fork(&NONZERO);
				}
				  process
				{	branch(&END);
					Label(NONZERO);
				}
			  ]*
		  ]
				{	Label(END);
					par_end();
				}
		| ALT		{	line=lineno; oind=ind;
					no_deadlock();
					Label(new_label(&BEGIN));
				}
		  [	  %if (line==lineno)
			  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test); }
			  guarded_process(&END)
				{	rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
			|
			  [ %while (tabulated(oind, ind)) guarded_process(&END)
			  ]*
		  ]
				{	resumenext();
					branch(&BEGIN);
					Label(END);
				}
	  ]
	| IF			{	line=lineno; oind=ind; }
	  [	  %if (line==lineno)
		  replicator(&v, &e1, &e2)
				{	rep_init(v, e1, e2, &to_test); }
		  conditional(&END)
				{	rep_test(v, e1, e2, &to_test);
					rep_cleanup(e1, e2);
				}
		|
		  [ %while (tabulated(oind, ind)) conditional(&END) ]*
	  ]
				{	Label(END); }
	| WHILE val_expr(&e1)	{	if (!valued(e1))
						report("WHILE needs valued expression");
					branch(&END);
					Label(new_label(&BEGIN));
				}
	  process
				{	int DONE=0;
					Label(END);
					code_bool(e1, negative, &DONE, &BEGIN);
					Label(DONE);
					destroy(e1);
				}
	;

subscript(register *byte; register struct expr **e; )
				{	struct expr *e1;
					register slice=0, err=0;
				} :
	  '['			{	*byte=0; }
	  [	  BYTE		{	*byte=T_BYTE; }
	  ]?
	  val_expr(e)		{	if (!valued(*e))
						err++;
				}
	  [	  FOR expression(&e1)
	  			{	static char siz[]="slize size";
					if (!constant(e1)) {
						if (!err)
							nonconst(siz);
						destroy(e1);
						e1=new_const(0L);
					} else
					if (e1->u.cst<=0)
						nonpositive(siz);
					*e=new_node(FOR, *e, e1, *byte);
					slice=1;
				}
	  ]?
	  ']'
				{	if (err)
						report(slice ?
				"slice must be '[' value FOR constant ']'" :
				"subscript needs valued expression");
				}
	;

chan	{ register type, arr_siz=1; register char *name; struct expr *e; }:
	  IDENTIFIER		{	type=T_CHAN;
					name=token.t_sval;
				}
	  [	  '[' expression(&e) ']'
				{	static char siz[]="channel array size";
					if (!constant(e))
						nonconst(siz);
					else
					if (e->u.cst<0)
						nonpositive(siz);
					else
						arr_siz=e->u.cst;
					destroy(e);
					type|=T_ARR;
				}
	  ]?
				{	chan_memory(&info, arr_siz);
					chan_init(&info, arr_siz);
					insert(name, type, arr_siz, &info);
				}
	;

var				{	register type, byte=0, arr_siz=1;
					register char *name;
					struct expr *e;
				}:
	  IDENTIFIER		{	type=T_VAR; name=token.t_sval; }
	  [	  '['
		  [	  BYTE	{	byte=T_BYTE; }
		  ]?
		  expression(&e) ']'
				{	static char siz[]="variable array size";
					if (!constant(e))
						nonconst(siz);
					else
					if (e->u.cst<=0)
						nonpositive(siz);
					else
						arr_siz=e->u.cst;
					destroy(e);
					type|=T_ARR|byte;
				}
	  ]?
				{	var_memory(&info, type, arr_siz);
					insert(name, type, arr_siz, &info);
				}
	;

const_def { register char *name; struct expr *e; }:
	  IDENTIFIER		{	name=token.t_sval; }
	  '=' expression(&e)
				{	if (!constant(e) && !arr_constant(e))
						nonconst("expression in constant definition");
					info.t_const=e;
					insert(name, T_CONST|T_USED, 0, &info);
				}
	;

form_parm(register struct par_list ***aapars; register *g_type;)
				{	register char *name;
					register type= *g_type;
				}:
	  [	  VAR		{	type=T_VAR|T_ASSIGNED|T_USED; }
		| CHAN		{	type=T_CHAN; }
		| VALUE		{	type=T_VALUE|T_ASSIGNED; }
	  ]?
	  IDENTIFIER		{
					if (type<0) {
						report("VAR, CHAN or VALUE expected");
						type=T_VAR;
					}
					name=token.t_sval;
					*g_type=type;
				}
	  [	  '[' ']'
				{	type|=T_ARR; }
	  ]?
				{	pars_add(aapars, type&(T_TYPE|T_ARR),
					  insert(name, type|T_PARAM, 0, &none));
				}
	;

form_parms(struct par_list **apars;) { int type= -1; }:
	  '(' form_parm(&apars, &type)
	  [	  ',' form_parm(&apars, &type)
	  ]*
	  ')'
	;

declaration:
	  VAR
	  var [ ',' var ]*
	| CHAN
	  chan [ ',' chan ]*
	| DEF
	  const_def [ ',' const_def ]*
	| proc_declaration
	;
	
proc_declaration		{	struct par_list *pars=nil;
					register struct symbol *proc;
					int OVER=0;
					register old_min_offset;
				}:
	  PROC IDENTIFIER	{	branch(&OVER);
					proc=insert(token.t_sval,
						T_PROC|T_RECURS, 0, &none);
					old_min_offset=min_offset;
					sym_down();
					prologue(proc);
				}
	  form_parms(&pars) ?	{	form_offsets(pars);
					proc->s_info.proc.pars=pars;
				}
	  '=' process		{	epilogue(proc);
					sym_up();
					proc->s_type&= ~T_RECURS;
					min_offset=old_min_offset;
					Label(OVER);
				}
	;

vector_constant(register struct expr **e;)
				{	struct table *pt=nil, **apt= &pt;
					register Tlen=0;
				}:
	  table(e)
	| STRING		{	register char *ps= token.t_sval;
					register len;

					Tlen+= len= (*ps++ & 0377);
					while (--len>=0)
						table_add(&apt, (long) *ps++);
				}
	  [ %while (1)	  STRING
				{	register char *ps= token.t_sval;
					register len;

					Tlen+= len= (*ps++ & 0377);
					while (--len>=0)
						table_add(&apt, (long) *ps++);
				}
	  ]*
				{	apt= &pt;
					table_add(&apt, (long) Tlen);
					*e=new_table(E_BTAB, pt);
				}
	;

item(register struct expr **e;)
				{	struct expr *e1;
					register struct symbol *var;
					struct par_list *pars=nil;
					register line, oind;
					int byte, err=0, subs_call=0;
					struct expr_list *elp=nil, **aelp= &elp;
				}:
	  IDENTIFIER		{	line=lineno;
					oind=ind;
					var=searchall(token.t_sval);

					if (var_constant(var))
						*e=copy_const(var->s_info.t_const);
					else {
						if (var_proc(var))
							pars=var->s_info.proc.pars;
						*e=new_var(var);
					}
				}
	  [ %while (line==lineno || tabulated(oind, ind))
		  [	  subscript(&byte, &e1)
				{	*e=new_node('[', *e, e1, byte); }
			| '('	{	if (!var_declared(var)) {
						var->s_type=T_PROC|T_USED|T_NOTDECL;
						var->s_info.proc.pars=nil;
						err=1;
					}
					if (!var_proc(var)) {
						report("%s is not a named process",
							var->s_name);
						err=1;
					}
				}
			  expression(&e1)
				{ 	check_param(&pars, e1, &err);
					expr_list_add(&aelp, e1);
				}
			  [	  ',' expression(&e1)
				{ 	check_param(&pars, e1, &err);
					expr_list_add(&aelp, e1);
				}
			  ]*
				{
					if (pars!=nil)
						report("too few actual parameters");
				}
			  ')'
				{	*e=new_call(*e, elp); }
		  ]
				{	subs_call=1; }
	  ]?
				{	if (!subs_call && var_proc(var)) {
						if (pars!=nil)
							report("no actual parameters");
						*e=new_call(*e, (char *)nil);
					}
				}
	| vector_constant(e)
	  [	  subscript(&byte, &e1)
				{	*e=new_node('[', *e, e1, byte); }
	  ]?
	;

statement(register struct expr **e;)
				{	struct expr *e1;
					struct expr_list *elp=nil, **aelp= &elp;
					register out;
				}:
	  item(e)
	  [	  AS expression(&e1)
				{	*e=new_node(AS, *e, e1, 0); }
		| [
			  '?'	{	out=0; }
			| '!'	{	out=1; }
		  ]
		  io_arg(&e1)
				{	if (e1!=nil) check_io(out, e1);
					expr_list_add(&aelp, e1);
				}
		  [ %while (1) ';' io_arg(&e1)
				{	if (e1!=nil) check_io(out, e1);
					expr_list_add(&aelp, e1);
				}
		  ]*
				{	*e=new_io(out, *e, elp); }
	  ]?
	;

io_arg(struct expr **e; ) :
	  expression(e)
	| ANY			{	*e=nil;	}
	;

table(register struct expr **e;)	
				{	struct table *pt=nil, **apt= &pt;
					struct expr *e1;
					register type;
				}:
	  TABLE '['		{	type=E_TABLE; }
	  [	  BYTE		{	type=E_BTAB; }
	  ]?
	  expression(&e1)	{	if (!constant(e1))
						nonconst("table element");
					else
						table_add(&apt, e1->u.cst);
					destroy(e1);
				}
	  [	  ',' expression(&e1)
				{	if (!constant(e1))
						nonconst("table element");
					else
						table_add(&apt, e1->u.cst);
					destroy(e1);
				}
	  ]*
				{	*e=new_table(type, pt); }
	  ']'
	;

arithmetic_op:	'+' | '-' | '*' | '/' | BS
	;

comparison_op:	'<' | '>' | LE | GE | NE | '=' | AFTER
	;

logical_op:	BA | BO | BX
	;

boolean_op:	AND | OR
	;

shift_op:	LS | RS
	;

monadic_op(register *op;):
	  '-'			{	*op='~'; }
	| NOT			{	*op=NOT; }
	;

operator: arithmetic_op | comparison_op | logical_op | boolean_op | shift_op
	;

element(register struct expr **e;) :
	  %default NUMBER	{	*e=new_const(token.t_lval); }
	| statement(e)
	| TRUE			{	*e=new_const(-1L); }
	| FALSE			{	*e=new_const(0L); }
	| NOW			{	*e=new_now(); }
	| CHAR_CONST		{	*e=new_const(token.t_lval); }
	| '(' expression(e) ')' {	if (valueless(*e))
						warning("primitive should not be parenthesized");
				}
	;

expression(register struct expr **e;)
				{	int op=0;
					struct expr *e1;
				}:
	  element(e)
	  [ %while (1)		{	if (op!=0) check_assoc(op, LLsymb);
					op=LLsymb;
				}
		  operator element(&e1)
				{	*e=new_node(op, *e, e1, 0); }
	  ]*
	| monadic_op(&op) element(&e1)
				{	*e=new_node(op, e1, (char *)nil, 0); }
	;

val_expr(register struct expr **e;) :
	  expression(e)		{	used(*e); }
	;

%lexical scanner;
{
int err=0;

main(argc, argv) register argc; register char **argv;
{
	while (argc > 1 && argv[1][0] == '-') {
		do_option(&argv[1][1]);
		argc--;
		argv++;
	}

	leader();
	occam();
	trailer();

	exit(err);
}

do_option(text)
	char *text;
{
	extern int Lflag;

	switch(*text++)	{

	default:
		fatal("illegal option: %c", *--text);

	case 'L' :			/* no fil/lin */
		Lflag++;
		break;
	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		arith size, align;
		char c;

		while (c = *text++)	{
			size = txt2int(&text);
			switch (c)	{
			case 'w':	/* word		*/
				if (size != (arith)0)
					wz = size;
				break;
			case 'p':	/* pointer	*/
				if (size != (arith)0)
					pz = size;
				break;
			case 'l':	/* long		*/
				if (size != (arith)0)
					vz = size;
				break;
			default:
				fatal("-V: bad type indicator %c\n", c);
			}
		}
		break;
	}
	}
}

int
txt2int(tp)
	char **tp;
{
	/*	the integer pointed to by *tp is read, while increasing
		*tp; the resulting value is yielded.
	*/
	register int val = 0, ch;
	
	while (ch = **tp, ch >= '0' && ch <= '9')	{
		val = val * 10 + ch - '0';
		(*tp)++;
	}
	return val;
}

LLmessage(tk) register tk;
{
	static errors=0;

	if (tk>0) {
		repeat_token(LLsymb);
		warning("syntax error: %s expected (inserted)", tokenname(tk, 1));
	} else
	if (tk==0)
		warning("syntax error: bad token %s (deleted)", tokenname(LLsymb, 0));
	else { /* tk<0 */
		warning("syntax error: garbage at end of program");
	}
	if (++errors==MAXERRORS) {
		fprint(STDERR, "Too many insert/delete errors. Compiler ends.\n");
		err=1; trailer(); exit(1);
	}
}

static void nonconst(siz) char *siz;
{
	report("%s should be a constant", siz);
}

static void nonpositive(siz) char *siz;
{
	report("%s must be positive", siz);
}

static void rep_cleanup(e1, e2) struct expr *e1, *e2;
{
	destroy(e1);
	destroy(e2);
	sym_up();
}

static void check_assoc(prev_op, op) register prev_op, op;
{
	switch (op) {
		char prev[5];
	case '+':	case '*':
	case AND:	case OR:
	case BA:	case BO:	case BX:
		if (prev_op==op) break;
	default:
		strcpy(prev, tokenname(prev_op, 0));

		warning("Operators %s and %s don't associate",
			prev, tokenname(op, 0)
		);
	}
}

void
No_Mem()
{
	fatal("out of memory");
}
}
