/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "symtab.h"
#include "sizes.h"
#include "expr.h"
#include "Lpars.h"

static void rvalue(), assignable(), inputable(), outputable(), subscriptable();
static void assigned();
char *Malloc();

/* The new_* functions make use of the used() and assinged() functions to
 * make known what is done to a variable.
 */

struct expr *new_node(op, left, right, byte)
	int op;
	register struct expr *left, *right;
	int byte;
/* Makes a new node with given operator, left and right operand.
 * Constant folding is done if possible.
 */
{
	if (op!=FOR && constant(left) && (right==nil || constant(right))) {
		register long lc, rc;

		lc=left->u.cst;
		if (right) rc=right->u.cst; else rc = 0;

		switch (op) {
		case '+':	lc+=rc; break;
		case '-':	lc-=rc; break;
		case '*':	lc*=rc; break;
		case '/':	if (rc==0L)
					report("division by zero");
				else
					lc/=rc;
				break;
		case BS:	lc%=rc; break;
		case '<':	lc= lc<rc ? -1L : 0L; break;
		case '>':	lc= lc>rc ? -1L : 0L; break;
		case LE:	lc= lc<=rc ? -1L : 0L; break;
		case GE:	lc= lc>=rc ? -1L : 0L; break;
		case NE:	lc= lc!=rc ? -1L : 0L; break;
		case '=':	lc= lc==rc ? -1L : 0L; break;
		case AFTER:	lc= (lc-rc)>0 ? -1L : 0L; break;
		case BA:	lc&=rc; break;
		case BO:	lc|=rc; break;
		case BX:	lc^=rc; break;
		case AND:	lc= lc&&rc ? -1L : 0L; break;
		case OR:	lc= lc||rc ? -1L : 0L; break;
		case LS:	lc<<=rc; break;
		case RS:	lc>>=rc; break;
		case '~':	lc= -lc; break;
		case NOT:	lc= ~lc; break;
		default:
			report("illegal operator on constants");
		}
		destroy(right);

		left->u.cst=lc;
		return left;
	} else {
		register struct expr *pe;
		int type=0, arr_siz=1;

		switch (op) {
		case '+':	case '-':	case '*':	case '/':
		case BS:	case '<':	case '>':	case LE:
		case GE:	case NE:	case '=':	case AFTER:
		case BA:	case BO:	case BX:	case AND:
		case OR:	case LS:	case RS:
			rvalue(left);
			rvalue(right);
			type=T_VALUE;
			break;
		case '~':
		case NOT:
			rvalue(left);
			type=T_VALUE;
			break;
		case AS:
			assignable(left, right);
			type=T_VOID;
			break;
		case '[':
			subscriptable(left, right, byte, &type, &arr_siz);
			break;
		}
		pe= (struct expr *) Malloc(sizeof *pe);

		pe->kind=E_NODE;
		pe->type=type;
		pe->arr_siz=arr_siz;
		pe->u.node.op=op;
		pe->u.node.left=left;
		pe->u.node.right=right;

		return pe;
	}
}

struct expr *new_var(var)
	register struct symbol *var;
/* Given a variable an expression node is constructed.  Note the changes in
 * type!  T_VAR becomes T_VALUE with flag T_LVALUE.
 */
{
	register struct expr *pe;

	pe= (struct expr *) Malloc(sizeof *pe);

	pe->kind=E_VAR;

	if ((var->s_type&T_TYPE)==T_VAR || var->s_type&T_NOTDECL) {
		pe->type=(var->s_type&(~T_TYPE));
		pe->type|=T_VALUE|T_LVALUE;
	} else
		pe->type=var->s_type;

	pe->arr_siz=var->s_arr_siz;

	pe->u.var=var;

	return pe;
}

struct expr *new_const(cst)
	long cst;
/* Make a constant, which is a VALUE, of course. */
{
	register struct expr *pe;

	pe= (struct expr *) Malloc(sizeof *pe);

	pe->kind=E_CONST;
	pe->type=T_VALUE;
	pe->u.cst=cst;

	return pe;
}

struct expr *new_table(kind, tab)
	register kind;
	register struct table *tab;
