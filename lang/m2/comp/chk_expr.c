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

int
chk_expr(expp)
	register struct node *expp;
{
	/*	Check the expression indicated by expp for semantic errors,
		identify identifiers used in it, replace constants by
		their value.
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
	if (expp->nd_left) {
		/* A type was given. Check it out
		*/
		(void) findname(expp->nd_left);
		assert(expp->nd_left->nd_class == Def);
		df = expp->nd_left->nd_def;
		if ((df->df_kind != D_TYPE && df->df_kind != D_ERROR) ||
		    (df->df_type->tp_fund != T_SET)) {
			node_error(expp, "Illegal set type");
			return 0;
		}
		tp = df->df_type;
	}
	else	tp = bitset_type;

	/* Now check the elements given, and try to compute a constant set.
	*/
	set = (arith *) Malloc(tp->tp_size * sizeof(arith) / wrd_size);
	nd = expp->nd_right;
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');
		if (!chk_el(nd->nd_left, tp->next, &set)) return 0;
		nd = nd->nd_right;
	}
	expp->nd_type = tp;
	if (set) {
		/* Yes, in was a constant set, and we managed to compute it!
		*/
		expp->nd_class = Set;
		expp->nd_set = set;
		FreeNode(expp->nd_left);
		FreeNode(expp->nd_right);
		expp->nd_left = expp->nd_right = 0;
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
	if (expp->nd_class == Link && expp->nd_symb == UPTO) {
		/* { ... , expr1 .. expr2,  ... }
		   First check expr1 and expr2, and try to compute them.
		*/
		if (!chk_el(expp->nd_left, tp, set) ||
		    !chk_el(expp->nd_right, tp, set)) {
			return 0;
		}
		if (expp->nd_left->nd_class == Value &&
		    expp->nd_right->nd_class == Value) {
			/* We have a constant range. Put all elements in the
			   set
			*/
			register int i;

		    	if (expp->nd_left->nd_INT > expp->nd_right->nd_INT) {
node_error(expp, "Lower bound exceeds upper bound in range");
				return rem_set(set);
			}
			
			if (*set) for (i = expp->nd_left->nd_INT + 1;
				     i < expp->nd_right->nd_INT; i++) {
				(*set)[i/wrd_bits] |= (1 << (i % wrd_bits));
			}
		}
		else if (*set) {
			free(*set);
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
		node_error(expp, "Set element has incompatible type");
		return rem_set(set);
	}
	if (expp->nd_class == Value) {
	    	if ((tp->tp_fund != T_ENUMERATION &&
		     (expp->nd_INT < tp->sub_lb || expp->nd_INT > tp->sub_ub))
		   ||
		    (tp->tp_fund == T_ENUMERATION &&
		     (expp->nd_INT < 0 || expp->nd_INT > tp->enm_ncst))
		   ) {
			node_error(expp, "Set element out of range");
			return rem_set(set);
		}
		if (*set) (*set)[expp->nd_INT/wrd_bits] |= (1 << (expp->nd_INT%wrd_bits));
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
		node_error(argp, "Too few arguments supplied");
		return 0;
	}
	argp = argp->nd_right;
	if (!chk_expr(argp->nd_left)) return 0;
	tp = argp->nd_left->nd_type;
	if (tp->tp_fund == T_SUBRANGE) tp = tp->next;
	if (!(tp->tp_fund & bases)) {
		node_error(argp, "Unexpected type");
		return 0;
	}
	return argp;
}

struct node *
getname(argp, kinds)
	struct node *argp;
{
	if (!argp->nd_right) {
		node_error(argp, "Too few arguments supplied");
		return 0;
	}
	argp = argp->nd_right;
	if (!findname(argp->nd_left)) return 0;
	assert(argp->nd_left->nd_class == Def);
	if (!(argp->nd_left->nd_def->df_kind & kinds)) {
		node_error(argp, "Unexpected type");
		return 0;
	}
	return argp;
}

int
chk_call(expp)
	register struct node *expp;
{
	register struct type *tp;
	register struct node *left;
	register struct node *arg;

	expp->nd_type = error_type;
	(void) findname(expp->nd_left);
	left = expp->nd_left;
	tp = left->nd_type;

	if (tp == error_type) return 0;
	if (left->nd_class == Def &&
	    (left->nd_def->df_kind & (D_HTYPE|D_TYPE|D_HIDDEN))) {
		/* A type cast. This is of course not portable.
		   No runtime action. Remove it.
		*/
		arg = expp->nd_right;
		if (!arg || arg->nd_right) {
node_error(expp, "Only one parameter expected in type cast");
			return 0;
		}
		if (! chk_expr(arg->nd_left)) return 0;
		if (arg->nd_left->nd_type->tp_size !=
		    	left->nd_type->tp_size) {
node_error(expp, "Size of type in type cast does not match size of operand");
			return 0;
		}
		arg->nd_left->nd_type = left->nd_type;
		FreeNode(expp->nd_left);
		*expp = *(arg->nd_left);
		arg->nd_left->nd_left = 0;
		arg->nd_left->nd_right = 0;
		FreeNode(arg);
		return 1;
	}

	if ((left->nd_class == Def && left->nd_def->df_kind == D_PROCEDURE) ||
	    tp->tp_fund == T_PROCEDURE) {
		/* A procedure call. it may also be a call to a
		   standard procedure
		*/
		arg = expp;
		if (tp == std_type) {
			assert(left->nd_class == Def);
			switch(left->nd_def->df_value.df_stdname) {
			case S_ABS:
				arg = getarg(arg, T_INTEGER|T_CARDINAL|T_REAL);
				if (! arg) return 0;
				expp->nd_type = arg->nd_left->nd_type;
				break;
			case S_CAP:
				arg = getarg(arg, T_CHAR);
				expp->nd_type = char_type;
				if (!arg) return 0;
				break;
			case S_CHR:
				arg = getarg(arg, T_INTEGER|T_CARDINAL);
				expp->nd_type = char_type;
				if (!arg) return 0;
				break;
			case S_FLOAT:
				arg = getarg(arg, T_CARDINAL|T_INTEGER);
				expp->nd_type = real_type;
				if (!arg) return 0;
				break;
			case S_HIGH:
				arg = getarg(arg, T_ARRAY);
				if (!arg) return 0;
				expp->nd_type = arg->nd_left->nd_type->next;
				if (!expp->nd_type) expp->nd_type = int_type;
				break;
			case S_MAX:
			case S_MIN:
				arg = getarg(arg, T_ENUMERATION|T_CHAR|T_INTEGER|T_CARDINAL);
				if (!arg) return 0;
				expp->nd_type = arg->nd_left->nd_type;
				break;
			case S_ODD:
				arg = getarg(arg, T_INTEGER|T_CARDINAL);
				if (!arg) return 0;
				expp->nd_type = bool_type;
				break;
			case S_ORD:
				arg = getarg(arg, T_ENUMERATION|T_CHAR|T_INTEGER|T_CARDINAL);
				if (!arg) return 0;
				expp->nd_type = card_type;
				break;
			case S_TSIZE:	/* ??? */
			case S_SIZE:
				arg = getname(arg, D_FIELD|D_VARIABLE|D_TYPE|D_HIDDEN|D_HTYPE);
				expp->nd_type = intorcard_type;
				if (!arg) return 0;
				break;
			case S_TRUNC:
				arg = getarg(arg, T_REAL);
				if (!arg) return 0;
				expp->nd_type = card_type;
				break;
			case S_VAL:
				arg = getname(arg, D_HIDDEN|D_HTYPE|D_TYPE);
				if (!arg) return 0;
				tp = arg->nd_left->nd_def->df_type;
				if (tp->tp_fund == T_SUBRANGE) tp = tp->next;
				if (!(tp->tp_fund & (T_ENUMERATION|T_CHAR|T_INTEGER|T_CARDINAL))) {
					node_error(arg, "unexpected type");
					return 0;
				}
				expp->nd_type = arg->nd_left->nd_def->df_type;
				FreeNode(arg->nd_left);
				arg->nd_left = 0;
				arg = getarg(arg, T_INTEGER|T_CARDINAL);
				if (!arg) return 0;
				break;
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
					arg = getarg(arg, T_INTEGER|T_CARDINAL);
					if (!arg) return 0;
				}
				break;
			case S_HALT:
				expp->nd_type = 0;
				break;
			case S_EXCL:
			case S_INCL:
				expp->nd_type = 0;
				arg = getname(arg, D_VARIABLE|D_FIELD);
				if (!arg) return 0;
				tp = arg->nd_left->nd_type;
				if (tp->tp_fund != T_SET) {
node_error(arg, "EXCL and INCL expect a SET parameter");
					return 0;
				}
				arg = getarg(arg, T_INTEGER|T_CARDINAL|T_CHAR|T_ENUMERATION);
				if (!arg) return 0;
				if (!TstCompat(tp->next, arg->nd_left->nd_type)) {
					node_error(arg, "Unexpected type");
					return 0;
				}
				break;
			default:
				assert(0);
			}
			if (arg->nd_right) {
				node_error(arg->nd_right,
					"Too many parameters supplied");
				return 0;
			}
			FreeNode(expp->nd_left);
			expp->nd_left = 0;
			return 1;
		}
		return 1;
	}
	node_error(expp->nd_left, "procedure, type, or function expected");
	return 0;
}

