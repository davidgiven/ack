/* C O N S T A N T   E X P R E S S I O N   H A N D L I N G */

#include    <stdlib.h>
#include    <string.h>
#include    "parameters.h"
#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"const.h"
#include	"node.h"
#include	"required.h"
#include	"type.h"

long mach_long_sign;	/* sign bit of the machine long */
long full_mask[MAXSIZE+1];/* full_mask[1] == 0xFF, full_mask[2] == 0xFFFF, .. */
arith max_int;		/* maximum integer on the target machine */
arith min_int;		/* mimimum integer on the target machin */
char *maxint_str;	/* string representation of maximum integer */
arith wrd_bits;		/* number of bits in a word */
arith max_intset;	/* largest value of set of integer */

overflow(expp)
	struct node *expp;
{
	node_warning(expp, "overflow in constant expression");
}

cstunary(expp)
	register struct node *expp;
{
	/*	The unary operation in "expp" is performed on the constant
		expression below it, and the result restored in expp.
	*/
	register arith o1 = expp->nd_right->nd_INT;

	switch( expp->nd_symb )	{
		/* Should not get here
		case '+':
		case '(':
			break;
		*/

		case '-':
			o1 = -o1;
			break;

		case NOT:
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
	expp->nd_right = NULLNODE;
}

cstbin(expp)
	register struct node *expp;
{
	/*	The binary operation in "expp" is performed on the constant
		expressions below it, and the result restored in expp.
	*/
	register arith o1, o2;
	register char *s1, *s2;
	int str = expp->nd_left->nd_type->tp_fund & T_STRINGCONST;

	if( str )	{
		o1 = o2 = 0;			/* so LINT won't complain */
		s1 = expp->nd_left->nd_STR;
		s2 = expp->nd_right->nd_STR;
	}
	else	{
		s1 = s2 = (char *) 0;		/* so LINT won't complain */
		o1 = expp->nd_left->nd_INT;
		o2 = expp->nd_right->nd_INT;
	}

	assert(expp->nd_class == Boper);
	assert(expp->nd_left->nd_class == Value);
	assert(expp->nd_right->nd_class == Value);

	switch( expp->nd_symb )	{
		case '+':
			if (o1 > 0 && o2 > 0) {
				if (max_int - o1 < o2) overflow(expp);
			}
			else if (o1 < 0 && o2 < 0) {
				if (min_int - o1 > o2) overflow(expp);
			}
			o1 += o2;
			break;

		case '-':
			if ( o1 >= 0 && o2 < 0) {
				if (max_int + o2 < o1) overflow(expp);
			}
			else if (o1 < 0 && o2 >= 0) {
				if (min_int + o2 > o1) overflow(expp);
			}
			o1 -= o2;
			break;

		case '*':
			if (o1 > 0 && o2 > 0) {
				if (max_int / o1 < o2) overflow(expp);
			}
			else if (o1 < 0 && o2 < 0) {
				if (o1 == min_int || o2 == min_int ||
				    max_int / (-o1) < (-o2)) overflow(expp);
			}
			else if (o1 > 0) {
				if (min_int / o1 > o2) overflow(expp);
			}
			else if (o2 > 0) {
				if (min_int / o2 > o1) overflow(expp);
			}
			o1 *= o2;
			break;

		case DIV:
			if( o2 == 0 )	{
				node_error(expp, "division by 0");
				return;
			}
			else o1 /= o2;
			break;

		case MOD:
			if( o2 == 0 )	{
				node_error(expp, "modulo by 0");
				return;
			}
			else
				o1 %= o2;
			break;

		case OR:
			o1 = (o1 || o2);
			break;

		case AND:
			o1 = (o1 && o2);
			break;

		case '=':
			o1 = str ? !strcmp(s1, s2) : (o1 == o2);
			break;

		case NOTEQUAL:
			o1 = str ? (strcmp(s1, s2) != 0) : (o1 != o2);
			break;

		case LESSEQUAL:
			o1 = str ? (strcmp(s1, s2) <= 0) : (o1 <= o2);
			break;

		case GREATEREQUAL:
			o1 = str ? (strcmp(s1, s2) >= 0) : (o1 >= o2);
			break;

		case '<':
			o1 = str ? (strcmp(s1, s2) < 0) : (o1 < o2);
			break;

		case '>':
			o1 = str ? (strcmp(s1, s2) > 0) : (o1 > o2);
			break;

		/* case '/': */
		default:
			crash("(cstbin)");

	}

	expp->nd_class = Value;
	expp->nd_token = expp->nd_right->nd_token;
	/* STRING compare has a bool_type as result */
	if( expp->nd_type == bool_type ) expp->nd_symb = INTEGER;
	expp->nd_INT = o1;
	CutSize(expp);
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = NULLNODE;
}

cstset(expp)
	register struct node *expp;
{
	register arith *set1, *set2;
	arith *resultset = (arith *) 0;
	int empty_result = 0;
	register int setsize, j;

	assert(expp->nd_right->nd_class == Set);
	assert(expp->nd_symb == IN || expp->nd_left->nd_class == Set);
	set2 = expp->nd_right->nd_set;
	setsize = (unsigned) (expp->nd_right->nd_type->tp_size) / (unsigned) word_size;

	if( expp->nd_symb == IN )	{
		arith i;

		assert(expp->nd_left->nd_class == Value);

		i = expp->nd_left->nd_INT;
		expp->nd_class = Value;
		expp->nd_symb = INTEGER;

		expp->nd_INT = (i >= 0 && set2 && i < (setsize * wrd_bits) &&
				(set2[i/wrd_bits] & (1 << (i%wrd_bits))));

		if( set2 ) free((char *) set2);
	}
	else	{
		set1 = expp->nd_left->nd_set;
		resultset = set1;
		expp->nd_left->nd_set = (arith *) 0;
		switch( expp->nd_symb )	{
		case '+':
			/* Set union
			*/
			if( !set1 )	{
				resultset = set2;
				expp->nd_right->nd_set = (arith *) 0;
				break;
			}
			if( set2 )
				for( j = 0; j < setsize; j++ )
					*set1++ |= *set2++;
			break;

		case '-':
			/* Set difference
			*/
			if( !set1 || !set2 )	{
				/* The set from which something is substracted
				   is already empty, or the set that is
				   substracted is empty. In either case, the
				   result set is set1.
				*/
				break;
			}
			empty_result = 1;
			for( j = 0; j < setsize; j++ )
				if( *set1++ &= ~*set2++ ) empty_result = 0;
			break;

		case '*':
			/* Set intersection
			*/
			if( !set1 )	{
				/* set1 is empty, and so is the result set
				*/
				break;
			}
			if( !set2 )	{
				/* set 2 is empty, so the result set must be
				   empty too.
				*/
				resultset = set2;
				expp->nd_right->nd_set = (arith *) 0;
				break;
			}
			empty_result = 1;
			for( j = 0; j < setsize; j++ )
				if( *set1++ &= *set2++ ) empty_result = 0;
			break;

		case '=':
		case NOTEQUAL:
		case LESSEQUAL:
		case GREATEREQUAL:
			/* Constant set comparisons
			*/
			if( !setsize ) setsize++;	/* force comparison */
			expp->nd_left->nd_set = set1;	/* may be disposed of */
			for( j = 0; j < setsize; j++ )	{
				switch( expp->nd_symb )	{
				case '=':
				case NOTEQUAL:
					if( !set1 && !set2 )	{
						j = setsize;
						break;
					}
					if( !set1 || !set2 ) break;
					if( *set1++ != *set2++ ) break;
					continue;
				case LESSEQUAL:
					if( !set1 )	{
						j = setsize;
						break;
					}
					if( !set2 ) break;
					if( (*set2 | *set1++) != *set2 ) break;
					set2++;
					continue;
				case GREATEREQUAL:
					if( !set2 )	{
						j = setsize;
						break;
					}
					if( !set1 ) break;
					if( (*set1 | *set2++) != *set1 ) break;
					set1++;
					continue;
				}
				break;
			}
			if( j < setsize )
				expp->nd_INT = expp->nd_symb == NOTEQUAL;
			else
				expp->nd_INT = expp->nd_symb != NOTEQUAL;
			expp->nd_class = Value;
			expp->nd_symb = INTEGER;
			if( expp->nd_left->nd_set )
				free((char *) expp->nd_left->nd_set);
			if( expp->nd_right->nd_set )
				free((char *) expp->nd_right->nd_set);
			FreeNode(expp->nd_left);
			FreeNode(expp->nd_right);
			expp->nd_left = expp->nd_right = NULLNODE;
			return;
		default:
			crash("(cstset)");
		}
		if( expp->nd_right->nd_set )
			free((char *) expp->nd_right->nd_set);
		if( expp->nd_left->nd_set )
			free((char *) expp->nd_left->nd_set);
		if( empty_result )	{
			free((char *) resultset);
			resultset = (arith *) 0;
		}
		expp->nd_class = Set;
		expp->nd_set = resultset;
	}
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_left = expp->nd_right = NULLNODE;
}

cstcall(expp, req)
	register struct node *expp;
{
	/*	a standard procedure call is found that can be evaluated
		compile time, so do so.
	*/
	register struct node *expr = NULLNODE;

	assert(expp->nd_class == Call);

	expr = expp->nd_right->nd_left;

	expp->nd_class = Value;
	expp->nd_symb = INTEGER;
	switch( req )	{
	    case R_ABS:
		if( expr->nd_INT < 0 ) {
			if (expr->nd_INT <= min_int) {
				overflow(expr);
			}
			expp->nd_INT = - expr->nd_INT;
		}
		else expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	    case R_SQR:
		if (expr->nd_INT < 0) {
			if ( expr->nd_INT == min_int ||
			    max_int / expr->nd_INT > expr->nd_INT) {
				overflow(expr);
			}
		}
		else if (max_int / expr->nd_INT < expr->nd_INT) {
			overflow(expr);
		}
		expp->nd_INT = expr->nd_INT * expr->nd_INT;
		CutSize(expp);
		break;

	    case R_ORD:
	    case R_CHR:
		expp->nd_INT = expr->nd_INT;
		CutSize(expp);
		break;

	    case R_ODD:
		expp->nd_INT = (expr->nd_INT & 1);
		break;

	    case R_SUCC:
		expp->nd_INT = expr->nd_INT + 1;
		if(	/* Check overflow of subranges or enumerations */
			(expp->nd_type->tp_fund & T_SUBRANGE &&
				expp->nd_INT > expp->nd_type->sub_ub
			)
		   ||
			( expp->nd_type->tp_fund & T_ENUMERATION &&
				expp->nd_INT >= expp->nd_type->enm_ncst
			)
		  )
			node_warning(expp, "\"succ\": no successor");
		else CutSize(expp);
		break;

	    case R_PRED:
		expp->nd_INT = expr->nd_INT - 1;
		if(	/* Check with lowerbound of subranges or enumerations */
			(expp->nd_type->tp_fund & T_SUBRANGE &&
				expp->nd_INT < expp->nd_type->sub_lb
			)
		   ||
			( expp->nd_type->tp_fund & T_ENUMERATION &&
				expp->nd_INT < 0
			)
		  )
			node_warning(expp, "\"pred\": no predecessor");
		else CutSize(expp);
		break;

	    default:
		crash("(cstcall)");
	}
	FreeNode(expp->nd_left);
	FreeNode(expp->nd_right);
	expp->nd_right = expp->nd_left = NULLNODE;
}

CutSize(expr)
	register struct node *expr;
{
	/* The constant value of the expression expr is made to conform
	 * to the size of the type of the expression
	 */
	register arith o1 = expr->nd_INT;
	register struct type *tp = BaseType(expr->nd_type);
	int size = tp->tp_size;
	long remainder = o1 & ~full_mask[size];

	assert(expr->nd_class == Value);

	if( tp->tp_fund & T_CHAR )	{
		if( o1 & (~full_mask[size] >> 1) ) 	{
			node_warning(expr, "overflow in character value");
			o1 &= 0177;
		}
	}
	else if( remainder != 0 && remainder != ~full_mask[size] ||
	    		(o1 & full_mask[size]) == 1 << (size * 8 - 1) )	{
		/* integers in [-maxint .. maxint] */
		int nbits = (int) (sizeof(long) - size) * 8;

		/* overflow(expr); */
		/* sign bit of o1 in sign bit of mach_long */
		o1 <<= nbits;
		/* shift back to get sign extension */
		o1 >>= nbits;
	}
	expr->nd_INT = o1;
}

InitCst()
{
	extern char *long2str(), *Salloc();
	register int i = 0;
	register arith bt = (arith)0;

	while( !(bt < 0) )	{
		bt = (bt << 8) + 0377;
		i++;
		if( i == MAXSIZE + 1 )
			fatal("array full_mask too small for this machine");
		full_mask[i] = bt;
	}
	mach_long_sign = 1L << (sizeof(long) * 8 - 1);
	if( int_size > sizeof(long) )
		fatal("sizeof (long) insufficient on this machine");

	max_int = full_mask[int_size] & ~(1L << (int_size * 8 - 1));
	min_int = - max_int;
	maxint_str = long2str(max_int, 10);
	maxint_str = Salloc(maxint_str, (unsigned int) strlen(maxint_str));
	wrd_bits = 8 * (int) word_size;
	if( !max_intset ) max_intset = wrd_bits - 1;
}
