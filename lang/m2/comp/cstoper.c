/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C O N S T A N T   E X P R E S S I O N   H A N D L I N G */

/* $Header$ */

#include	"debug.h"
#include	"target_sizes.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	<alloc.h>

#include	"idf.h"
#include	"type.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"
#include	"standards.h"
#include	"warning.h"

extern char	*symbol2str();

#define arith_sign	((arith) (1L << (sizeof(arith) * 8 - 1)))

arith full_mask[MAXSIZE];/* full_mask[1] == 0xFF, full_mask[2] == 0xFFFF, .. */
arith max_int[MAXSIZE];	/* max_int[1] == 0x7F, max_int[2] == 0x7FFF, .. */
arith min_int[MAXSIZE];	/* min_int[1] == 0xFFFFFF80, min_int[2] = 0xFFFF8000,
			   ...
			*/
#ifndef NOCROSS
unsigned int wrd_bits;	/* number of bits in a word */
#endif

extern char options[];

overflow(expp)
	t_node *expp;
{
	if (expp->nd_type != address_type) {
	    node_warning(expp, W_ORDINARY, "overflow in constant expression");
	}
}

underflow(expp)
	t_node *expp;
{
	if (expp->nd_type != address_type) {
	    node_warning(expp, W_ORDINARY, "underflow in constant expression");
	}
}

STATIC
commonbin(expp)
	register t_node **expp;
{
	register t_type *tp = (*expp)->nd_type;
	register t_node *right = (*expp)->nd_RIGHT;
	
	(*expp)->nd_RIGHT = 0;
	FreeNode(*expp);
	*expp = right;
	right->nd_type = tp;
}

cstunary(expp)
	t_node **expp;
{
	/*	The unary operation in "expp" is performed on the constant
		expression below it, and the result restored in expp.
	*/
	register t_node *exp = *expp;
	register t_node *right = exp->nd_RIGHT;
	register arith o1 = right->nd_INT;

	switch(exp->nd_symb) {
	/* Should not get here
	case '+':
		break;
	*/

	case '-':
		if (o1 == min_int[(int)(right->nd_type->tp_size)]) {
			overflow(exp);
		}
		o1 = -o1;
		break;

	case NOT:
	case '~':
		o1 = !o1;
		break;

	default:
		crash("(cstunary)");
	}

	commonbin(expp);
	(*expp)->nd_INT = o1;
	CutSize(*expp);
}

STATIC
divide(pdiv, prem)
	arith *pdiv, *prem;
{
	/*	Unsigned divide *pdiv by *prem, and store result in *pdiv,
		remainder in *prem
	*/
	register arith o1 = *pdiv;
	register arith o2 = *prem;

	/*	this is more of a problem than you might
		think on C compilers which do not have
		unsigned long.
	*/
	if (o2 & arith_sign)	{/* o2 > max_arith */
		if (! (o1 >= 0 || o1 < o2)) {
			/*	this is the unsigned test
				o1 < o2 for o2 > max_arith
			*/
			*prem = o2 - o1;
			*pdiv = 1;
		}
		else {
			*pdiv = 0;
		}
	}
	else	{		/* o2 <= max_arith */
		arith half, bit, hdiv, hrem, rem;

		half = (o1 >> 1) & ~arith_sign;
		bit = o1 & 01;
		/*	now o1 == 2 * half + bit
			and half <= max_arith
			and bit <= max_arith
		*/
		hdiv = half / o2;
		hrem = half % o2;
		rem = 2 * hrem + bit;
		*pdiv = 2*hdiv;
		*prem = rem;
		if (rem < 0 || rem >= o2) {
			/*	that is the unsigned compare
				rem >= o2 for o2 <= max_arith
			*/
			*pdiv += 1;
			*prem -= o2;
		}
	}
}

