/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* E X P R E S S I O N   C H E C K I N G */

/* $Header$ */

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
#include	"LLlex.h"
#include	"def.h"
#include	"node.h"
#include	"scope.h"
#include	"const.h"
#include	"standards.h"
#include	"chk_expr.h"
#include	"misc.h"
#include	"warning.h"

extern char *symbol2str();
extern char *sprint();

STATIC int
Xerror(nd, mess, edf)
	struct node *nd;
	char *mess;
	register struct def *edf;
{
	if (edf) {
		if (edf->df_kind != D_ERROR)  {
			node_error(nd,"\"%s\": %s", edf->df_idf->id_text, mess);
		}
	}
	else node_error(nd, "%s", mess);
	return 0;
}

MkCoercion(pnd, tp)
	struct node **pnd;
	register struct type *tp;
{
	register struct node *nd = *pnd;
	register struct type *nd_tp = nd->nd_type;
	extern int pass_1;
	int w = 0;

	if (nd_tp == tp) return;
	if (nd_tp->tp_fund == T_STRING) return;
	nd_tp = BaseType(nd_tp);
	if (nd->nd_class == Value &&
	    (nd_tp->tp_fund != T_REAL && tp->tp_fund != REAL)) {
		switch(tp->tp_fund) {
		case T_SUBRANGE:
			if (! chk_bounds(tp->sub_lb, nd->nd_INT, 
				BaseType(tp)->tp_fund) ||
			    ! chk_bounds(nd->nd_INT, tp->sub_ub,
				BaseType(tp)->tp_fund)) {
				node_warning(nd,
					     W_ORDINARY,
					     "might cause range bound error");
				w = 1;
			}
			break;
		case T_ENUMERATION:
		case T_CHAR:
			if (nd->nd_INT < 0 || nd->nd_INT >= tp->enm_ncst) {
				node_warning(nd,
					     W_ORDINARY,
					     "might cause range bound error");
				w = 1;
			}
			break;
		case T_INTORCARD:
		case T_CARDINAL:
		case T_POINTER:
			if ((nd_tp->tp_fund == T_INTEGER && nd->nd_INT < 0) ||
			    (nd->nd_INT & ~full_mask[(int)(tp->tp_size)])) {
				node_warning(nd,
					     W_ORDINARY,
					     "might cause conversion error");
				w = 1;
			}
			break;
		case T_INTEGER:  {
			long i = ~int_mask[(int)(tp->tp_size)];
			long j = nd->nd_INT & i;

			if ((nd_tp->tp_fund == T_INTEGER &&
			     j != i && j != 0) ||
			    (nd_tp->tp_fund != T_INTEGER && j)) {
				node_warning(nd,
					     W_ORDINARY,
					     "might cause conversion error");
				w = 1;
			}
			}
			break;
		}
		if (!w || pass_1) {
			nd->nd_type = tp;
			return;
		}
	}
	*pnd = nd = MkNode(Uoper, NULLNODE, nd, &(nd->nd_token));
	nd->nd_symb = COERCION;
	nd->nd_type = tp;
}

int
ChkVariable(expp)
	register struct node *expp;
{
	/*	Check that "expp" indicates an item that can be
		assigned to.
	*/

	return ChkDesignator(expp) &&
		( expp->nd_class != Def ||
	    	  ( expp->nd_def->df_kind & (D_FIELD|D_VARIABLE)) ||
		  Xerror(expp, "variable expected", expp->nd_def));
}

STATIC int
ChkArrow(expp)
	register struct node *expp;
{
	/*	Check an application of the '^' operator.
		The operand must be a variable of a pointer type.
	*/
	register struct type *tp;

	assert(expp->nd_class == Arrow);
	assert(expp->nd_symb == '^');

	expp->nd_type = error_type;

	if (! ChkVariable(expp->nd_right)) return 0;

	tp = expp->nd_right->nd_type;

	if (tp->tp_fund != T_POINTER) {
		node_error(expp, "\"^\": illegal operand");
		return 0;
	}

	expp->nd_type = RemoveEqual(PointedtoType(tp));
	return 1;
}

