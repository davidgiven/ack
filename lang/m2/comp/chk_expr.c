/* E X P R E S S I O N   C H E C K I N G */

static char *RcsId = "$Header$";

/*	Check expressions, and try to evaluate them as far as possible.
*/
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

#include	"debug.h"

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
		return	chk_expr(expp->nd_left) &&
			chk_expr(expp->nd_right) &&
			chk_oper(expp);

	case Uoper:
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
		return chk_name(expp);

	case Call:
		return chk_call(expp);

	case Link:
		return chk_name(expp);

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
		findname(nd);
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
getarg(argp, bases)
	struct node *argp;
{
	struct type *tp;

	if (!argp->nd_right) {
		node_error(argp, "too few arguments supplied");
		return 0;
	}
	argp = argp->nd_right;
	if (!chk_expr(argp->nd_left)) return 0;
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
	findname(argp->nd_left);
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
	findname(left);

	if (left->nd_type == error_type) return 0;
	if (left->nd_class == Def &&
	    (left->nd_def->df_kind & (D_HTYPE|D_TYPE|D_HIDDEN))) {
		/* It was a type cast. This is of course not portable.
		   No runtime action. Remove it.
		*/
		arg = expp->nd_right;
		if ((! arg) || arg->nd_right) {
node_error(expp, "only one parameter expected in type cast");
			return 0;
		}
		arg = arg->nd_left;
		if (! chk_expr(arg)) return 0;
		if (arg->nd_type->tp_size != left->nd_type->tp_size) {
node_error(expp, "size of type in type cast does not match size of operand");
			return 0;
		}
		arg->nd_type = left->nd_type;
		FreeNode(expp->nd_left);
		expp->nd_right->nd_left = 0;
		FreeNode(expp->nd_right);
		*expp = *arg;
		arg->nd_left = 0;
		arg->nd_right = 0;
		FreeNode(arg);
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
			assert(left->nd_class == Def);
DO_DEBUG(3, debug("standard name \"%s\", %d", 
left->nd_def->df_idf->id_text, left->nd_def->df_value.df_stdname));
			switch(left->nd_def->df_value.df_stdname) {
			case S_ABS:
				arg = getarg(arg, T_NUMERIC);
				if (! arg) return 0;
				left = arg->nd_left;
				expp->nd_type = left->nd_type;
				if (left->nd_class == Value) {
					cstcall(expp, S_ABS);
				}
				break;

			case S_CAP:
				arg = getarg(arg, T_CHAR);
				expp->nd_type = char_type;
				if (!arg) return 0;
				left = arg->nd_left;
				if (left->nd_class == Value) {
					cstcall(expp, S_CAP);
				}
				break;

			case S_CHR:
				arg = getarg(arg, T_INTORCARD);
				expp->nd_type = char_type;
				if (!arg) return 0;
				if (arg->nd_left->nd_class == Value) {
					cstcall(expp, S_CHR);
				}
				break;

			case S_FLOAT:
				arg = getarg(arg, T_INTORCARD);
				expp->nd_type = real_type;
				if (!arg) return 0;
				break;

			case S_HIGH:
				arg = getarg(arg, T_ARRAY);
				if (!arg) return 0;
				expp->nd_type = arg->nd_left->nd_type->next;
				if (!expp->nd_type) {
					/* A dynamic array has no explicit
					   index type
					*/
					expp->nd_type = int_type;
				}
				else	cstcall(expp, S_MAX);
				break;

			case S_MAX:
			case S_MIN:
				arg = getarg(arg, T_DISCRETE);
				if (!arg) return 0;
				expp->nd_type = arg->nd_left->nd_type;
				cstcall(expp,left->nd_def->df_value.df_stdname);
				break;

			case S_ODD:
				arg = getarg(arg, T_INTORCARD);
				if (!arg) return 0;
				expp->nd_type = bool_type;
				if (arg->nd_left->nd_class == Value) {
					cstcall(expp, S_ODD);
				}
				break;

			case S_ORD:
				arg = getarg(arg, T_DISCRETE);
				if (!arg) return 0;
				expp->nd_type = card_type;
				if (arg->nd_left->nd_class == Value) {
					cstcall(expp, S_ORD);
				}
				break;

			case S_TSIZE:	/* ??? */
			case S_SIZE:
				arg = getname(arg, D_FIELD|D_VARIABLE|D_TYPE|D_HIDDEN|D_HTYPE);
				expp->nd_type = intorcard_type;
				if (!arg) return 0;
				cstcall(expp, S_SIZE);
				break;

			case S_TRUNC:
				arg = getarg(arg, T_REAL);
				if (!arg) return 0;
				expp->nd_type = card_type;
				break;

			case S_VAL: {
				struct type *tp;

				arg = getname(arg, D_HIDDEN|D_HTYPE|D_TYPE);
				if (!arg) return 0;
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
				arg = getarg(expp, T_INTORCARD);
				if (!arg) return 0;
				if (arg->nd_left->nd_class == Value) {
					cstcall(expp, S_VAL);
				}
				break;
				}

			case S_ADR:
				arg = getname(arg, D_VARIABLE|D_FIELD|D_PROCEDURE);
				expp->nd_type = address_type;
				if (!arg) return 0;
				break;

			case S_DEC:
			case S_INC:
				expp->nd_type = 0;
				arg = getname(arg, D_VARIABLE|D_FIELD);
				if (!arg) return 0;
				if (arg->nd_right) {
					arg = getarg(arg, T_INTORCARD);
					if (!arg) return 0;
				}
				break;

			case S_HALT:
				expp->nd_type = 0;
				break;

			case S_EXCL:
			case S_INCL: {
				struct type *tp;

				expp->nd_type = 0;
				arg = getname(arg, D_VARIABLE|D_FIELD);
				if (!arg) return 0;
				tp = arg->nd_left->nd_type;
				if (tp->tp_fund != T_SET) {
node_error(arg, "EXCL and INCL expect a SET parameter");
					return 0;
				}
				arg = getarg(arg, T_DISCRETE);
				if (!arg) return 0;
				if (!TstCompat(tp->next, arg->nd_left->nd_type)) {
					node_error(arg, "unexpected type");
					return 0;
				}
				break;
				}

			default:
				assert(0);
			}
			if (arg->nd_right) {
				node_error(arg->nd_right,
					"too many parameters supplied");
				return 0;
			}
			return 1;
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

	expp->nd_type = left->nd_type->next;
	param = left->nd_type->prc_params;
	arg = expp;

	while (param) {
		arg = getarg(arg, 0);
		if (!arg) return 0;
		if (param->par_var &&
		    ! TstCompat(param->par_type, arg->nd_left->nd_type)) {
node_error(arg->nd_left, "type incompatibility in var parameter");
			return 0;
		}
		else
		if (!param->par_var &&
		    !TstAssCompat(param->par_type, arg->nd_left->nd_type)) {
node_error(arg->nd_left, "type incompatibility in value parameter");
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

findname(expp)
	register struct node *expp;
{
	/*	Find the name indicated by "expp", starting from the current
		scope.
	*/
	register struct def *df;
	register struct type *tp;
	struct def *lookfor();

	expp->nd_type = error_type;
	if (expp->nd_class == Name) {
		expp->nd_def = lookfor(expp, CurrentScope, 1);
		expp->nd_class = Def;
		expp->nd_type = expp->nd_def->df_type;
		return;
	}
	if (expp->nd_class == Link) {
		assert(expp->nd_symb == '.');
		assert(expp->nd_right->nd_class == Name);
		findname(expp->nd_left);
		tp = expp->nd_left->nd_type;
		if (tp == error_type) {
			df = ill_df;
		}
		else if (tp->tp_fund != T_RECORD) {
			/* This is also true for modules */
			node_error(expp,"illegal selection");
			df = ill_df;
		}
		else df = lookup(expp->nd_right->nd_IDF, tp->rec_scope);
		if (!df) {
			df = ill_df;
			id_not_declared(expp->nd_right);
		}
		else if (df != ill_df) {
			expp->nd_type = df->df_type;
			if (!(df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
node_error(expp->nd_right,
"identifier \"%s\" not exported from qualifying module",
df->df_idf->id_text);
			}
		}
		if (expp->nd_left->nd_class == Def) {
			expp->nd_class = Def;
			expp->nd_def = df;
			FreeNode(expp->nd_left);
			FreeNode(expp->nd_right);
			expp->nd_left = expp->nd_right = 0;
		}
		return;
	}
	if (expp->nd_class == Oper) {
		assert(expp->nd_symb == '[');
		findname(expp->nd_left);
		if (chk_expr(expp->nd_right) &&
		    expp->nd_left->nd_type != error_type &&
		    chk_oper(expp)) /* ??? */ ;
		return;
	}
	if (expp->nd_class == Uoper && expp->nd_symb == '^') {
		findname(expp->nd_right);
		if (expp->nd_right->nd_type != error_type &&
	    		chk_uoper(expp)) /* ??? */ ;
	}
	return;
}

int
chk_name(expp)
	register struct node *expp;
{
	register struct def *df;

	findname(expp);
	assert(expp->nd_class == Def);
	df = expp->nd_def;
	if (df->df_kind == D_ERROR) return 0;
	if (df->df_kind & (D_ENUM | D_CONST)) {
		if (df->df_kind == D_ENUM) {
			expp->nd_class = Value;
			expp->nd_INT = df->enm_val;
			expp->nd_symb = INTEGER;
		}
		else if (df->df_kind == D_CONST) {
			*expp = *(df->con_const);
		}
	}
	return 1;
}

int
chk_oper(expp)
	register struct node *expp;
{
	/*	Check a binary operation.
	*/
	register struct type *tpl = expp->nd_left->nd_type;
	register struct type *tpr = expp->nd_right->nd_type;
	char *symbol2str();
	int errval = 1;
	
	if (tpl == intorcard_type) {
		if (tpr == int_type || tpr == card_type) {
			expp->nd_left->nd_type = tpl = tpr;
		}
	}
	if (tpr == intorcard_type) {
		if (tpl == int_type || tpl == card_type) {
			expp->nd_right->nd_type = tpr = tpl;
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
		if (!TstCompat(tpl, tpr->next)) {
node_error(expp, "IN operator: type of LHS not compatible with element type of RHS");
			return 0;
		}
		if (expp->nd_left->nd_class == Value &&
		    expp->nd_right->nd_class == Set) {
			cstset(expp);
		}
		return 1;
	}

	if (expp->nd_symb == '[') {
		/* Handle ARRAY selection specially too! */
		if (tpl->tp_fund != T_ARRAY) {
			node_error(expp,
				   "array index not belonging to an ARRAY");
			return 0;
		}
		if (!TstCompat(tpl->next, tpr)) {
			node_error(expp, "incompatible index type");
			return 0;
		}
		expp->nd_type = tpl->arr_elem;
		return 1;
	}

	if (tpl->tp_fund == T_SUBRANGE) tpl = tpl->next;
	expp->nd_type = tpl;

	if (!TstCompat(tpl, tpr)) {
		node_error(expp,
			   "incompatible types for operator \"%s\"",
			   symbol2str(expp->nd_symb));
		return 0;
	}
	
	switch(expp->nd_symb) {
	case '+':
	case '-':
	case '*':
		switch(tpl->tp_fund) {
		case T_INTEGER:
		case T_CARDINAL:
		case T_INTORCARD:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;

		case T_SET:
			if (expp->nd_left->nd_class == Set &&
			    expp->nd_right->nd_class == Set) {
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
			if (expp->nd_left->nd_class == Set &&
			    expp->nd_right->nd_class == Set) {
				cstset(expp);
			}
			/* Fall through */

		case T_REAL:
			return 1;
		}
		break;

	case DIV:
	case MOD:
		if (tpl->tp_fund & T_INTORCARD) {
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		}
		break;

	case OR:
	case AND:
	case '&':
		if (tpl == bool_type) {
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
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
			if (expp->nd_left->nd_class == Set &&
			    expp->nd_right->nd_class == Set) {
				cstset(expp);
			}
			return 1;

		case T_INTEGER:
		case T_CARDINAL:
		case T_ENUMERATION:	/* includes boolean */
		case T_CHAR:
		case T_INTORCARD:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;

		case T_POINTER:
			if (expp->nd_symb == '=' ||
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
chk_uoper(expp)
	register struct node *expp;
{
	/*	Check an unary operation.
	*/
	register struct type *tpr = expp->nd_right->nd_type;

	if (tpr->tp_fund == T_SUBRANGE) tpr = tpr->next;
	expp->nd_type = tpr;

	switch(expp->nd_symb) {
	case '+':
		if (tpr->tp_fund & T_NUMERIC) {
			expp->nd_token = expp->nd_right->nd_token;
			FreeNode(expp->nd_right);
			expp->nd_right = 0;
			return 1;
		}
		break;

	case '-':
		if (tpr->tp_fund & T_INTORCARD) {
			if (expp->nd_right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		else if (tpr->tp_fund == T_REAL) {
			if (expp->nd_right->nd_class == Value) {
				expp->nd_token = expp->nd_right->nd_token;
				if (*(expp->nd_REL) == '-') {
					expp->nd_REL++;
				}
				else	expp->nd_REL--;
				FreeNode(expp->nd_right);
				expp->nd_right = 0;
			}
			return 1;
		}
		break;

	case NOT:
	case '~':
		if (tpr == bool_type) {
			if (expp->nd_right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		break;

	case '^':
		if (tpr->tp_fund != T_POINTER) break;
		expp->nd_type = tpr->next;
		return 1;

	default:
		assert(0);
	}
	node_error(expp, "illegal operand for unary operator \"%s\"",
			symbol2str(expp->nd_symb));
	return 0;
}