/* One table is being made, it is no doubt a VALUEd ARRay, but maybe even a
 * BYTE array.  A label is reserved for it and the individual elements are
 * rommified.
 */
{
	register struct expr *pe;

	pe= (struct expr *) Malloc(sizeof *pe);

	pe->kind=kind;
	pe->type=T_VALUE|T_ARR;
	if (kind==E_BTAB) pe->type|=T_BYTE;
	dot_label(new_dot_label(&pe->u.tab));

	pe->arr_siz=0;
	while (tab!=nil) {
		register struct table *junk=tab;
		
		rom(kind==E_BTAB ? 1 : vz, tab->val);

		tab=tab->next;
		pe->arr_siz++;
		free((char *)junk);
	}

	return pe;
}

struct expr *copy_const(e) struct expr *e;
/* If you double it up, you've got one you can throw away.  (Or do something
 * useful with).
 */
{
	register struct expr *c;

	c= (struct expr *) Malloc(sizeof *c);

	*c= *e;
	return c;
}

struct expr *new_now()
/* Now is the time to make a VALUE cell for the clock. */
{
	register struct expr *pe;

	pe= (struct expr *) Malloc(sizeof *pe);

	pe->kind=E_NOW;
	pe->type=T_VALUE;

	return pe;
}

struct expr *new_io(out, chan, args)
	int out;
	register struct expr *chan;
	struct expr_list *args;
/* Either c ? v0; v1; v2; ... (out=0) or c ! e0; e1; e2; ... (out=1). */
{
	register struct expr *pe;

	if ( ( (chan->type&T_TYPE) != T_CHAN || (chan->type&T_ARR) )
		&& ! (chan->type&T_NOTDECL)
	)
		report("channel variable expected");
	used(chan);

	pe= (struct expr *) Malloc(sizeof *pe);

	pe->kind=E_IO;
	pe->type=T_VOID;
	pe->u.io.out=out;
	pe->u.io.chan=chan;
	pe->u.io.args=args;

	return pe;
}

struct expr *new_call(proc, args)
	struct expr *proc;
	struct expr_list *args;
/* Dial proc(arg1, arg2, ...) and you'll hear the tone of this function.
 * Dialing yourself is not allowed, but it will work if you ignore the
 * compiler generated noise.
 */
{
	register struct expr *pe;

	pe= (struct expr *) Malloc(sizeof *pe);

	used(proc);

	check_recursion(proc);

	pe->kind=E_CALL;
	pe->type=T_VOID;
	pe->u.call.c_proc=proc;
	pe->u.call.c_args=args;

	return pe;
}

void table_add(aapt, val) register struct table ***aapt; long val;
/* Adds a value to a table using a hook to a hook. */
{
	register struct table *pt;

	pt= (struct table *) Malloc(sizeof *pt);

	pt->val=val;
	pt->next= **aapt;

	**aapt=pt;
	*aapt= &pt->next;
}

void expr_list_add(aaelp, arg)
	register struct expr_list ***aaelp;
	struct expr *arg;
/* Another add, this time for actual arguments and the like. */
{
	register struct expr_list *elp;

	elp= (struct expr_list *) Malloc(sizeof *elp);

	elp->arg=arg;
	elp->next= **aaelp;
	**aaelp=elp;
	*aaelp= &elp->next;
}

void check_io(out, arg) int out; struct expr *arg;
{
	if (out)
		outputable(arg);
	else
		inputable(arg);
}

void check_wait(e) struct expr *e;
{
	if ((e->type&T_TYPE)!=T_VALUE)
		report("WAIT process needs valued operand");
}

static void assigned(e) register struct expr *e;
/* Tries to tell e that it is assigned to. */
{
	if (e->kind==E_VAR || (e->kind==E_NODE && e->u.node.op=='['
		&& (e=e->u.node.left)->kind==E_VAR)
	) {
		register struct symbol *var;

		if ((var=e->u.var)->s_type&T_REP) {
			warning("replicator index %s may not be assigned",
				var->s_name);
			var->s_type&= ~T_REP;
		}
		var->s_type|=T_ASSIGNED;
	}
}

void used(e) register struct expr *e;
{
	if (e->kind==E_VAR || (e->kind==E_NODE && e->u.node.op=='['
		&& (e=e->u.node.left)->kind==E_VAR)
	) {
		register struct symbol *var;

		if ( ! ( (var=e->u.var)->s_type&(T_ASSIGNED|T_BUILTIN))
		    && (var->s_type&T_TYPE)==T_VAR
		    && var->s_info.vc.st.level==curr_level)
			warning("%s used before assigned", var->s_name);
		var->s_type|=(T_USED|T_ASSIGNED);
	}
}