STATIC int
ChkArr(expp)
	register struct node *expp;
{
	/*	Check an array selection.
		The left hand side must be a variable of an array type,
		and the right hand side must be an expression that is
		assignment compatible with the array-index.
	*/

	register struct type *tpl;

	assert(expp->nd_class == Arrsel);
	assert(expp->nd_symb == '[');

	expp->nd_type = error_type;

	if (! (ChkVariable(expp->nd_left) & ChkExpression(expp->nd_right))) {
		return 0;
	}

	tpl = expp->nd_left->nd_type;

	if (tpl->tp_fund != T_ARRAY) {
		node_error(expp, "not indexing an ARRAY type");
		return 0;
	}
	expp->nd_type = RemoveEqual(tpl->arr_elem);

	/* Type of the index must be assignment compatible with
	   the index type of the array (Def 8.1).
	   However, the index type of a conformant array is not specified.
	   In our implementation it is CARDINAL.
	*/
	return ChkAssCompat(&(expp->nd_right),
			    BaseType(IndexType(tpl)),
			    "index type");
}

#ifdef DEBUG
STATIC int
ChkValue(expp)
	struct node *expp;
{
	switch(expp->nd_symb) {
	case REAL:
	case STRING:
	case INTEGER:
		return 1;

	default:
		crash("(ChkValue)");
	}
	/*NOTREACHED*/
}
#endif

STATIC int
ChkLinkOrName(expp)
	register struct node *expp;
{
	/*	Check either an ID or a construction of the form
		ID.ID [ .ID ]*
	*/
	register struct def *df;

	expp->nd_type = error_type;

	if (expp->nd_class == Name) {
		expp->nd_def = lookfor(expp, CurrVis, 1);
		expp->nd_class = Def;
		expp->nd_type = RemoveEqual(expp->nd_def->df_type);
	}
	else if (expp->nd_class == Link) {
		/*	A selection from a record or a module.
			Modules also have a record type.
		*/
		register struct node *left = expp->nd_left;

		assert(expp->nd_symb == '.');

		if (! ChkDesignator(left)) return 0;

		if (left->nd_class==Def &&
		    (left->nd_type->tp_fund != T_RECORD ||
		    !(left->nd_def->df_kind & (D_MODULE|D_VARIABLE|D_FIELD))
		    )
		   ) {
			return Xerror(left, "illegal selection", left->nd_def);
		}
		if (left->nd_type->tp_fund != T_RECORD) {
			node_error(left, "illegal selection");
			return 0;
		}

		if (!(df = lookup(expp->nd_IDF, left->nd_type->rec_scope, 1))) {
			id_not_declared(expp);
			return 0;
		}
		expp->nd_def = df;
		expp->nd_type = RemoveEqual(df->df_type);
		expp->nd_class = Def;
		if (!(df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
			/* Fields of a record are always D_QEXPORTED,
			   so ...
			*/
Xerror(expp, "not exported from qualifying module", df);
		}

		if (!(left->nd_class == Def &&
		      left->nd_def->df_kind == D_MODULE)) {
			return 1;
		}
		FreeNode(left);
		expp->nd_left = 0;
	}

	assert(expp->nd_class == Def);

	return expp->nd_def->df_kind != D_ERROR;
}

STATIC int
ChkExLinkOrName(expp)
	register struct node *expp;
{
	/*	Check either an ID or an ID.ID [.ID]* occurring in an
		expression.
	*/
	register struct def *df;

	if (! ChkLinkOrName(expp)) return 0;

	df = expp->nd_def;

	if (df->df_kind & (D_ENUM | D_CONST)) {
		/* Replace an enum-literal or a CONST identifier by its value.
		*/
		if (df->df_type->tp_fund == T_SET) {
			expp->nd_class = Set;
		}
		else	expp->nd_class = Value;
		if (df->df_kind == D_ENUM) {
			expp->nd_INT = df->enm_val;
			expp->nd_symb = INTEGER;
		}
		else  {
			unsigned int ln = expp->nd_lineno;

			assert(df->df_kind == D_CONST);
			expp->nd_token = df->con_const;
			expp->nd_lineno = ln;
		}
	}

	if (!(df->df_kind & D_VALUE)) {
		Xerror(expp, "value expected", df);
	}

	if (df->df_kind == D_PROCEDURE) {
		/* Check that this procedure is one that we may take the
		   address from.
		*/
		if (df->df_type == std_type || df->df_scope->sc_level > 0) {
			/* Address of standard or nested procedure
			   taken.
			*/
node_error(expp, "standard or local procedures may not be assigned");
			return 0;
		}
	}

	return 1;
}

STATIC int
ChkEl(expr, tp)
	register struct node **expr;
	struct type *tp;
{

	return ChkExpression(*expr) && ChkCompat(expr, tp, "set element");
}

STATIC int
ChkElement(expp, tp, set)
	struct node **expp;
	struct type *tp;
	arith *set;
{
	/*	Check elements of a set. This routine may call itself
		recursively.
		Also try to compute the set!
	*/
	register struct node *expr = *expp;
	register unsigned int i;
	arith lo, hi, low, high;

	if (expr->nd_class == Link && expr->nd_symb == UPTO) {
		/* { ... , expr1 .. expr2,  ... }
		   First check expr1 and expr2, and try to compute them.
		*/
		if (! (ChkEl(&(expr->nd_left), tp) & 
		       ChkEl(&(expr->nd_right), tp))) {
			return 0;
		}

		if (!(expr->nd_left->nd_class == Value &&
		      expr->nd_right->nd_class == Value)) {
			return 1;
		}
		/* We have a constant range. Put all elements in the
		  set
		*/

		low = expr->nd_left->nd_INT;
		high = expr->nd_right->nd_INT;
	}
	else {
		if (! ChkEl(expp, tp)) return 0;
		expr = *expp;
		if (expr->nd_class != Value) {
			return 1;
		}
		low = high = expr->nd_INT;
	}
	if (low > high) {
		node_error(expr, "lower bound exceeds upper bound in range");
		return 0;
	}

	getbounds(tp, &lo, &hi);
	if (low < lo || high > hi) {
		node_error(expr, "set element out of range");
		return 0;
	}

	for (i=(unsigned)low; i<= (unsigned)high; i++) {
		set[i/wrd_bits] |= (1<<(i%wrd_bits));
	}
	FreeNode(expr);
	*expp = 0;
	return 1;
}

STATIC int
ChkSet(expp)
	register struct node *expp;
{
	/*	Check the legality of a SET aggregate, and try to evaluate it
		compile time. Unfortunately this is all rather complicated.
	*/
	register struct type *tp;
	register struct node *nd;
	register struct def *df;
	unsigned size;
	int retval = 1;
	int SetIsConstant = 1;

	assert(expp->nd_symb == SET);

	expp->nd_type = error_type;
	expp->nd_class = Set;

	/* First determine the type of the set
	*/
	if (nd = expp->nd_left) {
		/* A type was given. Check it out
		*/
		if (! ChkDesignator(nd)) return 0;
		assert(nd->nd_class == Def);
		df = nd->nd_def;

		if (!is_type(df) ||
	   	    (df->df_type->tp_fund != T_SET)) {
			if (df->df_kind != D_ERROR) {
				Xerror(nd, "not a SET type", df);
			}
			return 0;
		}
		tp = df->df_type;
		FreeNode(nd);
		expp->nd_left = 0;
	}
	else	tp = bitset_type;
	expp->nd_type = tp;

	nd = expp->nd_right;

	/* Now check the elements given, and try to compute a constant set.
	   First allocate room for the set.
	*/
	size = tp->tp_size * (sizeof(arith) / word_size);
	expp->nd_set = (arith *) Malloc(size);
	clear((char *) (expp->nd_set) , size);

	/* Now check the elements, one by one
	*/
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		if (!ChkElement(&(nd->nd_left), ElementType(tp),
						expp->nd_set)) {
			retval = 0;
		}
		if (nd->nd_left) SetIsConstant = 0;
		nd = nd->nd_right;
	}

	if (SetIsConstant) {
		FreeNode(expp->nd_right);
		expp->nd_right = 0;
	}
	return retval;
}

