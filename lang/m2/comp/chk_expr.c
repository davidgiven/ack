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
#include	"def.h"
#include	"LLlex.h"
#include	"node.h"
#include	"scope.h"
#include	"const.h"
#include	"standards.h"
#include	"chk_expr.h"
#include	"misc.h"
#include	"warning.h"

extern char *symbol2str();
extern char *sprint();

STATIC
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
	else	node_error(nd, "%s", mess);
}

int
ChkVariable(expp)
	register struct node *expp;
{
	/*	Check that "expp" indicates an item that can be
		assigned to.
	*/

	if (! ChkDesignator(expp)) return 0;

	if (expp->nd_class == Def &&
	    !(expp->nd_def->df_kind & (D_FIELD|D_VARIABLE))) {
		Xerror(expp, "variable expected", expp->nd_def);
		return 0;
	}
	if (expp->nd_class == Value) {
		node_error(expp, "variable expected");
		return 0;
	}

	return 1;
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

	register struct type *tpl, *tpr;
	int retval;

	assert(expp->nd_class == Arrsel);
	assert(expp->nd_symb == '[');

	expp->nd_type = error_type;

	retval = ChkVariable(expp->nd_left) & ChkExpression(expp->nd_right);

	tpl = expp->nd_left->nd_type;
	tpr = expp->nd_right->nd_type;
	if (tpl == error_type || tpr == error_type) return 0;

	if (tpl->tp_fund != T_ARRAY) {
		node_error(expp, "not indexing an ARRAY type");
		return 0;
	}

	/* Type of the index must be assignment compatible with
	   the index type of the array (Def 8.1).
	   However, the index type of a conformant array is not specified.
	   Either INTEGER or CARDINAL seems reasonable.
	*/
	if (IsConformantArray(tpl) ? !TstAssCompat(card_type, tpr)
				   : !TstAssCompat(IndexType(tpl), tpr)) {
		node_error(expp, "incompatible index type");
		return 0;
	}

	expp->nd_type = RemoveEqual(tpl->arr_elem);
	return retval;
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

		if (left->nd_class == Def &&
		    (left->nd_type->tp_fund != T_RECORD ||
		    !(left->nd_def->df_kind & (D_MODULE|D_VARIABLE|D_FIELD))
		    )
		   ) {
			Xerror(left, "illegal selection", left->nd_def);
			return 0;
		}
		if (left->nd_type->tp_fund != T_RECORD) {
			node_error(left, "illegal selection");
			return 0;
		}

		if (!(df = lookup(expp->nd_IDF, left->nd_type->rec_scope, 1))) {
			id_not_declared(expp);
			return 0;
		}
		else {
			expp->nd_def = df;
			expp->nd_type = RemoveEqual(df->df_type);
			expp->nd_class = LinkDef;
			if (!(df->df_flags & (D_EXPORTED|D_QEXPORTED))) {
				/* Fields of a record are always D_QEXPORTED,
				   so ...
				*/
Xerror(expp, "not exported from qualifying module", df);
			}
		}

		if (left->nd_class == Def &&
		    left->nd_def->df_kind == D_MODULE) {
			expp->nd_class = Def;
			FreeNode(left);
			expp->nd_left = 0;
		}
		else	return 1;
	}

	assert(expp->nd_class == Def);

	df = expp->nd_def;

	if (df->df_kind & (D_ENUM | D_CONST)) {
		/* Replace an enum-literal or a CONST identifier by its value.
		*/
		if (df->df_kind == D_ENUM) {
			expp->nd_class = Value;
			expp->nd_INT = df->enm_val;
			expp->nd_symb = INTEGER;
		}
		else  {
			unsigned int ln = expp->nd_lineno;

			assert(df->df_kind == D_CONST);
			*expp = *(df->con_const);
			expp->nd_lineno = ln;
		}
	}
	return df->df_kind != D_ERROR;
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
	if (expp->nd_class != Def) return 1;
	df = expp->nd_def;

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
ChkElement(expp, tp, set, level)
	struct node **expp;
	register struct type *tp;
	arith **set;
{
	/*	Check elements of a set. This routine may call itself
		recursively.
		Also try to compute the set!
	*/
	register struct node *expr = *expp;
	register struct node *left = expr->nd_left;
	register struct node *right = expr->nd_right;
	register arith i;

	if (expr->nd_class == Link && expr->nd_symb == UPTO) {
		/* { ... , expr1 .. expr2,  ... }
		   First check expr1 and expr2, and try to compute them.
		*/
		if (!ChkElement(&(expr->nd_left), tp, set, 1) ||
		    !ChkElement(&(expr->nd_right), tp, set, 1)) {
			return 0;
		}

		if (left->nd_class == Value && right->nd_class == Value) {
			/* We have a constant range. Put all elements in the
			   set
			*/

		    	if (left->nd_INT > right->nd_INT) {
node_error(expr, "lower bound exceeds upper bound in range");
				return 0;
			}

			for (i=left->nd_INT; i<=right->nd_INT; i++) {
				(*set)[i/wrd_bits] |= (1<<(i%wrd_bits));
			}
			FreeNode(expr);
			*expp = 0;
		}

		return 1;
	}

	/* Here, a single element is checked
	*/
	if (!ChkExpression(expr)) return 0;

	if (!TstCompat(tp, expr->nd_type)) {
		node_error(expr, "set element has incompatible type");
		return 0;
	}

	if (expr->nd_class == Value) {
		/* a constant element
		*/
		arith low, high;

		i = expr->nd_INT;
		getbounds(tp, &low, &high);

	    	if (i < low || i > high) {
			node_error(expr, "set element out of range");
			return 0;
		}

		if (! level) {
			(*set)[i/wrd_bits] |= (1 << (i%wrd_bits));
			FreeNode(expr);
			*expp = 0;
		}
	}

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

	assert(expp->nd_symb == SET);

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
				Xerror(nd, "not a set type", df);
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
						&(expp->nd_set), 0)) {
			retval = 0;
		}
		if (nd->nd_left) expp->nd_class = Xset;
		nd = nd->nd_right;
	}

	if (expp->nd_class == Set) {
		FreeNode(expp->nd_right);
		expp->nd_right = 0;
	}
	return retval;
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
	register struct node *arg = (*argp)->nd_right;
	register struct node *left;

	if (! arg) {
		Xerror(*argp, "too few arguments supplied", edf);
		return 0;
	}

	left = arg->nd_left;
	*argp = arg;

	if (designator ? !ChkVariable(left) : !ChkExpression(left)) {
		return 0;
	}

	if (designator && left->nd_class == Def) {
		left->nd_def->df_flags |= D_NOREG;
	}

	if (bases) {
		if (!(BaseType(left->nd_type)->tp_fund & bases)) {
			Xerror(arg, "unexpected parameter type", edf);
			return 0;
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
	register struct node *arg = *argp;
	register struct node *left;

	*argp = arg->nd_right;

	if (!arg->nd_right) {
		Xerror(arg, "too few arguments supplied", edf);
		return 0;
	}

	arg = arg->nd_right;
	left = arg->nd_left;
	if (! ChkDesignator(left)) return 0;

	if (left->nd_class != Def && left->nd_class != LinkDef) {
		Xerror(arg, "identifier expected", edf);
		return 0;
	}

	if (!(left->nd_def->df_kind & kinds)) {
		Xerror(arg, "unexpected parameter type", edf);
		return 0;
	}

	if (bases) {
		if (!(left->nd_type->tp_fund & bases)) {
			Xerror(arg, "unexpected parameter type", edf);
			return 0;
		}
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
	char ebuf[256];
	int retval = 1;
	int cnt = 0;

	left = expp->nd_left;
	if (left->nd_class == Def || left->nd_class == LinkDef) {
		edf = left->nd_def;
	}
	if (left->nd_type == error_type) {
		/* Just check parameters as if they were value parameters
		*/
		expp->nd_type = error_type;
		while (expp->nd_right) {
			getarg(&expp, 0, 0, edf);
		}
		return 0;
	}

	expp->nd_type = RemoveEqual(ResultType(left->nd_type));

	/* Check parameter list
	*/
	for (param = ParamList(left->nd_type); param; param = param->next) {
		if (!(left = getarg(&expp, 0, IsVarParam(param), edf))) {
			return 0;
		}
		cnt++;
		if (left->nd_symb == STRING) {
			TryToString(left, TypeOfParam(param));
		}
		if (! TstParCompat(RemoveEqual(TypeOfParam(param)),
				   left->nd_type,
				   IsVarParam(param),
				   left)) {
			sprint(ebuf, "type incompatibility in parameter %d",
					cnt);
			Xerror(left, ebuf, edf);
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
	int retval = 1;

	if (!ChkCall(expp)) retval = 0;
	if (expp->nd_type == 0) {
		node_error(expp, "function call expected");
		expp->nd_type = error_type;
		retval = 0;
	}
	return retval;
}

int
ChkCall(expp)
	register struct node *expp;
{
	/*	Check something that looks like a procedure or function call.
		Of course this does not have to be a call at all,
		it may also be a cast or a standard procedure call.
	*/
	register struct node *left;
	STATIC int ChkStandard();
	STATIC int ChkCast();

	/* First, get the name of the function or procedure
	*/
	expp->nd_type = error_type;
	left = expp->nd_left;
	if (ChkDesignator(left)) {
		if (IsCast(left)) {
			/* It was a type cast.
			*/
			return ChkCast(expp, left);
		}

		if (IsProcCall(left) || left->nd_type == error_type) {
			/* A procedure call.
		   	   It may also be a call to a standard procedure
			*/
			if (left->nd_type == std_type) {
				/* A standard procedure
				*/
				return ChkStandard(expp, left);
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

STATIC int
Boolean(operator)
{
	return operator == OR || operator == AND || operator == '&';
}

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
	case '&':
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
	struct type *tpl, *tpr;
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
		if (!TstAssCompat(tpl, ElementType(tpr))) {
			/* Assignment compatible ???
			   I don't know! Should we be allowed to check
			   if a INTEGER is a member of a BITSET???
			*/

			node_error(expp, "\"IN\": incompatible types");
			return 0;
		}
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
	if (!TstCompat(tpl, tpr)) {
		node_error(expp, "\"%s\": incompatible types", symbol2str(expp->nd_symb));
		return 0;
	}

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
	if (tpr == address_type) tpr = card_type;

	switch(expp->nd_symb) {
	case '+':
		if (tpr->tp_fund & T_NUMERIC) {
			*expp = *right;
			free_node(right);
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
			expp->nd_type = tpr;
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
	case '~':
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
	register struct node *arg = *argp;

	arg = arg->nd_right;
	if (!arg) {
		Xerror(arg, "too few parameters supplied", edf);
		return 0;
	}

	*argp = arg;
	arg = arg->nd_left;
	if (! ChkVariable(arg)) return 0;

	return arg;
}

STATIC int
ChkStandard(expp, left)
	register struct node *expp, *left;
{
	/*	Check a call of a standard procedure or function
	*/
	struct node *arg = expp;
	register struct def *edf;
	int std;

	assert(left->nd_class == Def);
	std = left->nd_def->df_value.df_stdname;
	edf = left->nd_def;

	switch(std) {
	case S_ABS:
		if (!(left = getarg(&arg, T_NUMERIC, 0, edf))) return 0;
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
		if (left->nd_class == Value) cstcall(expp, S_CHR);
		break;

	case S_FLOAT:
		expp->nd_type = real_type;
		if (!(left = getarg(&arg, T_INTORCARD, 0, edf))) return 0;
		break;

	case S_HIGH:
		if (!(left = getarg(&arg, T_ARRAY|T_STRING|T_CHAR, 0, edf))) {
			return 0;
		}
		if (IsConformantArray(left->nd_type)) {
			/* A conformant array has no explicit index type,
			   but it is a subrange with lower bound 0, so
			   it is of type CARDINAL !!!
			*/
			expp->nd_type = card_type;
			break;
		}
		if (left->nd_type->tp_fund == T_ARRAY) {
			expp->nd_type = IndexType(left->nd_type);
			cstcall(expp, S_MAX);
			break;
		}
		if (left->nd_symb != STRING) {
			Xerror(left,"array parameter expected", edf);
			return 0;
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
		cstcall(expp,std);
		break;

	case S_ODD:
		if (!(left = getarg(&arg, T_INTORCARD, 0, edf))) return 0;
		expp->nd_type = bool_type;
		if (left->nd_class == Value) cstcall(expp, S_ODD);
		break;

	case S_ORD:
		if (!(left = getarg(&arg, T_DISCRETE, 0, edf))) return 0;
		expp->nd_type = card_type;
		if (left->nd_class == Value) cstcall(expp, S_ORD);
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
		if (! (left = getvariable(&arg, edf))) return 0;
		if (! (left->nd_type->tp_fund == T_POINTER)) {
			Xerror(left, "pointer variable expected", edf);
			return 0;
		}
		if (left->nd_class == Def) {
			left->nd_def->df_flags |= D_NOREG;
		}
		/* Now, make it look like a call to ALLOCATE or DEALLOCATE */
		{
			struct token dt;
			register struct token *tk = &dt;
			struct node *nd;

			tk->TOK_INT = PointedtoType(left->nd_type)->tp_size;
			tk->tk_symb = INTEGER;
			tk->tk_lineno = left->nd_lineno;
			nd = MkLeaf(Value, tk);
			nd->nd_type = card_type;
			tk->tk_symb = ',';
			arg->nd_right = MkNode(Link, nd, NULLNODE, tk);
			/* Ignore other arguments to NEW and/or DISPOSE ??? */

			FreeNode(expp->nd_left);
			tk->tk_symb = IDENT;
			tk->tk_lineno = expp->nd_left->nd_lineno;
			tk->TOK_IDF = str2idf(std == S_NEW ?
						"ALLOCATE" : "DEALLOCATE", 0);
			expp->nd_left = MkLeaf(Name, tk);
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

	case S_TRUNC:
		expp->nd_type = card_type;
		if (!(left = getarg(&arg, T_REAL, 0, edf))) return 0;
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
		if (left->nd_class == Value) cstcall(expp, S_VAL);
		break;

	case S_ADR:
		expp->nd_type = address_type;
		if (!(left = getarg(&arg, 0, 1, edf))) return 0;
		break;

	case S_DEC:
	case S_INC:
		expp->nd_type = 0;
		if (! (left = getvariable(&arg, edf))) return 0;
		if (! (left->nd_type->tp_fund & T_DISCRETE)) {
			Xerror(left,"illegal parameter type", edf);
			return 0;
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
		struct type *tp;

		expp->nd_type = 0;
		if (!(left = getvariable(&arg, edf))) return 0;
		tp = left->nd_type;
		if (tp->tp_fund != T_SET) {
			Xerror(arg, "SET parameter expected", edf);
			return 0;
		}
		if (!(left = getarg(&arg, T_DISCRETE, 0, edf))) return 0;
		if (!TstAssCompat(ElementType(tp), left->nd_type)) {
			/* What type of compatibility do we want here?
			   apparently assignment compatibility! ??? ???
			*/
			Xerror(arg, "unexpected parameter type", edf);
			return 0;
		}
		break;
		}

	default:
		crash("(ChkStandard)");
	}

	if (arg->nd_right) {
		Xerror(arg->nd_right, "too many parameters supplied", edf);
		return 0;
	}

	return 1;
}

STATIC int
ChkCast(expp, left)
	register struct node *expp, *left;
{
	/*	Check a cast and perform it if the argument is constant.
		If the sizes don't match, only complain if at least one of them
		has a size larger than the word size.
		If both sizes are equal to or smaller than the word size, there
		is no problem as such values take a word on the EM stack
		anyway.
	*/
	register struct node *arg = expp->nd_right;

	if ((! arg) || arg->nd_right) {
		Xerror(expp, "too many parameters in type cast", left->nd_def);
		return 0;
	}

	arg = arg->nd_left;
	if (! ChkExpression(arg)) return 0;

	if (arg->nd_type->tp_size != left->nd_type->tp_size &&
	    (arg->nd_type->tp_size > word_size ||
	     left->nd_type->tp_size > word_size)) {
		Xerror(expp, "unequal sizes in type cast", left->nd_def);
	}

	if (arg->nd_class == Value) {
		struct type *tp = left->nd_type;

		FreeNode(left);
		expp->nd_right->nd_left = 0;
		FreeNode(expp->nd_right);
		expp->nd_left = expp->nd_right = 0;
		*expp = *arg;
		expp->nd_type = tp;
	}
	else expp->nd_type = left->nd_type;

	return 1;
}

TryToString(nd, tp)
	register struct node *nd;
	struct type *tp;
{
	/*	Try a coercion from character constant to string.
	*/

	assert(nd->nd_symb == STRING);

	if (tp->tp_fund == T_ARRAY && nd->nd_type == char_type) {
		int ch = nd->nd_INT;

		nd->nd_type = standard_type(T_STRING, 1, (arith) 2);
		nd->nd_token.tk_data.tk_str = 
			(struct string *) Malloc(sizeof(struct string));
		nd->nd_STR = Salloc("X", 2);
		*(nd->nd_STR) = ch;
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
	done_before
};