cstibin(expp)
	t_node **expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in expp.
		This version is for INTEGER expressions.
	*/
	register t_node *exp = *expp;
	register arith o1 = exp->nd_LEFT->nd_INT;
	register arith o2 = exp->nd_RIGHT->nd_INT;
	register int sz = exp->nd_type->tp_size;

	assert(exp->nd_class == Oper);
	assert(exp->nd_LEFT->nd_class == Value);
	assert(exp->nd_RIGHT->nd_class == Value);

	switch (exp->nd_symb)	{
	case '*':
		if (o1 > 0 && o2 > 0) {
			if (max_int[sz] / o1 < o2) overflow(exp);
		}
		else if (o1 < 0 && o2 < 0) {
			if (o1 == min_int[sz] || o2 == min_int[sz] ||
			    max_int[sz] / (-o1) < (-o2)) overflow(exp);
		}
		else if (o1 > 0) {
			if (min_int[sz] / o1 > o2) overflow(exp);
		}
		else if (o2 > 0) {
			if (min_int[sz] / o2 > o1) overflow(exp);
		}
		o1 *= o2;
		break;

	case DIV:
		if (o2 == 0)	{
			node_error(exp, "division by 0");
			return;
		}
		if ((o1 < 0) != (o2 < 0)) {
			if (o1 < 0) o1 = -o1;
			else o2 = -o2;
			o1 = -((o1+o2-1)/o2);
		}
		else {
			o1 /= o2;
		}
		break;
	case MOD:
		if (o2 == 0)	{
			node_error(exp, "modulo by 0");
			return;
		}
		if ((o1 < 0) != (o2 < 0)) {
			if (o1 < 0) o1 = -o1;
			else o2 = -o2;
			o1 = ((o1+o2-1)/o2) * o2 - o1;
		}
		else {
			o1 %= o2;
		}
		break;

	case '+':
		if (o1 > 0 && o2 > 0) {
			if (max_int[sz] - o1 < o2) overflow(exp);
		}
		else if (o1 < 0 && o2 < 0) {
			if (min_int[sz] - o1 > o2) overflow(exp);
		}
		o1 += o2;
		break;

	case '-':
		if (o1 >= 0 && o2 < 0) {
			if (max_int[sz] + o2 < o1) overflow(exp);
		}
		else if (o1 < 0 && o2 >= 0) {
			if (min_int[sz] + o2 > o1) overflow(exp);
		}
		o1 -= o2;
		break;

	case '<':
		o1 = (o1 < o2);
		break;

	case '>':
		o1 = (o1 > o2);
		break;

	case LESSEQUAL:
		o1 = (o1 <= o2);
		break;

	case GREATEREQUAL:
		o1 = (o1 >= o2);
		break;

	case '=':
		o1 = (o1 == o2);
		break;

	case '#':
		o1 = (o1 != o2);
		break;

	default:
		crash("(cstibin)");
	}

	commonbin(expp);
	(*expp)->nd_INT = o1;
	CutSize(*expp);
}

cstfbin(expp)
	t_node **expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in expp.
		This version is for REAL expressions.
	*/
	register t_node *exp = *expp;
	register struct real *p = exp->nd_LEFT->nd_REAL;
	register flt_arith *o1 = &p->r_val;
	register flt_arith *o2 = &exp->nd_RIGHT->nd_RVAL;
	int compar = 0;
	int cmpval = 0;

	assert(exp->nd_class == Oper);
	assert(exp->nd_LEFT->nd_class == Value);
	assert(exp->nd_RIGHT->nd_class == Value);

	switch (exp->nd_symb)	{
	case '*':
		flt_mul(o1, o2, o1);
		break;

	case '/':
		flt_div(o1, o2, o1);
		break;

	case '+':
		flt_add(o1, o2, o1);
		break;

	case '-':
		flt_sub(o1, o2, o1);
		break;

	case '<':
	case '>':
	case LESSEQUAL:
	case GREATEREQUAL:
	case '=':
	case '#':
		compar++;
		cmpval = flt_cmp(o1, o2);
		switch(exp->nd_symb) {
		case '<':		cmpval = (cmpval < 0); break;
		case '>':		cmpval = (cmpval > 0); break;
		case LESSEQUAL:		cmpval = (cmpval <= 0); break;
		case GREATEREQUAL:	cmpval = (cmpval >= 0); break;
		case '=':		cmpval = (cmpval == 0); break;
		case '#':		cmpval = (cmpval != 0); break;
		}
		if (exp->nd_RIGHT->nd_RSTR) free(exp->nd_RIGHT->nd_RSTR);
		free_real(exp->nd_RIGHT->nd_REAL);
		break;

	default:
		crash("(cstfbin)");
	}

	switch(flt_status) {
	case FLT_OVFL:
		node_warning(exp, "floating point overflow on %s", 
				symbol2str(exp->nd_symb));
		break;
	case FLT_DIV0:
		node_error(exp, "division by 0.0");
		break;
	}

	if (p->r_real) {
		free(p->r_real);
		p->r_real = 0;
	}
	if (compar) {
		free_real(p);
	}
	commonbin(expp);
	exp = *expp;
	if (compar) {
		exp->nd_symb = INTEGER;
		exp->nd_INT = cmpval;
	}
	else {
		exp->nd_REAL = p;
	}
	CutSize(exp);
}