STATIC struct node *
nextarg(argp, edf)
	struct node **argp;
	struct def *edf;
{
	register struct node *arg = (*argp)->nd_right;

	if (! arg) {
		return (struct node *)Xerror(*argp, "too few arguments supplied", edf);
	}

	*argp = arg;
	return arg->nd_left;
}

STATIC struct node *
getarg(argp, bases, designator, edf)
	struct node **argp;
	struct def *edf;
{
	/*	This routine is used to fetch the next argument from an
		argument list. The argument list is indicated by "argp".
		The parameter "bases" is a bitset indicating which types
		are allowed at this point, and "designator" is a flag
		indicating that the address from this argument is taken, so
		that it must be a designator and may not be a register
		variable.
	*/
	register struct node *left = nextarg(argp, edf);

	if (!left || (designator ? !ChkVariable(left) : !ChkExpression(left))) {
		return 0;
	}

	if (designator && left->nd_class==Def) {
		left->nd_def->df_flags |= D_NOREG;
	}

	if (bases) {
		struct type *tp = BaseType(left->nd_type);

		MkCoercion(&((*argp)->nd_left), tp);
		left = (*argp)->nd_left;
		if (!(tp->tp_fund & bases)) {
			return (struct node *)Xerror(left, "unexpected parameter type", edf);
		}
	}

