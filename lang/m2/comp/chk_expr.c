/* E X P R E S S I O N   C H E C K I N G */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

/*	Check expressions, and try to evaluate them as far as possible.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	<alloc.h>

#include	"Lpars.h"
#include	"idf.h"
#include	"type.h"
#include	"def.h"
#include	"LLlex.h"
#include	"node.h"
#include	"scope.h"
#include	"const.h"
#include	"standards.h"

extern char *symbol2str();

int
chk_expr(expp)
	register struct node *expp;
{
	/*	Check the expression indicated by expp for semantic errors,
		identify identifiers used in it, replace constants by
		their value, and try to evaluate the expression.
	*/

	switch(expp->nd_class) {
	case Oper:
		if (expp->nd_symb == '[') {
			return chk_designator(expp, DESIGNATOR|VARIABLE);
		}

		return	chk_expr(expp->nd_left) &&
			chk_expr(expp->nd_right) &&
			chk_oper(expp);

	case Uoper:
		if (expp->nd_symb == '^') {
			return chk_designator(expp, DESIGNATOR|VARIABLE);
		}

		return	chk_expr(expp->nd_right) &&
			chk_uoper(expp);

	case Value:
		switch(expp->nd_symb) {
		case REAL:
		case STRING:
		case INTEGER:
			return 1;

		default:
			assert(0);
		}
		break;

	case Xset:
		return chk_set(expp);

	case Name:
		return chk_designator(expp, VALUE);

	case Call:
		return chk_call(expp);

	case Link:
		return chk_designator(expp, DESIGNATOR|VALUE);

	default:
		assert(0);
	}
	/*NOTREACHED*/
}

int
chk_set(expp)
	register struct node *expp;
{
	/*	Check the legality of a SET aggregate, and try to evaluate it
		compile time. Unfortunately this is all rather complicated.
	*/
	struct type *tp;
	struct def *df;
	register struct node *nd;
	arith *set;

	assert(expp->nd_symb == SET);

	/* First determine the type of the set
	*/
	if (nd = expp->nd_left) {
		/* A type was given. Check it out
		*/
		if (! chk_designator(nd, 0)) return 0;

		assert(nd->nd_class == Def);
		df = nd->nd_def;

		if (!(df->df_kind & (D_TYPE|D_ERROR)) ||
		    (df->df_type->tp_fund != T_SET)) {
			node_error(expp, "specifier does not represent a set type");
			return 0;
		}
		tp = df->df_type;
		FreeNode(expp->nd_left);
		expp->nd_left = 0;
	}
	else	tp = bitset_type;

	/* Now check the elements given, and try to compute a constant set.
	   First allocate room for the set
	*/
	set = (arith *)
		Malloc((unsigned) (tp->tp_size * sizeof(arith) / word_size));

	/* Now check the elements, one by one
	*/
	nd = expp->nd_right;
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		if (!chk_el(nd->nd_left, tp->next, &set)) return 0;
		nd = nd->nd_right;
	}

	expp->nd_type = tp;

	if (set) {
		/* Yes, it was a constant set, and we managed to compute it!
		   Notice that at the moment there is no such thing as
		   partial evaluation. Either we evaluate the set, or we
		   don't (at all). Improvement not neccesary. (???)
		*/
		expp->nd_class = Set;
		expp->nd_set = set;
		FreeNode(expp->nd_right);
		expp->nd_right = 0;
	}

	return 1;
}

int
chk_el(expp, tp, set)
	register struct node *expp;
	struct type *tp;
	arith **set;
{
	/*	Check elements of a set. This routine may call itself
		recursively.
		Also try to compute the set!
	*/
	register int i;
	register struct node *left = expp->nd_left;
	register struct node *right = expp->nd_right;

	if (expp->nd_class == Link && expp->nd_symb == UPTO) {
		/* { ... , expr1 .. expr2,  ... }
		   First check expr1 and expr2, and try to compute them.
		*/
		if (!chk_el(left, tp, set) || !chk_el(right, tp, set)) {
			return 0;
		}

		if (left->nd_class == Value && right->nd_class == Value) {
			/* We have a constant range. Put all elements in the
			   set
			*/

		    	if (left->nd_INT > right->nd_INT) {
node_error(expp, "lower bound exceeds upper bound in range");
				return rem_set(set);
			}

			if (*set) {
				for (i=left->nd_INT+1; i<right->nd_INT; i++) {
					(*set)[i/wrd_bits] |= (1<<(i%wrd_bits));
				}
			}
		}
		else if (*set) {
			free((char *) *set);
			*set = 0;
		}

		return 1;
	}

	/* Here, a single element is checked
	*/
	if (!chk_expr(expp)) {
		return rem_set(set);
	}

	if (!TstCompat(tp, expp->nd_type)) {
		node_error(expp, "set element has incompatible type");
		return rem_set(set);
	}

	if (expp->nd_class == Value) {
		/* a constant element
		*/
		i = expp->nd_INT;

	    	if ((tp->tp_fund != T_ENUMERATION &&
		     (i < tp->sub_lb || i > tp->sub_ub))
		   ||
		    (tp->tp_fund == T_ENUMERATION &&
		     (i < 0 || i > tp->enm_ncst))
		   ) {
			node_error(expp, "set element out of range");
			return rem_set(set);
		}

		if (*set) (*set)[i/wrd_bits] |= (1 << (i%wrd_bits));
	}

	return 1;
}