findname(expp)
	register struct node *expp;
{
	/*	Find the name indicated by "expp", starting from the current
		scope.
	*/
	register struct def *df;
	struct def *lookfor();
	register struct type *tp;
	int scope;
	int module;

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
			node_error(expp,"Illegal selection");
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
"identifier \"%s\" not exprted from qualifying module",
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
		(void) findname(expp->nd_left);
		if (chk_expr(expp->nd_right, 0) &&
		    expp->nd_left->nd_type != error_type &&
		    chk_oper(expp)) /* ??? */ ;
		return 1;
	}
	if (expp->nd_class == Uoper && expp->nd_symb == '^') {
		(void) findname(expp->nd_right);
		if (expp->nd_right->nd_type != error_type &&
	    		chk_uoper(expp)) /* ??? */ ;
	}
	return 0;
}

int
chk_name(expp)
	register struct node *expp;
{
	register struct def *df;

	(void) findname(expp);
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
		return 1;
	}

	if (expp->nd_symb == '[') {
		/* Handle ARRAY selection specially too! */
		if (tpl->tp_fund != T_ARRAY) {
node_error(expp, "array index not belonging to an ARRAY");
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
node_error(expp, "Incompatible types for operator \"%s\"", symbol2str(expp->nd_symb));
		return 0;
	}
	
	switch(expp->nd_symb) {
	case '+':
	case '-':
	case '*':
		switch(tpl->tp_fund) {
		case T_INTEGER:
		case T_CARDINAL:
		case T_SET:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case T_REAL:
			return 1;
		}
		break;
	case '/':
		switch(tpl->tp_fund) {
		case T_SET:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case T_REAL:
			return 1;
		}
		break;
	case DIV:
	case MOD:
		switch(tpl->tp_fund) {
		case T_INTEGER:
		case T_CARDINAL:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		}
		break;
	case OR:
	case AND:
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
	case GREATEREQUAL:
	case LESSEQUAL:
	case '<':
	case '>':
		switch(tpl->tp_fund) {
		case T_SET:
			if (expp->nd_symb == '<' || expp->nd_symb == '>') {
				break;
			}
			if (expp->nd_left->nd_class == Set &&
			    expp->nd_right->nd_class == Set) {
				cstbin(expp);
			}
			return 1;
		case T_INTEGER:
		case T_CARDINAL:
		case T_ENUMERATION:	/* includes boolean */
		case T_CHAR:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case T_POINTER:
			if (!(expp->nd_symb == '=' || expp->nd_symb == '#')) {
				break;
			}
			/* Fall through */
		case T_REAL:
			return 1;
		}
	default:
		assert(0);
	}
	switch(errval) {
	case 1:
		node_error(expp,"Operator \"%s\": illegal operand type(s)", symbol2str(expp->nd_symb));
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
		switch(tpr->tp_fund) {
		case T_INTEGER:
		case T_REAL:
		case T_CARDINAL:
			expp->nd_token = expp->nd_right->nd_token;
			FreeNode(expp->nd_right);
			expp->nd_right = 0;
			return 1;
		}
		break;
	case '-':
		switch(tpr->tp_fund) {
		case T_INTEGER:
			if (expp->nd_right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		case T_REAL:
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
	node_error(expp, "Illegal operand for unary operator \"%s\"",
			symbol2str(expp->nd_symb));
	return 0;
}