	return left;
}

STATIC struct node *
getname(argp, kinds, bases, edf)
	struct node **argp;
	struct def *edf;
{
	/*	Get the next argument from argument list "argp".
		The argument must indicate a definition, and the
		definition kind must be one of "kinds".
	*/
	register struct node *left = nextarg(argp, edf);

	if (!left || ! ChkDesignator(left)) return 0;

	if (left->nd_class != Def) {
		return (struct node *)Xerror(left, "identifier expected", edf);
	}

	if (!(left->nd_def->df_kind & kinds) ||
	    (bases && !(left->nd_type->tp_fund & bases))) {
		return (struct node *)Xerror(left, "unexpected parameter type", edf);
	}

	return left;
}

STATIC int
ChkProcCall(expp)
	struct node *expp;
{
	/*	Check a procedure call
	*/
	register struct node *left;
	struct def *edf = 0;
	register struct paramlist *param;
	int retval = 1;
	int cnt = 0;

	left = expp->nd_left;
	if (left->nd_class == Def) {
		edf = left->nd_def;
	}
	if (left->nd_type == error_type) {
		/* Just check parameters as if they were value parameters
		*/
		while (expp->nd_right) {
			getarg(&expp, 0, 0, edf);
		}
		return 0;
	}

	expp->nd_type = RemoveEqual(ResultType(left->nd_type));

	/* Check parameter list
	*/
	for (param = ParamList(left->nd_type); param; param = param->par_next) {
		if (!(left = getarg(&expp, 0, IsVarParam(param), edf))) {
			return 0;
		}
		cnt++;
		if (left->nd_symb == STRING) {
			TryToString(left, TypeOfParam(param));
		}
		if (! TstParCompat(cnt,
				   RemoveEqual(TypeOfParam(param)),
				   IsVarParam(param),
				   &(expp->nd_left),
				   edf)) {
			retval = 0;
		}
	}

	if (expp->nd_right) {
		Xerror(expp->nd_right, "too many parameters supplied", edf);
		while (expp->nd_right) {
			getarg(&expp, 0, 0, edf);
		}
		return 0;
	}

	return retval;
}

int
ChkFunCall(expp)
	register struct node *expp;
{
	/*	Check a call that must have a result
	*/

	if (! ChkCall(expp)) return 0;

	if (expp->nd_type == 0) {
		node_error(expp, "function call expected");
		expp->nd_type = error_type;
		return 0;
	}
	return 1;
}

int
ChkCall(expp)
	register struct node *expp;
{
	/*	Check something that looks like a procedure or function call.
		Of course this does not have to be a call at all,
		it may also be a cast or a standard procedure call.
	*/
	register struct node *left = expp->nd_left;
	STATIC int ChkStandard();
	STATIC int ChkCast();

	/* First, get the name of the function or procedure
	*/
	if (ChkDesignator(left)) {
		if (IsCast(left)) {
			/* It was a type cast.
			*/
			return ChkCast(expp);
		}

		if (IsProcCall(left) || left->nd_type == error_type) {
			/* A procedure call.
		   	   It may also be a call to a standard procedure
			*/
			if (left->nd_type == std_type) {
				/* A standard procedure
				*/
				return ChkStandard(expp);
			}
			/* Here, we have found a real procedure call. 
			   The left hand side may also represent a procedure
			   variable.
			*/
		}
		else node_error(left, "procedure, type, or function expected");
	}
	return ChkProcCall(expp);
}

STATIC struct type *
ResultOfOperation(operator, tp)
	struct type *tp;
{
	/*	Return the result type of the binary operation "operator",
		with operand type "tp".
	*/

	switch(operator) {
	case '=':
	case '#':
	case GREATEREQUAL:
	case LESSEQUAL:
	case '<':
	case '>':
	case IN:
		return bool_type;
	}

	return tp;
}

#define Boolean(operator) (operator == OR || operator == AND)

