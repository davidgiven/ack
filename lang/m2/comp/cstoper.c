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

#include	"idf.h"
#include	"type.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"
#include	"standards.h"
#include	"warning.h"

long mach_long_sign;	/* sign bit of the machine long */
int mach_long_size;	/* size of long on this machine == sizeof(long) */
long full_mask[MAXSIZE];/* full_mask[1] == 0xFF, full_mask[2] == 0xFFFF, .. */
long int_mask[MAXSIZE];	/* int_mask[1] == 0x7F, int_mask[2] == 0x7FFF, .. */
arith max_int;		/* maximum integer on target machine	*/
unsigned int wrd_bits;	/* number of bits in a word */

extern char options[];

static char ovflow[] = "overflow in constant expression";

cstunary(expp)
	register t_node *expp;
{
	/*	The unary operation in "expp" is performed on the constant
		expression below it, and the result restored in expp.
	*/
	register t_node *right = expp->nd_right;

	switch(expp->nd_symb) {
	/* Should not get here
	case '+':
		break;
	*/

	case '-':
		if (right->nd_INT < -int_mask[(int)(right->nd_type->tp_size)])
			node_warning(expp, W_ORDINARY, ovflow);
		
		expp->nd_INT = -right->nd_INT;
		break;

	case NOT:
	case '~':
		expp->nd_INT = !right->nd_INT;
		break;

	default:
		crash("(cstunary)");
	}

	expp->nd_class = Value;
	expp->nd_symb = right->nd_symb;
	CutSize(expp);
	FreeNode(right);
	expp->nd_right = 0;
}

STATIC
divide(pdiv, prem, uns)
	arith *pdiv, *prem;
{
	/*	Divide *pdiv by *prem, and store result in *pdiv,
		remainder in *prem
	*/
	register arith o1 = *pdiv;
	register arith o2 = *prem;

	if (uns)	{
		/*	this is more of a problem than you might
			think on C compilers which do not have
			unsigned long.
		*/
		if (o2 & mach_long_sign)	{/* o2 > max_long */
			if (! (o1 >= 0 || o1 < o2)) {
				/*	this is the unsigned test
					o1 < o2 for o2 > max_long
				*/
				*prem = o2 - o1;
				*pdiv = 1;
			}
			else {
				*pdiv = 0;
			}
		}
		else	{		/* o2 <= max_long */
			long half, bit, hdiv, hrem, rem;

			half = (o1 >> 1) & ~mach_long_sign;
			bit = o1 & 01;
			/*	now o1 == 2 * half + bit
				and half <= max_long
				and bit <= max_long
			*/
			hdiv = half / o2;
			hrem = half % o2;
			rem = 2 * hrem + bit;
			*pdiv = 2*hdiv;
			*prem = rem;
			if (rem < 0 || rem >= o2) {
				/*	that is the unsigned compare
					rem >= o2 for o2 <= max_long
				*/
				*pdiv += 1;
				*prem -= o2;
			}
		}
	}
	else {
		*pdiv = o1 / o2;		/* ??? */
		*prem = o1 - *pdiv * o2;
	}
}

cstbin(expp)
	register t_node *expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in
		expp.
	*/
	arith o1 = expp->nd_left->nd_INT;
	arith o2 = expp->nd_right->nd_INT;
	register int uns = expp->nd_left->nd_type != int_type;

	assert(expp->nd_class == Oper);
	assert(expp->nd_left->nd_class == Value);
	assert(expp->nd_right->nd_class == Value);

	switch (expp->nd_symb)	{
	case '*':
		o1 *= o2;
		break;

	case DIV:
		if (o2 == 0)	{
			node_error(expp, "division by 0");
			return;
		}
		divide(&o1, &o2, uns);
		break;

	case MOD:
		if (o2 == 0)	{
			node_error(expp, "modulo by 0");
			return;
		}
		divide(&o1, &o2, uns);
		o1 = o2;
		break;

	case '+':
		o1 += o2;
		break;

	case '-':
		o1 -= o2;
		if (expp->nd_type->tp_fund == T_INTORCARD) {
			if (o1 < 0) expp->nd_type = int_type;
		}
		break;

	case '<':
		{	arith tmp = o1;
			
			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */

	case '>':
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 > o2 : 1) :
				(o2 & mach_long_sign ? 0 : o1 > o2)
			);
		}
		else
			o1 = (o1 > o2);
		break;

	case LESSEQUAL:
		{	arith tmp = o1;
			
			o1 = o2;
			o2 = tmp;
		}
		/* Fall through */

	case GREATEREQUAL:
		o1 = chk_bounds(o2, o1, uns ? T_CARDINAL : T_INTEGER);
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
		crash("(cstbin)");
	}

	expp->nd_class = Value;
	expp->nd_token = expp->nd_right->nd_token;
	if (expp->nd_type == bool_type) expp->nd_symb = INTEGER;
	expp->nd_INT = o1;
	CutSize(expp);
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