cstubin(expp)
	t_node **expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in
		expp.
	*/
	register t_node *exp = *expp;
	arith o1 = exp->nd_LEFT->nd_INT;
	arith o2 = exp->nd_RIGHT->nd_INT;
	register int sz = exp->nd_type->tp_size;
	arith tmp1, tmp2;

	assert(exp->nd_class == Oper);
	assert(exp->nd_LEFT->nd_class == Value);
	assert(exp->nd_RIGHT->nd_class == Value);

	switch (exp->nd_symb)	{
	case '*':
		if (o1 == 0 || o2 == 0) {
			o1 = 0;
			break;
		}
		tmp1 = full_mask[sz];
		tmp2 = o2;
		divide(&tmp1, &tmp2);
		if (! chk_bounds(o1, tmp1, T_CARDINAL)) overflow(exp);
		o1 *= o2;
		break;

	case DIV:
		if (o2 == 0)	{
			node_error(exp, "division by 0");
			return;
		}
		divide(&o1, &o2);
		break;

	case MOD:
		if (o2 == 0)	{
			node_error(exp, "modulo by 0");
			return;
		}
		divide(&o1, &o2);
		o1 = o2;
		break;

	case '+':
		if (! chk_bounds(o2, full_mask[sz] - o1, T_CARDINAL)) {
			overflow(exp);
		}
		o1 += o2;
		break;

	case '-':
		if (! chk_bounds(o2, o1, T_CARDINAL)) {
			if (exp->nd_type->tp_fund == T_INTORCARD) {
				exp->nd_type = int_type;
				if (! chk_bounds(min_int[sz], o1 - o2, T_CARDINAL)) {
					underflow(exp);
				}
			}
			else	underflow(exp);
		}
		o1 -= o2;
		break;

	case '<':
		o1 = ! chk_bounds(o2, o1, T_CARDINAL);
		break;

	case '>':
		o1 = ! chk_bounds(o1, o2, T_CARDINAL);
		break;

	case LESSEQUAL:
		o1 = chk_bounds(o1, o2, T_CARDINAL);
		break;

	case GREATEREQUAL:
		o1 = chk_bounds(o2, o1, T_CARDINAL);
		break;

	case '=':
		o1 = (o1 == o2);
		break;

	case '#':
		o1 = (o1 != o2);
		break;

	case AND:
	case '&':
		o1 = (o1 && o2);
		break;

	case OR:
		o1 = (o1 || o2);
		break;

	default:
		crash("(cstubin)");
	}

	commonbin(expp);
	exp = *expp;
	exp->nd_INT = o1;
	if (exp->nd_type == bool_type) exp->nd_symb = INTEGER;
	CutSize(exp);
}

cstset(expp)
	t_node **expp;
{
	extern arith *MkSet();
	register t_node *exp = *expp;
	register arith *set1, *set2, *set3;
	register unsigned int setsize;
	register int j;

	assert(exp->nd_RIGHT->nd_class == Set);
	assert(exp->nd_symb == IN || exp->nd_LEFT->nd_class == Set);

	set2 = exp->nd_RIGHT->nd_set;
	setsize = (unsigned) (exp->nd_RIGHT->nd_type->tp_size) / (unsigned) word_size;

	if (exp->nd_symb == IN) {
		/*	The setsize must fit in an unsigned, as it is
			allocated with Malloc, so we can do the arithmetic
			in an unsigned too.
		*/
		unsigned i;

		assert(exp->nd_LEFT->nd_class == Value);

		exp->nd_LEFT->nd_INT -= exp->nd_RIGHT->nd_type->set_low;
		i = exp->nd_LEFT->nd_INT;
		/*	Careful here; use exp->nd_LEFT->nd_INT to see if
			it falls in the range of the set. Do not use i
			for this, as i may be truncated.
		*/
		i = (exp->nd_LEFT->nd_INT >= 0 &&
		     exp->nd_LEFT->nd_INT < setsize * wrd_bits &&
		    (set2[i / wrd_bits] & (1 << (i % wrd_bits))));
		FreeSet(set2);
		exp = getnode(Value);
		exp->nd_symb = INTEGER;
		exp->nd_lineno = (*expp)->nd_lineno;
		exp->nd_INT = i;
		exp->nd_type = bool_type;
		FreeNode(*expp);
		*expp = exp;
		return;
	}

	set1 = exp->nd_LEFT->nd_set;
	*expp = MkLeaf(Set, &(exp->nd_RIGHT->nd_token));
	(*expp)->nd_type = exp->nd_type;
	switch(exp->nd_symb) {
	case '+': /* Set union */
	case '-': /* Set difference */
	case '*': /* Set intersection */
	case '/': /* Symmetric set difference */
		(*expp)->nd_set = set3 = MkSet(exp->nd_type->set_sz);
		for (j = 0; j < setsize; j++) {
			switch(exp->nd_symb) {
			case '+':
				*set3++ = *set1++ | *set2++;
				break;
			case '-':
				*set3++ = *set1++ & ~*set2++;
				break;
			case '*':
				*set3++ = *set1++ & *set2++;
				break;
			case '/':
				*set3++ = *set1++ ^ *set2++;
				break;
			}
		}
		break;

	case GREATEREQUAL:
	case LESSEQUAL:
	case '=':
	case '#':
		/* Constant set comparisons
		*/
		for (j = 0; j < setsize; j++) {
			switch(exp->nd_symb) {
			case GREATEREQUAL:
				if ((*set1 | *set2++) != *set1) break;
				set1++;
				continue;
			case LESSEQUAL:
				if ((*set2 | *set1++) != *set2) break;
				set2++;
				continue;
			case '=':
			case '#':
				if (*set1++ != *set2++) break;
				continue;
			}
			break;
		}
		if (j < setsize) {
			j = exp->nd_symb == '#';
		}
		else {
			j = exp->nd_symb != '#';
		}
		*expp = getnode(Value);
		(*expp)->nd_symb = INTEGER;
		(*expp)->nd_INT = j;
		(*expp)->nd_type = bool_type;
		(*expp)->nd_lineno = (*expp)->nd_lineno;
		break;
	default:
		crash("(cstset)");
	}
	FreeSet(exp->nd_LEFT->nd_set);
	FreeSet(exp->nd_RIGHT->nd_set);
	FreeNode(exp);
}