STATIC int
AllowedTypes(operator)
{
	/*	Return a bit mask indicating the allowed operand types
		for binary operator "operator".
	*/

	switch(operator) {
	case '+':
	case '-':
	case '*':
		return T_NUMERIC|T_SET;
	case '/':
		return T_REAL|T_SET;
	case DIV:
	case MOD:
		return T_INTORCARD;
	case OR:
	case AND:
		return T_ENUMERATION;
	case '=':
	case '#':
		return T_POINTER|T_HIDDEN|T_SET|T_NUMERIC|T_ENUMERATION|T_CHAR;
	case GREATEREQUAL:
	case LESSEQUAL:
		return T_SET|T_NUMERIC|T_CHAR|T_ENUMERATION;
	case '<':
	case '>':
		return T_NUMERIC|T_CHAR|T_ENUMERATION;
	default:
		crash("(AllowedTypes)");
	}
	/*NOTREACHED*/
}

STATIC int
ChkAddress(tpl, tpr)
	register struct type *tpl, *tpr;
{
	/*	Check that either "tpl" or "tpr" are both of type
		address_type, or that one of them is, but the other is
		of type cardinal.
	*/
	
	if (tpl == address_type) {
		return tpr == address_type || (tpr->tp_fund & T_CARDINAL);
	}

	if (tpr == address_type) {
		return (tpl->tp_fund & T_CARDINAL);
	}

	return 0;
}

STATIC int
ChkBinOper(expp)
	register struct node *expp;
{
	/*	Check a binary operation.
	*/
	register struct node *left, *right;
	register struct type *tpl, *tpr;
	int allowed;
	int retval;

	left = expp->nd_left;
	right = expp->nd_right;

	retval = ChkExpression(left) & ChkExpression(right);

	tpl = BaseType(left->nd_type);
	tpr = BaseType(right->nd_type);

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

	expp->nd_type = ResultOfOperation(expp->nd_symb, tpr);

	/* Check that the application of the operator is allowed on the type
	   of the operands.
	   There are three tricky parts:
	   - Boolean operators are only allowed on boolean operands, but
	     the "allowed-mask" of "AllowedTypes" can only indicate
	     an enumeration type.
	   - All operations that are allowed on CARDINALS are also allowed
	     on ADDRESS.
	   - The IN-operator has as right-hand-size operand a set.
	*/
	if (expp->nd_symb == IN) {
		if (tpr->tp_fund != T_SET) {
			node_error(expp, "\"IN\": right operand must be a set");
			return 0;
		}
		if (!TstAssCompat(ElementType(tpr), tpl)) {
			/* Assignment compatible ???
			   I don't know! Should we be allowed to check
			   if a INTEGER is a member of a BITSET???
			*/
			node_error(left, "type incompatibility in IN");
			return 0;
		}
		MkCoercion(&(expp->nd_left), word_type);
		left = expp->nd_left;
		if (left->nd_class == Value && right->nd_class == Set) {
			cstset(expp);
		}
		return retval;
	}

	if (!retval) return 0;

	allowed = AllowedTypes(expp->nd_symb);

	if (!(tpr->tp_fund & allowed) || !(tpl->tp_fund & allowed)) {
	    	if (!((T_CARDINAL & allowed) &&
	             ChkAddress(tpl, tpr))) {
			node_error(expp, "\"%s\": illegal operand type(s)", 
				     symbol2str(expp->nd_symb));
			return 0;
		}
		if (expp->nd_type->tp_fund & T_CARDINAL) {
			expp->nd_type = address_type;
		}
	}

	if (Boolean(expp->nd_symb) && tpl != bool_type) {
		node_error(expp, "\"%s\": illegal operand type(s)",
			     symbol2str(expp->nd_symb));
		return 0;
	}

	/* Operands must be compatible (distilled from Def 8.2)
	*/
	if (!TstCompat(tpr, tpl)) {
		node_error(expp,"\"%s\": incompatible types",
			   symbol2str(expp->nd_symb));
		return 0;
	}

	MkCoercion(&(expp->nd_left), tpl);
	MkCoercion(&(expp->nd_right), tpr);

	if (tpl->tp_fund == T_SET) {
	    	if (left->nd_class == Set && right->nd_class == Set) {
			cstset(expp);
		}
	}
	else if ( tpl->tp_fund != T_REAL &&
		  left->nd_class == Value && right->nd_class == Value) {
		cstbin(expp);
	}

	return 1;
}