cstset(expp)
	register t_node *expp;
{
	extern arith *MkSet();
	register arith *set1, *set2;
	register arith *resultset;
	register unsigned int setsize;
	register int j;

	assert(expp->nd_right->nd_class == Set);
	assert(expp->nd_symb == IN || expp->nd_left->nd_class == Set);

	set2 = expp->nd_right->nd_set;
	setsize = (unsigned) expp->nd_right->nd_type->tp_size / (unsigned) word_size;

	if (expp->nd_symb == IN) {
		unsigned i;

		assert(expp->nd_left->nd_class == Value);

		i = expp->nd_left->nd_INT;
		expp->nd_class = Value;
		expp->nd_INT = (expp->nd_left->nd_INT >= 0 &&
				expp->nd_left->nd_INT < setsize * wrd_bits &&
		    (set2[i / wrd_bits] & (1 << (i % wrd_bits))));
		FreeSet(set2);
		expp->nd_symb = INTEGER;
		FreeNode(expp->nd_left);
		FreeNode(expp->nd_right);
		expp->nd_left = expp->nd_right = 0;
		return;
	}

	set1 = expp->nd_left->nd_set;
	switch(expp->nd_symb) {
	case '+': /* Set union */
	case '-': /* Set difference */
	case '*': /* Set intersection */
	case '/': /* Symmetric set difference */
		expp->nd_set = resultset = MkSet(setsize * (unsigned) word_size);
		for (j = 0; j < setsize; j++) {
			switch(expp->nd_symb) {
			case '+':
				*resultset = *set1++ | *set2++;
				break;
			case '-':
				*resultset = *set1++ & ~*set2++;
				break;
			case '*':
				*resultset = *set1++ & *set2++;
				break;
			case '/':
				*resultset = *set1++ ^ *set2++;
				break;
			}
			resultset++;
		}
		expp->nd_class = Set;
		break;

	case GREATEREQUAL:
	case LESSEQUAL:
	case '=':
	case '#':
		/* Constant set comparisons
		*/
		for (j = 0; j < setsize; j++) {
			switch(expp->nd_symb) {
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
			expp->nd_INT = expp->nd_symb == '#';
		}
		else {
			expp->nd_INT = expp->nd_symb != '#';
		}
		expp->nd_class = Value;
		expp->nd_symb = INTEGER;
		break;
	default:
		crash("(cstset)");
	}
	FreeSet(expp->nd_left->nd_set);
	FreeSet(expp->nd_right->nd_set);
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

cstcall(expp, call)
	register t_node *expp;
{
	/*	a standard procedure call is found that can be evaluated
		compile time, so do so.
	*/
	register t_node *expr;
	register t_type *tp;

	assert(expp->nd_class == Call);

	expr = expp->nd_right->nd_left;
	expp->nd_right->nd_left = 0;
	FreeNode(expp->nd_right);
	tp = expr->nd_type;

	expp->nd_class = Value;
	expp->nd_symb = INTEGER;
	switch(call) {
	case S_ABS:
		if (expr->nd_INT < 0) expp->nd_INT = - expr->nd_INT;
		else expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	case S_CAP:
		if (expr->nd_INT >= 'a' && expr->nd_INT <= 'z') {
			expr->nd_INT = expr->nd_INT + ('A' - 'a');
		}
		expp->nd_INT = expr->nd_INT;
		break;

	case S_MAX:
		if (tp->tp_fund == T_INTEGER) {
			expp->nd_INT = int_mask[(int)(tp->tp_size)];
		}
		else if (tp == card_type) {
			expp->nd_INT = full_mask[(int)(int_size)];
		}
		else if (tp->tp_fund == T_SUBRANGE) {
			expp->nd_INT = tp->sub_ub;
		}
		else	expp->nd_INT = tp->enm_ncst - 1;
		break;

	case S_MIN:
		if (tp->tp_fund == T_INTEGER) {
			expp->nd_INT = -int_mask[(int)(tp->tp_size)];
			if (! options['s']) expp->nd_INT--;
		}
		else if (tp->tp_fund == T_SUBRANGE) {
			expp->nd_INT = tp->sub_lb;
		}
		else	expp->nd_INT = 0;
		break;

	case S_ODD:
		expp->nd_INT = (expr->nd_INT & 1);
		break;

	case S_SIZE:
		expp->nd_INT = tp->tp_size;
		break;

	default:
		crash("(cstcall)");
	}
	FreeNode(expr);
	FreeNode(expp->nd_left);
	expp->nd_right = expp->nd_left = 0;
}

CutSize(expr)
	register t_node *expr;
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	register arith o1 = expr->nd_INT;
	register t_type *tp = BaseType(expr->nd_type);
	int uns;
	int size = tp->tp_size;

	assert(expr->nd_class == Value);
	uns = (tp->tp_fund & (T_CARDINAL|T_CHAR));
	if (uns) {
		if (o1 & ~full_mask[size]) {
			node_warning(expr, W_ORDINARY, ovflow);
			o1 &= full_mask[size];
		}
	}
	else {
		int nbits = (int) (mach_long_size - size) * 8;
		long remainder = o1 & ~int_mask[size];

		if (remainder != 0 && remainder != ~int_mask[size]) {
			node_warning(expr, W_ORDINARY, ovflow);
			o1 <<= nbits;
			o1 >>= nbits;
		}
	}
	expr->nd_INT = o1;
}

InitCst()
{
	register int i = 0;
	register arith bt = (arith)0;

	while (!(bt < 0))	{
		bt = (bt << 8) + 0377, i++;
		if (i == MAXSIZE)
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
		int_mask[i] = bt & ~(1L << ((i << 3) - 1));
	}
	mach_long_size = i;
	mach_long_sign = 1L << (mach_long_size * 8 - 1);
	if (long_size > mach_long_size) {
		fatal("sizeof (long) insufficient on this machine");
	}

	max_int = int_mask[(int)int_size];
	wrd_bits = 8 * (unsigned) word_size;
}