cstcall(expp, call)
	t_node **expp;
{
	/*	a standard procedure call is found that can be evaluated
		compile time, so do so.
	*/
	register t_node *expr;
	register t_type *tp;

	assert((*expp)->nd_class == Call);
	expr = (*expp)->nd_RIGHT->nd_LEFT;
	tp = expr->nd_type;
	expr->nd_type = (*expp)->nd_type;

	(*expp)->nd_RIGHT->nd_LEFT = 0;
	FreeNode(*expp);
	*expp = expr;
	expr->nd_symb = INTEGER;
	expr->nd_class = Value;
	switch(call) {
	case S_ABS:
		if (expr->nd_INT < 0) {
			if (expr->nd_INT <= min_int[(int)(tp->tp_size)]) {
				overflow(expr);
			}
			expr->nd_INT = - expr->nd_INT;
		}
		CutSize(expr);
		break;

	case S_CAP:
		if (expr->nd_INT >= 'a' && expr->nd_INT <= 'z') {
			expr->nd_INT += ('A' - 'a');
		}
		break;

	case S_HIGH:
	case S_MAX:
		if (tp->tp_fund == T_INTEGER) {
			expr->nd_INT = max_int[(int)(tp->tp_size)];
		}
		else if (tp == card_type) {
			expr->nd_INT = full_mask[(int)(int_size)];
		}
		else if (tp->tp_fund == T_SUBRANGE) {
			expr->nd_INT = tp->sub_ub;
		}
		else	expr->nd_INT = tp->enm_ncst - 1;
		break;

	case S_MIN:
		if (tp->tp_fund == T_INTEGER) {
			expr->nd_INT = min_int[(int)(tp->tp_size)];
		}
		else if (tp->tp_fund == T_SUBRANGE) {
			expr->nd_INT = tp->sub_lb;
		}
		else	expr->nd_INT = 0;
		break;

	case S_ODD:
		expr->nd_INT &= 1;
		break;

	case S_TSIZE:
	case S_SIZE:
		expr->nd_INT = tp->tp_size;
		break;

	default:
		crash("(cstcall)");
	}
}

CutSize(expr)
	register t_node *expr;
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	register t_type *tp = BaseType(expr->nd_type);

	assert(expr->nd_class == Value);
	if (tp->tp_fund == T_REAL) return;
	if (tp->tp_fund != T_INTEGER) {
		expr->nd_INT &= full_mask[(int)(tp->tp_size)];
	}
	else {
		int nbits = (int) (sizeof(arith) - tp->tp_size) * 8;

		expr->nd_INT = (expr->nd_INT << nbits) >> nbits;
	}
}

InitCst()
{
	register int i = 0;
	register arith bt = (arith)0;

	while (!(bt < 0))	{
		i++;
		bt = (bt << 8) + 0377;
		if (i == MAXSIZE)
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
		max_int[i] = bt & ~(1L << ((8 * i) - 1));
		min_int[i] = - max_int[i];
		if (! options['s']) min_int[i]--;
	}
	if ((int)long_size > sizeof(arith)) {
		fatal("sizeof (arith) insufficient on this machine");
	}

#ifndef NOCROSS
	wrd_bits = 8 * (int) word_size;
#endif
}