int
rem_set(set)
	arith **set;
{
	/*	This routine is only used for error exits of chk_el.
		It frees the set indicated by "set", and returns 0.
	*/
	if (*set) {
		free((char *) *set);
		*set = 0;
	}
	return 0;
}

struct node *
getarg(argp, bases, designator)
	struct node *argp;
{
	struct type *tp;

	if (!argp->nd_right) {
		node_error(argp, "too few arguments supplied");
		return 0;
	}
	argp = argp->nd_right;
	if ((!designator && !chk_expr(argp->nd_left)) ||
	    (designator && !chk_designator(argp->nd_left, DESIGNATOR))) {
		return 0;
	}
	tp = argp->nd_left->nd_type;
	if (tp->tp_fund == T_SUBRANGE) tp = tp->next;
	if (bases && !(tp->tp_fund & bases)) {
		node_error(argp, "unexpected type");
		return 0;
	}
	return argp;
}

struct node *
getname(argp, kinds)
	struct node *argp;
{
	if (!argp->nd_right) {
		node_error(argp, "too few arguments supplied");
		return 0;
	}
	argp = argp->nd_right;
	if (! chk_designator(argp->nd_left, 0)) return 0;

	assert(argp->nd_left->nd_class == Def);

	if (!(argp->nd_left->nd_def->df_kind & kinds)) {
		node_error(argp, "unexpected type");
		return 0;
	}

	return argp;
}

int
chk_call(expp)
	register struct node *expp;
{
	/*	Check something that looks like a procedure or function call.
		Of course this does not have to be a call at all.
		it may also be a cast or a standard procedure call.
	*/
	register struct node *left;
	register struct node *arg;

	/* First, get the name of the function or procedure
	*/
	expp->nd_type = error_type;
	left = expp->nd_left;
	if (! chk_designator(left, 0)) return 0;

	if (left->nd_class == Def &&
	    (left->nd_def->df_kind & (D_HTYPE|D_TYPE|D_HIDDEN))) {
		/* It was a type cast. This is of course not portable.
		*/
		arg = expp->nd_right;
		if ((! arg) || arg->nd_right) {
node_error(expp, "only one parameter expected in type cast");
			return 0;
		}
		arg = arg->nd_left;
		if (! chk_expr(arg)) return 0;
		if (arg->nd_type->tp_size != left->nd_type->tp_size) {
node_error(expp, "unequal sizes in type cast");
		}
		if (arg->nd_class == Value) {
			struct type *tp = left->nd_type;

			FreeNode(expp->nd_left);
			expp->nd_right->nd_left = 0;
			FreeNode(expp->nd_right);
			expp->nd_left = expp->nd_right = 0;
			*expp = *arg;
			expp->nd_type = tp;
		}
		else expp->nd_type = left->nd_type;

		return 1;
	}

	if ((left->nd_class == Def && left->nd_def->df_kind == D_PROCEDURE) ||
	    left->nd_type->tp_fund == T_PROCEDURE) {
		/* A procedure call. it may also be a call to a
		   standard procedure
		*/
		arg = expp;
		if (left->nd_type == std_type) {
			/* A standard procedure
			*/
			return chk_std(expp, left, arg);
		}
		/* Here, we have found a real procedure call. The left hand
		   side may also represent a procedure variable.
		*/
		return chk_proccall(expp);
	}
	node_error(expp->nd_left, "procedure, type, or function expected");
	return 0;
}

chk_proccall(expp)
	struct node *expp;
{
	/*	Check a procedure call
	*/
	register struct node *left = expp->nd_left;
	register struct node *arg;
	register struct paramlist *param;