STATIC int
ChkUnOper(expp)
	register struct node *expp;
{
	/*	Check an unary operation.
	*/
	register struct node *right = expp->nd_right;
	register struct type *tpr;

	if (! ChkExpression(right)) return 0;
	expp->nd_type = tpr = BaseType(right->nd_type);
	MkCoercion(&(expp->nd_right), tpr);
	right = expp->nd_right;

	if (tpr == address_type) tpr = card_type;

	switch(expp->nd_symb) {
	case '+':
		if (!(tpr->tp_fund & T_NUMERIC)) break;
		/* fall through */

	case '(':
		*expp = *right;
		free_node(right);
		return 1;

	case '-':
		if (tpr->tp_fund & T_INTORCARD) {
			if (tpr == intorcard_type || tpr == card_type) {
				expp->nd_type = int_type;
			}
			if (right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		else if (tpr->tp_fund == T_REAL) {
			if (right->nd_class == Value) {
				if (*(right->nd_REL) == '-') (right->nd_REL)++;
				else (right->nd_REL)--;
				expp->nd_class = Value;
				expp->nd_symb = REAL;
				expp->nd_REL = right->nd_REL;
				FreeNode(right);
				expp->nd_right = 0;
			}
			return 1;
		}
		break;

	case NOT:
		if (tpr == bool_type) {
			if (right->nd_class == Value) {
				cstunary(expp);
			}
			return 1;
		}
		break;

	default:
		crash("ChkUnOper");
	}
	node_error(expp, "\"%s\": illegal operand", symbol2str(expp->nd_symb));
	return 0;
}

STATIC struct node *
getvariable(argp, edf)
	struct node **argp;
	struct def *edf;
{
	/*	Get the next argument from argument list "argp".
		It must obey the rules of "ChkVariable".
	*/
	register struct node *left = nextarg(argp, edf);

	if (!left || !ChkVariable(left)) return 0;

	return left;
}

STATIC int
ChkStandard(expp)
	register struct node *expp;
{
	/*	Check a call of a standard procedure or function
	*/
	struct node *arg = expp;
	register struct node *left = expp->nd_left;
	register struct def *edf = left->nd_def;
	int free_it = 0;

	assert(left->nd_class == Def);

	expp->nd_type = error_type;
	switch(edf->df_value.df_stdname) {
	case S_ABS:
		if (!(left = getarg(&arg, T_NUMERIC, 0, edf))) return 0;
		MkCoercion(&(arg->nd_left), BaseType(left->nd_type));
		left = arg->nd_left;
		expp->nd_type = left->nd_type;
		if (left->nd_class == Value &&
		    expp->nd_type->tp_fund != T_REAL) {
			cstcall(expp, S_ABS);
		}
		break;

	case S_CAP:
		expp->nd_type = char_type;
		if (!(left = getarg(&arg, T_CHAR, 0, edf))) return 0;
		if (left->nd_class == Value) cstcall(expp, S_CAP);
		break;

	case S_CHR:
		expp->nd_type = char_type;
		if (!(left = getarg(&arg, T_INTORCARD, 0, edf))) return 0;
		MkCoercion(&(arg->nd_left), char_type);
		free_it = 1;
		break;

	case S_FLOATD:
	case S_FLOAT:
		if (! getarg(&arg, T_INTORCARD, 0, edf)) return 0;
		if (edf->df_value.df_stdname == S_FLOAT) {
			MkCoercion(&(arg->nd_left), card_type);
		}
		MkCoercion(&(arg->nd_left),
			   edf->df_value.df_stdname == S_FLOATD ?
				longreal_type :
				real_type);
		free_it = 1;
		break;

	case S_SHORT:
	case S_LONG: {
		struct type *tp;
		struct type *s1, *s2, *d1, *d2;

		if (edf->df_value.df_stdname == S_SHORT) {
			s1 = longint_type;
			d1 = int_type;
			s2 = longreal_type;
			d2 = real_type;
		}
		else {
			d1 = longint_type;
			s1 = int_type;
			d2 = longreal_type;
			s2 = real_type;
		}

		if (!(left = getarg(&arg, 0, 0, edf))) {
			return 0;
		}
		tp = BaseType(left->nd_type);
		if (tp == s1) {
			MkCoercion(&(arg->nd_left), d1);
		}
		else if (tp == s2) {
			MkCoercion(&(arg->nd_left), d2);
		}
		else {
			Xerror(left, "unexpected parameter type", edf);
			break;
		}
		free_it = 1;
		break;
		}

	case S_HIGH:
		if (!(left = getarg(&arg, T_ARRAY|T_STRING|T_CHAR, 0, edf))) {
			return 0;
		}
		if (left->nd_type->tp_fund == T_ARRAY) {
			expp->nd_type = IndexType(left->nd_type);
			if (! IsConformantArray(left->nd_type)) {
				cstcall(expp, S_MAX);
			}
			break;
		}
		if (left->nd_symb != STRING) {
			return Xerror(left,"array parameter expected", edf);
		}
		expp->nd_type = card_type;
		expp->nd_class = Value;
		/* Notice that we could disallow HIGH("") here by checking
		   that left->nd_type->tp_fund != T_CHAR || left->nd_INT != 0.
		   ??? For the time being, we don't. !!!
		   Maybe the empty string should not be allowed at all.
		*/
		expp->nd_INT = left->nd_type->tp_fund == T_CHAR ? 0 :
					left->nd_SLE - 1;
		expp->nd_symb = INTEGER;
		break;

	case S_MAX:
	case S_MIN:
		if (!(left = getname(&arg, D_ISTYPE, T_DISCRETE, edf))) {
			return 0;
		}
		expp->nd_type = left->nd_type;
		cstcall(expp,edf->df_value.df_stdname);
		break;

	case S_ODD:
		if (! (left = getarg(&arg, T_INTORCARD, 0, edf))) return 0;
		MkCoercion(&(arg->nd_left), BaseType(left->nd_type));
		expp->nd_type = bool_type;
		if (arg->nd_left->nd_class == Value) cstcall(expp, S_ODD);
		break;

	case S_ORD:
		if (! getarg(&arg, T_DISCRETE, 0, edf)) return 0;
		MkCoercion(&(arg->nd_left), card_type);
		free_it = 1;
		break;

	case S_NEW:
	case S_DISPOSE:
		{
			static int warning_given = 0;

			if (!warning_given) {
				warning_given = 1;
	node_warning(expp, W_OLDFASHIONED, "NEW and DISPOSE are obsolete");
			}
		}
		expp->nd_type = 0;
		if (! (left = getvariable(&arg, edf))) return 0;
		if (! (left->nd_type->tp_fund == T_POINTER)) {
			return Xerror(left, "pointer variable expected", edf);
		}
		/* Now, make it look like a call to ALLOCATE or DEALLOCATE */
		{
			struct token dt;
			struct node *nd;

			dt.TOK_INT = PointedtoType(left->nd_type)->tp_size;
			dt.tk_symb = INTEGER;
			dt.tk_lineno = left->nd_lineno;
			nd = MkLeaf(Value, &dt);
			nd->nd_type = card_type;
			dt.tk_symb = ',';
			arg->nd_right = MkNode(Link, nd, NULLNODE, &dt);
			/* Ignore other arguments to NEW and/or DISPOSE ??? */

			FreeNode(expp->nd_left);
			dt.tk_symb = IDENT;
			dt.tk_lineno = expp->nd_left->nd_lineno;
			dt.TOK_IDF = str2idf(edf->df_value.df_stdname==S_NEW ?
						"ALLOCATE" : "DEALLOCATE", 0);
			expp->nd_left = MkLeaf(Name, &dt);
		}
		return ChkCall(expp);

	case S_TSIZE:	/* ??? */
	case S_SIZE:
		expp->nd_type = intorcard_type;
		if (!(left = getname(&arg,D_FIELD|D_VARIABLE|D_ISTYPE,0,edf))) {
			return 0;
		}
		if (! IsConformantArray(left->nd_type)) cstcall(expp, S_SIZE);
		else node_warning(expp,
				  W_STRICT,
				  "%s on conformant array",
				  expp->nd_left->nd_def->df_idf->id_text);
		break;

	case S_TRUNCD:
	case S_TRUNC:
		expp->nd_type = card_type;
		if (edf->df_value.df_stdname == S_TRUNCD) {
			expp->nd_type = longint_type;
		}
		if (! getarg(&arg, T_REAL, 0, edf)) return 0;
		MkCoercion(&(arg->nd_left), expp->nd_type);
		free_it = 1;
		break;

	case S_VAL:
		if (!(left = getname(&arg, D_ISTYPE, T_DISCRETE, edf))) {
			return 0;
		}
		expp->nd_type = left->nd_def->df_type;
		expp->nd_right = arg->nd_right;
		arg->nd_right = 0;
		FreeNode(arg);
		arg = expp;
		if (!(left = getarg(&arg, T_INTORCARD, 0, edf))) return 0;
		MkCoercion(&(arg->nd_left), expp->nd_type);
		free_it = 1;
		break;

	case S_ADR:
		expp->nd_type = address_type;
		if (! getarg(&arg, 0, 1, edf)) return 0;
		break;

	case S_DEC:
	case S_INC:
		expp->nd_type = 0;
		if (! (left = getvariable(&arg, edf))) return 0;
		if (! (left->nd_type->tp_fund & T_DISCRETE)) {
			return Xerror(left,"illegal parameter type", edf);
		}
		if (arg->nd_right) {
			if (! getarg(&arg, T_INTORCARD, 0, edf)) return 0;
		}
		break;

	case S_HALT:
		expp->nd_type = 0;
		break;

	case S_EXCL:
	case S_INCL:
		{
		register struct type *tp;
		struct node *dummy;

		expp->nd_type = 0;
		if (!(left = getvariable(&arg, edf))) return 0;
		tp = left->nd_type;
		if (tp->tp_fund != T_SET) {
			return Xerror(arg, "SET parameter expected", edf);
		}
		if (!(dummy = getarg(&arg, 0, 0, edf))) return 0;
		if (!ChkAssCompat(&dummy, ElementType(tp), "EXCL/INCL")) {
			/* What type of compatibility do we want here?
			   apparently assignment compatibility! ??? ???
			   But we don't want the coercion in the tree, because
			   we don't want a range check here. We want a SET
			   error.
			*/
			return 0;
		}
		MkCoercion(&(arg->nd_left), word_type);
		break;
		}

	default:
		crash("(ChkStandard)");
	}

	if (arg->nd_right) {
		return Xerror(arg->nd_right, "too many parameters supplied", edf);
	}

	if (free_it) {
		FreeNode(expp->nd_left);
		*expp = *(arg->nd_left);
		arg->nd_left = 0;
		FreeNode(arg);
	}

	return 1;
}

STATIC int
ChkCast(expp)
	register struct node *expp;
{
	/*	Check a cast and perform it if the argument is constant.
		If the sizes don't match, only complain if at least one of them
		has a size larger than the word size.
		If both sizes are equal to or smaller than the word size, there
		is no problem as such values take a word on the EM stack
		anyway.
	*/
	register struct node *left = expp->nd_left;
	register struct node *arg = expp->nd_right;
	register struct type *lefttype = left->nd_type;

	if ((! arg) || arg->nd_right) {
		return Xerror(expp, "type cast must have 1 parameter", left->nd_def);
	}

	if (! ChkExpression(arg->nd_left)) return 0;

	MkCoercion(&(arg->nd_left), BaseType(arg->nd_left->nd_type));

	arg = arg->nd_left;
	if (arg->nd_type->tp_size != lefttype->tp_size &&
	    (arg->nd_type->tp_size > word_size ||
	     lefttype->tp_size > word_size)) {
		Xerror(expp, "unequal sizes in type cast", left->nd_def);
	}

	if (arg->nd_class == Value) {
		FreeNode(left);
		expp->nd_right->nd_left = 0;
		FreeNode(expp->nd_right);
		*expp = *arg;
	}
	expp->nd_type = lefttype;

	return 1;
}

TryToString(nd, tp)
	register struct node *nd;
	struct type *tp;
{
	/*	Try a coercion from character constant to string.
	*/
	static char buf[2];

	assert(nd->nd_symb == STRING);

	if (tp->tp_fund == T_ARRAY && nd->nd_type == char_type) {
		buf[0] = nd->nd_INT;
		nd->nd_type = standard_type(T_STRING, 1, (arith) 2);
		nd->nd_token.tk_data.tk_str = 
			(struct string *) Malloc(sizeof(struct string));
		nd->nd_STR = Salloc(buf, 2);
		nd->nd_SLE = 1;
	}
}

STATIC int
no_desig(expp)
	struct node *expp;
{
	node_error(expp, "designator expected");
	return 0;
}

STATIC int
done_before()
{
	return 1;
}

extern int	NodeCrash();

int (*ExprChkTable[])() = {
#ifdef DEBUG
	ChkValue,
#else
	done_before,
#endif
	ChkArr,
	ChkBinOper,
	ChkUnOper,
	ChkArrow,
	ChkFunCall,
	ChkExLinkOrName,
	NodeCrash,
	ChkSet,
	NodeCrash,
	NodeCrash,
	ChkExLinkOrName,
	NodeCrash
};

int (*DesigChkTable[])() = {
	no_desig,
	ChkArr,
	no_desig,
	no_desig,
	ChkArrow,
	no_desig,
	ChkLinkOrName,
	NodeCrash,
	no_desig,
	done_before,
	NodeCrash,
	ChkLinkOrName,
	NodeCrash
};