static void rvalue(e) register struct expr *e;
{
	if ((e->type&T_TYPE)!=T_VALUE || e->type&T_ARR)
		report("illegal operand of arithmetic operator");
	used(e);
}

static void assignable(l, r) register struct expr *l, *r;
/* See if l can be assigned r. */
{
	if ( ! ( (l->type&T_LVALUE && (r->type&T_TYPE)==T_VALUE
		  && (l->type&T_ARR)==(r->type&T_ARR))
		|| (l->type|r->type)&T_NOTDECL
	))
		report("operands of assignment are not conformable");
	else
	if (l->type&T_ARR && ! ( (l->type|r->type)&T_NOTDECL ) ) {
		register lsiz=l->arr_siz, rsiz=r->arr_siz;

		if (lsiz!=0 && rsiz!=0 && lsiz!=rsiz)
			report("arrays have incompatible sizes");
	}
	used(r);
	assigned(l);
	
}

static void inputable(e) struct expr *e;
{
	if ( ! (e->type&T_LVALUE) )
		report("operand of input process can't be assigned");

	assigned(e);
}

static void outputable(e) struct expr *e;
{
	if ( ! ( (e->type&T_TYPE)==T_VALUE ) )
		report("operand of output process has no value");
	used(e);
}

static void subscriptable(l, r, byte, atype, arr_siz)
	register struct expr *l, *r;
	register byte;
	int *atype, *arr_siz;
/* Tries to subscript l by r, returning type and array size for slices. */
{
	register type= (l->type&T_TYPE)|byte;

	if ( !(l->type&(T_ARR|T_NOTDECL) ) )
		report("indexing on a non-array");
	else
	if ( ! ( (r->type&T_TYPE)==T_VALUE
		|| (r->kind==E_NODE && r->u.node.op==FOR)
	) )
		report("index is not computable");

	type|=(l->type&T_LVALUE);

	if (r->kind==E_NODE && r->u.node.op==FOR) {
		type|=T_ARR;
		if (r->u.node.right->kind!=E_CONST)
			report("slice must be of constant size");
		else
			*arr_siz=r->u.node.right->u.cst;
		used(r->u.node.left);
	} else
		used(r);
	*atype=type;
}

void check_param(aform, act, err)
	struct par_list **aform;
	register struct expr *act;
	int *err;
/* Test if formal parameter *aform corresponds with actual act.  Err returns
 * error status.  The aform hook is set to the next formal after the check.
 */
{
	register struct par_list *form= *aform;
	register struct expr *left;
	register struct symbol *var;
	static char NONCORR[]="actual and formal parameter don't correspond";

	if (form==nil) {
		if (! *err) {
			report("too many actual parameters");
			*err=1;
		}
		return;
	}

	if ((form->pr_type&T_ARR)!=(act->type&T_ARR) && !(act->type&T_NOTDECL) ) {
			report(NONCORR);
	} else {
		switch (form->pr_type&T_TYPE) {
		case T_VAR:
			if ( ! (
				(act->type&T_TYPE)==T_VALUE
				&& act->type&T_LVALUE
				&& !(act->type&T_BYTE)
			))
				report(NONCORR);
			assigned(act);
			used(act);
			break;
		case T_CHAN:
			if((act->type&T_TYPE)!=T_CHAN && !(act->type&T_NOTDECL))
				report(NONCORR);
			used(act);
			break;
		case T_VALUE:
			if ((act->type&T_TYPE)!=T_VALUE)
				report(NONCORR);
			used(act);
			break;
		}
	}
	*aform= form->pr_next;
}

void destroy(e) register struct expr *e;
/* Opposite of making. */
{
	if (e!=nil) {
		switch (e->kind) {
		case E_NODE:
			destroy(e->u.node.left);
			destroy(e->u.node.right);
			break;
		case E_IO:
		case E_CALL:
			destroy(e->kind==E_IO ? e->u.io.chan : e->u.call.c_proc);
			{
				register struct expr_list *elp, *junk;

				elp= e->kind==E_IO ? e->u.io.args : e->u.call.c_args;

				while (elp!=nil) {
					destroy(elp->arg);
					junk=elp;
					elp=elp->next;
					free((char *)junk);
				}
			}
			break;
		}
		free((char *)e);
	}
}