	arg = expp;
	arg->nd_type = left->nd_type->next;
	param = left->nd_type->prc_params;

	while (param) {
		if (!(arg = getarg(arg, 0, param->par_var))) return 0;

		if (! TstParCompat(param->par_type,
				   arg->nd_left->nd_type,
				   param->par_var)) {
node_error(arg->nd_left, "type incompatibility in parameter");
			return 0;
		}

		param = param->next;
	}

	if (arg->nd_right) {
		node_error(arg->nd_right, "too many parameters supplied");
		return 0;
	}

	return 1;
}

static int
FlagCheck(expp, df, flag)
	struct node *expp;
	struct def *df;
{
	/*	See the routine "chk_designator" for an explanation of
		"flag". Here, a definition "df" is checked against it.
	*/

	if (df->df_kind == D_ERROR) return 0;

	if ((flag & VARIABLE) &&
	    !(df->df_kind & (D_FIELD|D_VARIABLE))) {
		node_error(expp, "variable expected");
		return 0;
	}

	if ((flag & HASSELECTORS) &&
	    ( !(df->df_kind & (D_VARIABLE|D_FIELD|D_MODULE)) ||
	      df->df_type->tp_fund != T_RECORD)) {
		node_error(expp, "illegal selection");
		return 0;
	}

	if ((flag & VALUE) &&
	    ( !(df->df_kind & (D_VARIABLE|D_FIELD|D_CONST|D_ENUM)))) {
		node_error(expp, "value expected");
		return 0;
	}

	return 1;
}

