/* E X P R E S S I O N   C H E C K I N G */

static char *RcsId = "$Header$";

/*	Check expressions, and try to evaluate them as far as possible.
*/
#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>
#include	"idf.h"
#include	"type.h"
#include	"def.h"
#include	"LLlex.h"
#include	"node.h"
#include	"Lpars.h"
#include	"scope.h"

int
chk_expr(expp, const)
	register struct node *expp;
{
	/*	Check the expression indicated by expp for semantic errors,
		identify identifiers used in it, replace constants by
		their value.
	*/

	switch(expp->nd_class) {
	case Oper:
		return	chk_expr(expp->nd_left, const) &&
			chk_expr(expp->nd_right, const) &&
			chk_oper(expp, const);
	case Uoper:
		return	chk_expr(expp->nd_right, const) &&
			chk_uoper(expp, const);
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
		return chk_set(expp, const);
	case Name:
		return chk_name(expp, const);
	case Call:
		return chk_call(expp, const);
	case Link:
		return chk_name(expp, const);
	}
	/*NOTREACHED*/
}

int
chk_set(expp, const)
	register struct node *expp;
{
	/* ??? */
	return 1;
}

int
chk_call(expp, const)
	register struct node *expp;
{
	/* ??? */
	return 1;
}

struct def *
findname(expp)
	register struct node *expp;
{
	/*	Find the name indicated by "expp", starting from the current
		scope.
	*/
	register struct def *df;
	struct def *lookfor();
	register struct node *nd;
	int scope;
	int module;

	if (expp->nd_class == Name) {
		return lookfor(expp, CurrentScope, 1);
	}
	assert(expp->nd_class == Link && expp->nd_symb == '.');
	assert(expp->nd_left->nd_class == Name);
	df = lookfor(expp->nd_left, CurrentScope, 1);
	if (df->df_kind == D_ERROR) return df;
	nd = expp;
	while (nd->nd_class == Link) {
		struct node *nd1;

		if (!(scope = has_selectors(df))) {
			node_error(nd, "identifier \"%s\" has no selectors",
					df->df_idf->id_text);
			return ill_df;
		}
		nd = nd->nd_right;
		if (nd->nd_class == Name) nd1 = nd;
		else nd1 = nd->nd_left;
		module = (df->df_kind == D_MODULE);
		df = lookup(nd1->nd_IDF, scope);
		if (!df) {
			id_not_declared(nd1);
			return ill_df;
		}
		if (module && !(df->df_flags&(D_EXPORTED|D_QEXPORTED))) {
node_error(nd1, "identifier \"%s\" not exprted from qualifying module",
df->df_idf->id_text);
		}
	}
	return df;
}

int
chk_name(expp, const)
	register struct node *expp;
{
	register struct def *df;
	int retval = 1;

	df = findname(expp);
	if (df->df_kind == D_ERROR) {
		retval = 0;
	}
	expp->nd_type = df->df_type;
	if (df->df_kind == D_ENUM || df->df_kind == D_CONST) {
		if (expp->nd_left) FreeNode(expp->nd_left);
		if (expp->nd_right) FreeNode(expp->nd_right);
		if (df->df_kind == D_ENUM) {
			expp->nd_left = expp->nd_right = 0;
			expp->nd_class = Value;
			expp->nd_INT = df->enm_val;
			expp->nd_symb = INTEGER;
		}
		else if (df->df_kind == D_CONST) {
			*expp = *(df->con_const);
		}
	}
	else if (const) {
		node_error(expp, "constant expected");
		retval = 0;
	}
	return retval;
}

int
chk_oper(expp, const)
	register struct node *expp;
{
	/*	Check a binary operation. If "const" is set, also check
		that it is constant.
		The code is ugly !
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

	if (expp->nd_symb == IN) {
		/* Handle this one specially */
		expp->nd_type == bool_type;
		if (tpr->tp_fund != SET) {
node_error(expp, "RHS of IN operator not a SET type");
			return 0;
		}
		if (!TstCompat(tpl, tpr->next)) {
node_error(expp, "IN operator: type of LHS not compatible with element type of RHS");
			return 0;
		}
		return 1;
	}

	if (tpl->tp_fund == SUBRANGE) tpl = tpl->next;
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
		case INTEGER:
		case INTORCARD:
		case CARDINAL:
		case LONGINT:
		case SET:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case REAL:
		case LONGREAL:
			if (const) {
				errval = 2;
				break;
			}
			return 1;
		}
		break;
	case '/':
		switch(tpl->tp_fund) {
		case SET:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case REAL:
		case LONGREAL:
			if (const) {
				errval = 2;
				break;
			}
			return 1;
		}
		break;
	case DIV:
	case MOD:
		switch(tpl->tp_fund) {
		case INTEGER:
		case INTORCARD:
		case CARDINAL:
		case LONGINT:
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
		case SET:
			if (expp->nd_symb == '<' || expp->nd_symb == '>') {
				break;
			}
		case INTEGER:
		case INTORCARD:
		case LONGINT:
		case CARDINAL:
		case ENUMERATION:	/* includes boolean */
		case CHAR:
			if (expp->nd_left->nd_class == Value &&
			    expp->nd_right->nd_class == Value) {
				cstbin(expp);
			}
			return 1;
		case POINTER:
			if (!(expp->nd_symb == '=' || expp->nd_symb == '#')) {
				break;
			}
			/* Fall through */
		case REAL:
		case LONGREAL:
			if (const) {
				errval = 2;
				break;
			}
			return 1;
		}
	default:
		assert(0);
	}
	switch(errval) {
	case 1:
		node_error(expp,"Operator \"%s\": illegal operand type(s)", symbol2str(expp->nd_symb));
		break;
	case 2:
		node_error(expp, "Expression not constant");
		break;
	case 3:
		node_error(expp, "BOOLEAN type(s) expected");
		break;
	}
	return 0;
}

int
chk_uoper(expp, const)
	register struct node *expp;
{
	/*	Check an unary operation. If "const" is set, also check that
		it can be evaluated compile-time.
	*/
	register struct type *tpr = expp->nd_right->nd_type;

	if (tpr->tp_fund == SUBRANGE) tpr = tpr->next;
	expp->nd_type = tpr;

	switch(expp->nd_symb) {
	case '+':
		switch(tpr->tp_fund) {
		case INTEGER:
		case LONGINT:
		case REAL:
		case LONGREAL:
		case CARDINAL:
		case INTORCARD:
			expp->nd_token = expp->nd_right->nd_token;
			FreeNode(expp->nd_right);
			expp->nd_right = 0;
			return 1;
		}
		break;
	case '-':
		switch(tpr->tp_fund) {
		case INTEGER:
		case LONGINT:
		case INTORCARD:
			if (expp->nd_right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		case REAL:
		case LONGREAL:
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
	default:
		assert(0);
	}
	node_error(expp, "Illegal operand for unary operator \"%s\"",
			symbol2str(expp->nd_symb));
	return 0;
}
