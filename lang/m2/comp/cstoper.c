/* C O N S T A N T   E X P R E S S I O N   H A N D L I N G */

static char *RcsId = "$Header$";

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"def_sizes.h"
#include	"idf.h"
#include	"type.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"

long mach_long_sign;	/* sign bit of the machine long */
int mach_long_size;	/* size of long on this machine == sizeof(long) */
long full_mask[MAXSIZE];/* full_mask[1] == 0xFF, full_mask[2] == 0xFFFF, .. */
arith max_int;		/* maximum integer on target machine	*/
arith max_unsigned;	/* maximum unsigned on target machine	*/
arith max_longint;	/* maximum longint on target machine	*/
arith wrd_bits;		/* number of bits in a word */

cstunary(expp)
	register struct node *expp;
{
	/*	The unary operation in "expp" is performed on the constant
		expression below it, and the result restored in expp.
	*/
	arith o1 = expp->nd_right->nd_INT;

	switch(expp->nd_symb) {
	case '+':
		break;
	case '-':
		o1 = -o1;
		break;
	case NOT:
		o1 = !o1;
		break;
	default:
		assert(0);
	}
	expp->nd_class = Value;
	expp->nd_token = expp->nd_right->nd_token;
	expp->nd_INT = o1;
	cut_size(expp);
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
	arith o1 = expp->nd_left->nd_INT;
	arith o2 = expp->nd_right->nd_INT;
	int uns = expp->nd_type != int_type;

	assert(expp->nd_class == Oper);
	if (expp->nd_right->nd_type->tp_fund == SET) {
		cstset(expp);
		return;
	}
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
		break;
	case '<':
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 < o2 : 0) :
				(o2 & mach_long_sign ? 1 : o1 < o2)
			);
		}
		else
			o1 = o1 < o2;
		break;
	case '>':
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 > o2 : 1) :
				(o2 & mach_long_sign ? 0 : o1 > o2)
			);
		}
		else
			o1 = o1 > o2;
		break;
	case LESSEQUAL:
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 <= o2 : 0) :
				(o2 & mach_long_sign ? 1 : o1 <= o2)
			);
		}
		else
			o1 = o1 <= o2;
		break;
	case GREATEREQUAL:
		if (uns)	{
			o1 = (o1 & mach_long_sign ?
				(o2 & mach_long_sign ? o1 >= o2 : 1) :
				(o2 & mach_long_sign ? 0 : o1 >= o2)
			);
		}
		else
			o1 = o1 >= o2;
		break;
	case '=':
		o1 = o1 == o2;
		break;
	case '#':
		o1 = o1 != o2;
		break;
	case AND:
		o1 = o1 && o2;
		break;
	case OR:
		o1 = o1 || o2;
		break;
	default:
		assert(0);
	}
	expp->nd_class = Value;
	expp->nd_token = expp->nd_right->nd_token;
	expp->nd_INT = o1;
	cut_size(expp);
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

cstset(expp)
	register struct node *expp;
{
	register arith *set1 = 0, *set2;
	register int setsize, j;

	assert(expp->nd_right->nd_class == Set);
	assert(expp->nd_symb == IN || expp->nd_left->nd_class == Set);
	set2 = expp->nd_right->nd_set;
	setsize = expp->nd_right->nd_type->tp_size / wrd_size;

	if (expp->nd_symb == IN) {
		arith i;

		assert(expp->nd_left->nd_class == Value);
		i = expp->nd_left->nd_INT;
		expp->nd_INT = (i >= 0 &&
		    i < setsize * wrd_bits &&
		    (set2[i / wrd_bits] & (1 << (i % wrd_bits))));
		    free((char *) set2);
	}
	else {
		set1 = expp->nd_left->nd_set;
		switch(expp->nd_symb) {
		case '+':
			for (j = 0; j < setsize; j++) {
				*set1++ |= *set2++;
			}
			break;
		case '-':
			for (j = 0; j < setsize; j++) {
				*set1++ &= ~*set2++;
			}
			break;
		case '*':
			for (j = 0; j < setsize; j++) {
				*set1++ &= *set2++;
			}
			break;
		case '/':
			for (j = 0; j < setsize; j++) {
				*set1++ ^= *set2++;
			}
			break;
		case GREATEREQUAL:
		case LESSEQUAL:
		case '=':
		case '#':
			/* Clumsy, but who cares? Nobody writes these things! */
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
				expp->nd_INT = expp->nd_symb == '#';
				break;
			}
			if (j == setsize) expp->nd_INT = expp->nd_symb != '#';
			expp->nd_class = Value;
			free((char *) expp->nd_left->nd_set);
			free((char *) expp->nd_right->nd_set);
			break;
		default:
			assert(0);
		}
		free((char *) expp->nd_right->nd_set);
		expp->nd_class = Set;
		expp->nd_set = expp->nd_left->nd_set;
	}
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = 0;
}

cut_size(expr)
	register struct node *expr;
{
	/*	The constant value of the expression expr is made to
		conform to the size of the type of the expression.
	*/
	arith o1 = expr->nd_INT;
	int uns = expr->nd_type == card_type || expr->nd_type == intorcard_type;
	int size = expr->nd_type->tp_size;

	assert(expr->nd_class == Value);
	if (uns) {
		if (o1 & ~full_mask[size]) {
			node_warning(expr,
				"overflow in constant expression");
		}
		o1 &= full_mask[size];
	}
	else {
		int nbits = (int) (mach_long_size - size) * 8;
		long remainder = o1 & ~full_mask[size];

		if (remainder != 0 && remainder != ~full_mask[size]) {
			node_warning(expr, "overflow in constant expression");
		}
		o1 <<= nbits;
		o1 >>= nbits;
	}
	expr->nd_INT = o1;
}

init_cst()
{
	int i = 0;
	arith bt = (arith)0;

	while (!(bt < 0))	{
		bt = (bt << 8) + 0377, i++;
		if (i == MAXSIZE)
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
	}
	mach_long_size = i;
	mach_long_sign = 1 << (mach_long_size * 8 - 1);
	if (int_size > mach_long_size) {
		fatal("sizeof (long) insufficient on this machine");
	}

	max_int = full_mask[int_size] & ~(1 << (int_size * 8 - 1));
	max_unsigned = full_mask[int_size];
	wrd_bits = 8 * wrd_size;
}