int
chk_designator(expp, flag)
	register struct node *expp;
{
	/*	Find the name indicated by "expp", starting from the current
		scope.  "flag" indicates the kind of designator we expect:
		It contains the flags VARIABLE, indicating that the result must
		be something that can be assigned to.
		It may also contain the flag VALUE, indicating that a
		value is expected. In this case, VARIABLE may not be set.
		It also contains the flag DESIGNATOR, indicating that '['
		and '^' are allowed for this designator.
		Also contained may be the flag HASSELECTORS, indicating that
		the result must have selectors.
	*/
	register struct def *df;
	register struct type *tp;
	struct def *lookfor();

	expp->nd_type = error_type;

	if (expp->nd_class == Name) {
		expp->nd_def = lookfor(expp, CurrVis, 1);
		expp->nd_class = Def;
		expp->nd_type = expp->nd_def->df_type;
		if (expp->nd_type == error_type) return 0;
	}

	if (expp->nd_class == Link) {
		assert(expp->nd_symb == '.');
		assert(expp->nd_right->nd_class == Name);

		if (! chk_designator(expp->nd_left,
				     (flag|HASSELECTORS))) return 0;

		tp = expp->nd_left->nd_type;

		assert(tp->tp_fund == T_RECORD);

		df = lookup(expp->nd_right->nd_IDF, tp->rec_scope);

		if (!df) {
			id_not_declared(expp->nd_right);
			return 0;
		}
		else {
			expp->nd_right->nd_class = Def;
			expp->nd_right->nd_def = df;
			expp->nd_type = df->df_type;
			if (!(df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
node_error(expp->nd_right,
"identifier \"%s\" not exported from qualifying module",
df->df_idf->id_text);
				return 0;
			}
		}

		if (expp->nd_left->nd_class == Def &&
		    expp->nd_left->nd_def->df_kind == D_MODULE) {
			expp->nd_class = Def;
			expp->nd_def = df;
			FreeNode(expp->nd_left);
			FreeNode(expp->nd_right);
			expp->nd_left = expp->nd_right = 0;
		}
		else {
			return FlagCheck(expp->nd_right, df, flag);
		}
	}

	if (expp->nd_class == Def) {
		df = expp->nd_def;

		if (! FlagCheck(expp, df, flag)) return 0;

		if (df->df_kind & (D_ENUM | D_CONST)) {
			if (df->df_kind == D_ENUM) {
				expp->nd_class = Value;
				expp->nd_INT = df->enm_val;
				expp->nd_symb = INTEGER;
			}
			else  {
				char *fn;
				int ln;

				assert(df->df_kind == D_CONST);
				ln = expp->nd_lineno;
				fn = expp->nd_filename;
				*expp = *(df->con_const);
				expp->nd_lineno = ln;
				expp->nd_filename = fn;
			}
		}

		return 1;
	}

	if (! (flag & DESIGNATOR)) {
		node_error(expp, "identifier expected");
		return 0;
	}

	if (expp->nd_class == Oper) {
		struct type *tpl, *tpr;

		assert(expp->nd_symb == '[');

		if ( 
		    	!chk_designator(expp->nd_left, DESIGNATOR|VARIABLE)
		   ||
		    	!chk_expr(expp->nd_right)
		   ||
		    	expp->nd_left->nd_type == error_type
		   ) return 0;

		tpr = expp->nd_right->nd_type;
		tpl = expp->nd_left->nd_type;

		if (tpl->tp_fund != T_ARRAY) {
			node_error(expp,
				   "array index not belonging to an ARRAY");
			return 0;
		}

		/* Type of the index must be assignment compatible with
		   the index type of the array (Def 8.1)
		*/
		if ((tpl->next && !TstAssCompat(tpl->next, tpr)) ||
		    (!tpl->next && !TstAssCompat(intorcard_type, tpr))) {
			node_error(expp, "incompatible index type");
			return 0;
		}

		expp->nd_type = tpl->arr_elem;
		return 1;
	}

	if (expp->nd_class == Uoper) {
		assert(expp->nd_symb == '^');

		if (! chk_designator(expp->nd_right, DESIGNATOR|VARIABLE)) {
			return 0;
		}

		if (expp->nd_right->nd_type->tp_fund != T_POINTER) {
node_error(expp, "illegal operand for unary operator \"%s\"",
symbol2str(expp->nd_symb));
			return 0;
		}

		expp->nd_type = expp->nd_right->nd_type->next;
		return 1;
	}

	node_error(expp, "designator expected");
	return 0;
}

int
chk_oper(expp)
	register struct node *expp;
{
	/*	Check a binary operation.
	*/
	register struct node *left = expp->nd_left;
	register struct node *right = expp->nd_right;
	struct type *tpl = left->nd_type;
	struct type *tpr = right->nd_type;
	int errval = 1;
	
	if (tpl == intorcard_type) {
		if (tpr == int_type || tpr == card_type) {
			 left->nd_type = tpl = tpr;
		}
	}
	if (tpr == intorcard_type) {
		if (tpl == int_type || tpl == card_type) {
			right->nd_type = tpr = tpl;
		}
	}
	expp->nd_type = error_type;

	if (expp->nd_symb == IN) {
		/* Handle this one specially */
		expp->nd_type = bool_type;
		if (tpr->tp_fund != T_SET) {
node_error(expp, "RHS of IN operator not a SET type");
			return 0;
		}
		if (!TstAssCompat(tpl, tpr->next)) {
			/* Assignment compatible ???
			   I don't know! Should we be allowed to check
			   if a CARDINAL is a member of a BITSET???
			*/

node_error(expp, "IN operator: type of LHS not compatible with element type of RHS");
			return 0;
		}
		if (left->nd_class == Value && right->nd_class == Set) {
			cstset(expp);
		}
		return 1;
	}

	if (tpl->tp_fund == T_SUBRANGE) tpl = tpl->next;
	expp->nd_type = tpl;

	/* Operands must be compatible (distilled from Def 8.2)
	*/
	if (!TstCompat(tpl, tpr)) {
		node_error(expp, "incompatible types for operator \"%s\"",
					symbol2str(expp->nd_symb));
		return 0;
	}

	switch(expp->nd_symb) {
	case '+':
	case '-':
	case '*':
		switch(tpl->tp_fund) {
		case T_POINTER:
			if (! chk_address(tpl, tpr)) break;
			/* Fall through */
		case T_INTEGER:
		case T_CARDINAL:
		case T_INTORCARD:
			if (left->nd_class==Value && right->nd_class==Value) {
				cstbin(expp);
			}
			return 1;

		case T_SET:
			if (left->nd_class == Set && right->nd_class == Set) {
				cstset(expp);
			}
			/* Fall through */

		case T_REAL:
			return 1;
		}
		break;

	case '/':
		switch(tpl->tp_fund) {
		case T_SET:
			if (left->nd_class == Set && right->nd_class == Set) {
				cstset(expp);
			}
			/* Fall through */

		case T_REAL:
			return 1;
		}
		break;

	case DIV:
	case MOD:
		switch(tpl->tp_fund) {
		case T_POINTER:
			if (! chk_address(tpl, tpr)) break;
			/* Fall through */
		case T_INTEGER:
		case T_CARDINAL:
		case T_INTORCARD:
			if (left->nd_class==Value && right->nd_class==Value) {
				cstbin(expp);
			}
			return 1;
		}
		break;

	case OR:
	case AND:
	case '&':
		if (tpl == bool_type) {
			if (left->nd_class==Value && right->nd_class==Value) {
				cstbin(expp);
			}
			return 1;
		}
		errval = 3;
		break;

	case '=':
	case '#':
	case UNEQUAL:
	case GREATEREQUAL:
	case LESSEQUAL:
	case '<':
	case '>':
		expp->nd_type = bool_type;
		switch(tpl->tp_fund) {
		case T_SET:
			if (expp->nd_symb == '<' || expp->nd_symb == '>') {
				break;
			}
			if (left->nd_class == Set && right->nd_class == Set) {
				cstset(expp);
			}
			return 1;

		case T_INTEGER:
		case T_CARDINAL:
		case T_ENUMERATION:	/* includes boolean */
		case T_CHAR:
		case T_INTORCARD:
			if (left->nd_class==Value && right->nd_class==Value) {
				cstbin(expp);
			}
			return 1;

		case T_POINTER:
			if (chk_address(tpl, tpr) ||
			    expp->nd_symb == '=' ||
			    expp->nd_symb == UNEQUAL ||
			    expp->nd_symb == '#') return 1;
			break;

		case T_REAL:
			return 1;
		}

	default:
		assert(0);
	}
	switch(errval) {
	case 1:
		node_error(expp,"operator \"%s\": illegal operand type(s)", symbol2str(expp->nd_symb));
		break;

	case 3:
		node_error(expp, "BOOLEAN type(s) expected");
		break;

	default:
		assert(0);
	}
	return 0;
}

int
chk_address(tpl, tpr)
	register struct type *tpl, *tpr;
{
	
	if (tpl == address_type) {
		return tpr == address_type || tpr->tp_fund != T_POINTER;
	}

	if (tpr == address_type) {
		return tpl->tp_fund != T_POINTER;
	}

	return 0;
}

int
chk_uoper(expp)
	register struct node *expp;
{
	/*	Check an unary operation.
	*/
	register struct node *right = expp->nd_right;
	register struct type *tpr = right->nd_type;

	if (tpr->tp_fund == T_SUBRANGE) tpr = tpr->next;
	expp->nd_type = tpr;

	switch(expp->nd_symb) {
	case '+':
		if (tpr->tp_fund & T_NUMERIC) {
			expp->nd_token = right->nd_token;
			FreeNode(right);
			expp->nd_right = 0;
			return 1;
		}
		break;

	case '-':
		if (tpr->tp_fund & T_INTORCARD) {
			if (tpr == intorcard_type) {
				expp->nd_type = int_type;
			}
			if (right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		else if (tpr->tp_fund == T_REAL) {
			if (right->nd_class == Value) {
				expp->nd_token = right->nd_token;
				if (*(expp->nd_REL) == '-') {
					expp->nd_REL++;
				}
				else	expp->nd_REL--;
				FreeNode(right);
				expp->nd_right = 0;
			}
			return 1;
		}
		break;

	case NOT:
	case '~':
		if (tpr == bool_type) {
			if (right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		break;

	default:
		assert(0);
	}
	node_error(expp, "illegal operand for unary operator \"%s\"",
			symbol2str(expp->nd_symb));
	return 0;
}

struct node *
getvariable(arg)
	register struct node *arg;
{
	struct def *df;
	register struct node *left;

	arg = arg->nd_right;
	if (!arg) {
		node_error(arg, "too few parameters supplied");
		return 0;
	}

	left = arg->nd_left;

	if (! chk_designator(left, DESIGNATOR)) return 0;
	if (left->nd_class == Oper || left->nd_class == Uoper) {
		return arg;
	}

	df = 0;
	if (left->nd_class == Link) df = left->nd_right->nd_def;
	else if (left->nd_class == Def) df = left->nd_def;

	if (!df || !(df->df_kind & (D_VARIABLE|D_FIELD))) {
		node_error(arg, "variable expected");
		return 0;
	}

	return arg;
}

int
chk_std(expp, left, arg)
	register struct node *expp, *left, *arg;
{
	/*	Check a call of a standard procedure or function
	*/

	assert(left->nd_class == Def);
DO_DEBUG(3, debug("standard name \"%s\", %d", 
left->nd_def->df_idf->id_text, left->nd_def->df_value.df_stdname));

	switch(left->nd_def->df_value.df_stdname) {
	case S_ABS:
		if (!(arg = getarg(arg, T_NUMERIC, 0))) return 0;
		left = arg->nd_left;
		expp->nd_type = left->nd_type;
		if (left->nd_class == Value) cstcall(expp, S_ABS);
		break;

	case S_CAP:
		expp->nd_type = char_type;
		if (!(arg = getarg(arg, T_CHAR, 0))) return 0;
		left = arg->nd_left;
		if (left->nd_class == Value) cstcall(expp, S_CAP);
		break;

	case S_CHR:
		expp->nd_type = char_type;
		if (!(arg = getarg(arg, T_INTORCARD, 0))) return 0;
		left = arg->nd_left;
		if (left->nd_class == Value) cstcall(expp, S_CHR);
		break;

	case S_FLOAT:
		expp->nd_type = real_type;
		if (!(arg = getarg(arg, T_INTORCARD, 0))) return 0;
		break;

	case S_HIGH:
		if (!(arg = getarg(arg, T_ARRAY, 0))) return 0;
		expp->nd_type = arg->nd_left->nd_type->next;
		if (!expp->nd_type) {
			/* A dynamic array has no explicit index type
			*/
			expp->nd_type = intorcard_type;
		}
		else	cstcall(expp, S_MAX);
		break;

	case S_MAX:
	case S_MIN:
		if (!(arg = getarg(arg, T_DISCRETE, 0))) return 0;
		expp->nd_type = arg->nd_left->nd_type;
		cstcall(expp,left->nd_def->df_value.df_stdname);
		break;

	case S_ODD:
		if (!(arg = getarg(arg, T_INTORCARD, 0))) return 0;
		expp->nd_type = bool_type;
		if (arg->nd_left->nd_class == Value) cstcall(expp, S_ODD);
		break;

	case S_ORD:
		if (!(arg = getarg(arg, T_DISCRETE, 0))) return 0;
		expp->nd_type = card_type;
		if (arg->nd_left->nd_class == Value) cstcall(expp, S_ORD);
		break;

	case S_TSIZE:	/* ??? */
	case S_SIZE:
		expp->nd_type = intorcard_type;
		arg = getname(arg, D_FIELD|D_VARIABLE|D_TYPE|D_HIDDEN|D_HTYPE);
		if (!arg) return 0;
		cstcall(expp, S_SIZE);
		break;

	case S_TRUNC:
		expp->nd_type = card_type;
		if (!(arg = getarg(arg, T_REAL, 0))) return 0;
		break;

	case S_VAL:
		{
		struct type *tp;

		if (!(arg = getname(arg, D_HIDDEN|D_HTYPE|D_TYPE))) return 0;
		tp = arg->nd_left->nd_def->df_type;
		if (tp->tp_fund == T_SUBRANGE) tp = tp->next;
		if (!(tp->tp_fund & T_DISCRETE)) {
			node_error(arg, "unexpected type");
			return 0;
		}
		expp->nd_type = arg->nd_left->nd_def->df_type;
		expp->nd_right = arg->nd_right;
		arg->nd_right = 0;
		FreeNode(arg);
		arg = getarg(expp, T_INTORCARD, 0);
		if (!arg) return 0;
		if (arg->nd_left->nd_class == Value) cstcall(expp, S_VAL);
		break;
		}

	case S_ADR:
		expp->nd_type = address_type;
		if (!(arg = getarg(arg, 0, 1))) return 0;
		break;

	case S_DEC:
	case S_INC:
		expp->nd_type = 0;
		if (!(arg = getvariable(arg))) return 0;
		if (arg->nd_right) {
			if (!(arg = getarg(arg, T_INTORCARD, 0))) return 0;
		}
		break;

	case S_HALT:
		expp->nd_type = 0;
		break;

	case S_EXCL:
	case S_INCL:
		{
		struct type *tp;

		expp->nd_type = 0;
		if (!(arg = getvariable(arg))) return 0;
		tp = arg->nd_left->nd_type;
		if (tp->tp_fund != T_SET) {
node_error(arg, "EXCL and INCL expect a SET parameter");
			return 0;
		}
		if (!(arg = getarg(arg, T_DISCRETE, 0))) return 0;
		if (!TstAssCompat(tp->next, arg->nd_left->nd_type)) {
			/* What type of compatibility do we want here?
			   apparently assignment compatibility! ??? ???
			*/
			node_error(arg, "unexpected type");
			return 0;
		}
		break;
		}

	default:
		assert(0);
	}

	if (arg->nd_right) {
		node_error(arg->nd_right, "too many parameters supplied");
		return 0;
	}

	return 1;
}
