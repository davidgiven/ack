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
arith max_int;		/* maximum integer on target machine	*/
arith max_unsigned;	/* maximum unsigned on target machine	*/
arith max_longint;	/* maximum longint on target machine	*/
arith wrd_bits;		/* number of bits in a word */

static char ovflow[] = "overflow in constant expression";

cstunary(expp)
	register struct node *expp;
{
	/*	The unary operation in "expp" is performed on the constant
		expression below it, and the result restored in expp.
	*/
	register arith o1 = expp->nd_right->nd_INT;

	switch(expp->nd_symb) {
	/* Should not get here
	case '+':
		break;
	*/

	case '-':
		o1 = -o1;
		if (expp->nd_type->tp_fund == T_INTORCARD) {
			expp->nd_type = int_type;
		}
		break;

	case NOT:
	case '~':
		o1 = !o1;
		break;

	default:
		crash("(cstunary)");
	}

	expp->nd_class = Value;
	expp->nd_token = expp->nd_right->nd_token;
	expp->nd_INT = o1;
	CutSize(expp);
	FreeNode(expp->nd_right);
	expp->nd_right = 0;
}

cstbin(expp)
	register struct node *expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in
		expp.
	*/
	register arith o1 = expp->nd_left->nd_INT;
	register arith o2 = expp->nd_right->nd_INT;
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
		if (uns)	{
			/*	this is more of a problem than you might
				think on C compilers which do not have
				unsigned long.
			*/
			if (o2 & mach_long_sign)	{/* o2 > max_long */
				o1 = ! (o1 >= 0 || o1 < o2);
				/*	this is the unsigned test
					o1 < o2 for o2 > max_long
				*/
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
				o1 = 2 * hdiv + (rem < 0 || rem >= o2);
				/*	that is the unsigned compare
					rem >= o2 for o2 <= max_long
				*/
			}
		}
		else
			o1 /= o2;
		break;

	case MOD:
		if (o2 == 0)	{
			node_error(expp, "modulo by 0");
			return;
		}
		if (uns)	{
			if (o2 & mach_long_sign)	{/* o2 > max_long */
				o1 = (o1 >= 0 || o1 < o2) ? o1 : o1 - o2;
				/*	this is the unsigned test
					o1 < o2 for o2 > max_long
				*/
			}
			else	{		/* o2 <= max_long */
				long half, bit, hrem, rem;

				half = (o1 >> 1) & ~mach_long_sign;
				bit = o1 & 01;
				/*	now o1 == 2 * half + bit
					and half <= max_long
					and bit <= max_long
				*/
				hrem = half % o2;
				rem = 2 * hrem + bit;
				o1 = (rem < 0 || rem >= o2) ? rem - o2 : rem;
			}
		}
		else
			o1 %= o2;
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
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 >= o2 : 1) :
				(o2 & mach_long_sign ? 0 : o1 >= o2)
			);
		}
		else
			o1 = (o1 >= o2);
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
	register struct node *expp;
{
	register arith *set1, *set2;
	arith *resultset = 0;
	register int setsize, j;

	assert(expp->nd_right->nd_class == Set);
	assert(expp->nd_symb == IN || expp->nd_left->nd_class == Set);
	set2 = expp->nd_right->nd_set;
	setsize = expp->nd_right->nd_type->tp_size / word_size;

	if (expp->nd_symb == IN) {
		arith i;

		assert(expp->nd_left->nd_class == Value);

		i = expp->nd_left->nd_INT;
		expp->nd_class = Value;
		expp->nd_INT = (i >= 0 && i < setsize * wrd_bits &&
		    (set2[i / wrd_bits] & (1 << (i % wrd_bits))));
		free((char *) set2);
	}
	else {
		set1 = expp->nd_left->nd_set;
		resultset = set1;
		expp->nd_left->nd_set = 0;
		switch(expp->nd_symb) {
		case '+':
			/* Set union
			*/
			for (j = 0; j < setsize; j++) {
				*set1++ |= *set2++;
			}
			break;

		case '-':
			/* Set difference
			*/
			for (j = 0; j < setsize; j++) {
				*set1++ &= ~*set2++;
			}
			break;

		case '*':
			/* Set intersection
			*/
			for (j = 0; j < setsize; j++) {
				*set1++ &= *set2++;
			}
			break;

		case '/':
			/* Symmetric set difference
			*/
			for (j = 0; j < setsize; j++) {
				*set1++ ^= *set2++;
			}
			break;

		case GREATEREQUAL:
		case LESSEQUAL:
		case '=':
		case '#':
			/* Constant set comparisons
			*/
			expp->nd_left->nd_set = set1;	/* may be disposed of */
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
			freesets(expp);
			return;
		default:
			crash("(cstset)");
		}
		freesets(expp);
		expp->nd_class = Set;
		expp->nd_set = resultset;
		return;
	}
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

freesets(expp)
	register struct node *expp;
{
	if (expp->nd_right->nd_set) {
		free((char *) expp->nd_right->nd_set);
	}
	if (expp->nd_left->nd_set) {
		free((char *) expp->nd_left->nd_set);
	}
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

cstcall(expp, call)
	register struct node *expp;
{
	/*	a standard procedure call is found that can be evaluated
		compile time, so do so.
	*/
	register struct node *expr = 0;

	assert(expp->nd_class == Call);

	if (expp->nd_right) {
		expr = expp->nd_right->nd_left;
		expp->nd_right->nd_left = 0;
		FreeNode(expp->nd_right);
	}

	expp->nd_class = Value;
	expp->nd_symb = INTEGER;
	switch(call) {
	case S_ABS:
		if (expr->nd_INT < 0) expp->nd_INT = - expr->nd_INT;
		else expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	case S_LONG:
	case S_SHORT: {
		struct type *tp = expp->nd_type;

		*expp = *expr;
		expp->nd_type = tp;
		break;
		}
	case S_CAP:
		if (expr->nd_INT >= 'a' && expr->nd_INT <= 'z') {
			expr->nd_INT = expr->nd_INT + ('A' - 'a');
		}
		/* fall through */
	case S_CHR:
		expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	case S_MAX:
		if (expp->nd_type == int_type) {
			expp->nd_INT = max_int;
		}
		else if (expp->nd_type == longint_type) {
			expp->nd_INT = max_longint;
		}
		else if (expp->nd_type == card_type) {
			expp->nd_INT = max_unsigned;
		}
		else if (expp->nd_type->tp_fund == T_SUBRANGE) {
			expp->nd_INT = expp->nd_type->sub_ub;
		}
		else	expp->nd_INT = expp->nd_type->enm_ncst - 1;
		break;

	case S_MIN:
		if (expp->nd_type == int_type) {
			expp->nd_INT = (-max_int) - 1;
		}
		else if (expp->nd_type == longint_type) {
			expp->nd_INT = (-max_longint) - 1;
		}
		else if (expp->nd_type->tp_fund == T_SUBRANGE) {
			expp->nd_INT = expp->nd_type->sub_lb;
		}
		else	expp->nd_INT = 0;
		break;

	case S_ODD:
		expp->nd_INT = (expr->nd_INT & 1);
		break;

	case S_ORD:
		expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	case S_SIZE:
		expp->nd_INT = expr->nd_type->tp_size;
		break;

	case S_VAL:
		expp->nd_INT = expr->nd_INT;
		if ( /* Check overflow of subranges or enumerations */
		    ( expp->nd_type->tp_fund == T_SUBRANGE
		    &&
		      (  expp->nd_INT < expp->nd_type->sub_lb
		      || expp->nd_INT > expp->nd_type->sub_ub
		      )
		    )
		   ||
		    ( expp->nd_type->tp_fund == T_ENUMERATION
		    &&
		      (  expp->nd_INT < 0
		      || expp->nd_INT >= expp->nd_type->enm_ncst
		      )
		    )
		   )	node_warning(expp, W_ORDINARY, ovflow);
		else CutSize(expp);
		break;

	default:
		crash("(cstcall)");
	}
	FreeNode(expr);
	FreeNode(expp->nd_left);
	expp->nd_right = expp->nd_left = 0;
}

CutSize(expr)
	register struct node *expr;
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	register arith o1 = expr->nd_INT;
	register struct type *tp = BaseType(expr->nd_type);
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
		long remainder = o1 & ~full_mask[size];

		if (remainder != 0 && remainder != ~full_mask[size]) {
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
	}
	mach_long_size = i;
	mach_long_sign = 1L << (mach_long_size * 8 - 1);
	if (long_size > mach_long_size) {
		fatal("sizeof (long) insufficient on this machine");
	}

	max_int = full_mask[int_size] & ~(1L << (int_size * 8 - 1));
	max_unsigned = full_mask[int_size];
	max_longint = full_mask[long_size] & ~(1L << (long_size * 8 - 1));
	wrd_bits = 8 * word_size;
}
