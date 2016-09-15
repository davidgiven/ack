/* E X P R E S S I O N   C H E C K I N G */

/*	Check expressions, and try to evaluate them as far as possible.
*/

#include    "parameters.h"
#include	"debug.h"

#include    <stdlib.h>
#include	<alloc.h>
#include	<assert.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<system.h>
#include	<print.h>

#include	"LLlex.h"
#include	"Lpars.h"
#include	"chk_expr.h"
#include	"const.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"
#include	"required.h"
#include	"scope.h"
#include	"type.h"

extern char *symbol2str();
STATIC int ChkUnOper();

STATIC
Xerror(nd, mess)
	register struct node *nd;
	char *mess;
{
	if( nd->nd_class == Def && nd->nd_def )	{
		if( nd->nd_def->df_kind != D_ERROR )
			node_error(nd,"\"%s\": %s",
				    nd->nd_def->df_idf->id_text, mess);
	}
	else	node_error(nd, "%s", mess);
}

struct node *
ZeroParam()
{
	register struct node *nd;

	nd = MkLeaf(Value, &dot);
	nd->nd_type = int_type;
	nd->nd_symb = INTEGER;
	nd->nd_INT = (arith) 0;
	nd = MkNode(Link, nd, NULLNODE, &dot);
	nd->nd_symb = ',';

	return nd;
}

MarkUsed(nd)
	register struct node *nd;
{
	while( nd && nd->nd_class != Def ) {
		if( (nd->nd_class == Arrsel) || (nd->nd_class == LinkDef) )
			nd = nd->nd_left;
		else if( nd->nd_class == Arrow)
			nd = nd->nd_right;
		else break;
	}

	if( nd && nd->nd_class == Def ) {
		register struct def *df = nd->nd_def;

		if( df->df_kind != D_FIELD ) {
			if( !(df->df_flags & (D_SET|D_VARPAR)) &&
			    (df->df_scope == CurrentScope) )
				if( !is_anon_idf(df->df_idf) ) {
					warning("\"%s\" used before set",
						df->df_idf->id_text);
				}
			df->df_flags |= (D_USED | D_SET);
		}
	}
}

int
ChkConstant(expp)
	register struct node *expp;
{
	register struct node *nd;

	if( !(nd = expp->nd_right) ) nd = expp;

	if( nd->nd_class == Name &&  !ChkLinkOrName(nd) ) return 0;

	if( nd->nd_class != Value || expp->nd_left )	{
		Xerror(nd, "constant expected");
		return 0;
	}

	if( expp->nd_class == Uoper )
		return ChkUnOper(expp);
	else if( nd != expp )	{
		Xerror(expp, "constant expected");
		return 0;
	}
	return 1;
}

int
ChkVariable(expp)
	register struct node *expp;
{
	/* Check that "expp" indicates an item that can be accessed */

	if( !ChkLhs(expp) ) return 0;

	if( expp->nd_class == Def && expp->nd_def->df_kind == D_FUNCTION ) {
		Xerror(expp, "illegal use of function name");
		return 0;
	}
	return 1;
}

int
ChkLhs(expp)
	register struct node *expp;
{
	int class;

	/* Check that "expp" indicates an item that can be the lhs
	   of an assignment.
	*/
	if( !ChkVarAccess(expp) ) return 0;

	class = expp->nd_class;

	/* a constant is replaced by it's value in ChkLinkOrName, check here !,
	 * the remaining classes are checked by ChkVarAccess
	 */
	if( class == Value )	{
		node_error(expp, "can't access a value");
		return 0;
	}

	if( class == Def &&
	    !(expp->nd_def->df_kind & (D_FIELD | D_FUNCTION | D_VARIABLE)) ) {
		Xerror(expp, "variable expected");
		return 0;
	}

	/* assignment to function name */
	if( class == Def && expp->nd_def->df_kind == D_FUNCTION )
		if( expp->nd_def->prc_res )
			expp->nd_type = ResultType(expp->nd_def->df_type);
		else	{
			Xerror(expp, "illegal assignment to function-name");
			return 0;
		}

	return 1;
}

#ifdef DEBUG
STATIC int
ChkValue(expp)
	register struct node *expp;
{
	switch( expp->nd_symb )	{
		case INTEGER:
		case REAL:
		case STRING:
		case NIL:
			return 1;

		default:
			crash("(ChkValue)");
	}
	/*NOTREACHED*/
}
#endif

int
ChkLinkOrName(expp)
	register struct node *expp;
{
	register struct def *df;

	expp->nd_type = error_type;

	if( expp->nd_class == Name )	{
		expp->nd_def = lookfor(expp, CurrVis, 1);
		expp->nd_class = Def;
		expp->nd_type = expp->nd_def->df_type;
	}
	else if( expp->nd_class == Link )	{
		/* a selection from a record */
		register struct node *left = expp->nd_left;

		assert(expp->nd_symb == '.');

		if( !ChkVariable(left) ) return 0;

		if( left->nd_type->tp_fund != T_RECORD )	{
			Xerror(left, "illegal selection");
			return 0;
		}

		if( !(df = lookup(expp->nd_IDF, left->nd_type->rec_scope, D_INUSE)) ) {
			id_not_declared(expp);
			return 0;
		}
		else	{
			expp->nd_def = df;
			expp->nd_type = df->df_type;
			expp->nd_class = LinkDef;
		}
		return 1;
	}
	assert(expp->nd_class == Def);

	df = expp->nd_def;

	if( df->df_kind & (D_ENUM | D_CONST) )	{
		MarkUsed(expp);
		/* Replace an enum-literal or a CONST identifier by its value.
		*/
		if( df->df_kind == D_ENUM )	{
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
	if( !ChkLinkOrName(expp) ) return 0;
	if( expp->nd_class != Def ) return 1;

	if( !(expp->nd_def->df_kind & D_VALUE) ) {
		Xerror(expp, "value expected");
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

	if( !ChkExpression(right) ) return 0;

	MarkUsed(right);

	expp->nd_type = tpr = BaseType(right->nd_type);

	switch( expp->nd_symb )	{
	case '+':
		if( tpr->tp_fund & T_NUMERIC )	{
			*expp = *right;
			free_node(right);
			return 1;
		}
		break;

	case '-':
		if( tpr->tp_fund == T_INTEGER || tpr->tp_fund == T_LONG ) {
			if( right->nd_class == Value )
				cstunary(expp);
			return 1;
		}
		if( tpr->tp_fund == T_REAL )	{
			if( right->nd_class == Value )	{
				expp->nd_token.tk_data.tk_real = right->nd_RIV;
				expp->nd_class = Value;
				expp->nd_symb = REAL;
				FreeNode(right);
				expp->nd_right = NULLNODE;
			}
			return 1;
		}
		break;

	case NOT:
		if( tpr == bool_type )	{
			if( right->nd_class == Value )
				cstunary(expp);
			return 1;
		}
		break;

	case '(':
		/* Delete the brackets */
		*expp = *right;
		free_node(right);
		return 1;

	default:
		crash("(ChkUnOper)");
	}
	node_error(expp, "\"%s\": illegal operand", symbol2str(expp->nd_symb));
	return 0;
}

STATIC struct type *
ResultOfOperation(operator, tpl, tpr)
	struct type *tpl, *tpr;
{
	/* Return the result type of the binary operation "operator",
	   with operand types "tpl" and "tpr".
	 */

	switch( operator )	{
		case '='	:
		case NOTEQUAL	:
		case '<'	:
		case '>'	:
		case LESSEQUAL	:
		case GREATEREQUAL:
		case IN		:
				return bool_type;
		case '+'	:
		case '-'	:
		case '*'	:
				if( tpl == real_type || tpr == real_type )
					return real_type;
				if( tpl == long_type || tpr == long_type)
					return long_type;
				return tpl;
		case '/'	:
				return real_type;
	}
	if (tpr == long_type && tpl == int_type) return tpr;
	return tpl;
}

STATIC int
AllowedTypes(operator)
{
	/* Return a bit mask indicating the allowed operand types for
	   binary operator "operator".
	 */

	switch( operator )	{
		case '+'	:
		case '-'	:
		case '*'	:
				return T_NUMERIC | T_SET;
		case '/'	:
				return T_NUMERIC;
		case DIV	:
		case MOD	:
				return T_INTEGER | T_LONG;
		case OR		:
		case AND	:
				return T_ENUMERATION;
		case '='	:
		case NOTEQUAL   :
				return T_ENUMERATION | T_CHAR | T_NUMERIC |
					T_SET | T_POINTER | T_STRINGCONST |
					T_STRING;
		case LESSEQUAL	:
		case GREATEREQUAL:
				return T_ENUMERATION | T_CHAR | T_NUMERIC |
					T_SET | T_STRINGCONST;
		case '<'	:
		case '>'	:
				return T_ENUMERATION | T_CHAR | T_NUMERIC |
					T_STRINGCONST;
		default		:
				crash("(AllowedTypes)");
	}
	/*NOTREACHED*/
}

STATIC int
Boolean(operator)
{
	return operator == OR || operator == AND;
}

STATIC int
ChkBinOper(expp)
	register struct node *expp;
{
	/*	Check a binary operation.
	 */
	register struct node *left, *right;
	struct type *tpl, *tpr;
	int retval, allowed;

	left = expp->nd_left;
	right = expp->nd_right;

	retval = ChkExpression(left);
	retval &= ChkExpression(right);

	MarkUsed(left);
	MarkUsed(right);

	tpl = BaseType(left->nd_type);
	tpr = BaseType(right->nd_type);

	expp->nd_type = ResultOfOperation(expp->nd_symb, tpl ,tpr);

	/* Check that the application of the operator is allowed on the type
	   of the operands.
	   There are some needles and pins:
	   - Boolean operators are only allowed on boolean operands, but the
	     "allowed-mask" of "AllowedTypes" can only indicate an enumeration
	     type.
	   - The IN-operator has as right-hand-side operand a set.
	   - Strings and packed arrays can be equivalent.
	   - In some cases, integers must be converted to reals.
	   - If one of the operands is the empty set then the result doesn't
	     have to be the empty set.
	*/

	if( expp->nd_symb == IN )	{
		if( tpr->tp_fund != T_SET )	{
			node_error(expp, "\"IN\": right operand must be a set");
			return 0;
		}
		if( !TstAssCompat(tpl, ElementType(tpr)) )	{
			node_error(expp, "\"IN\": incompatible types");
			return 0;
		}
		if( left->nd_class == Value && right->nd_class == Set )
			cstset(expp);
		return retval;
	}

	if( !retval ) return 0;

	allowed = AllowedTypes(expp->nd_symb);

	if( !(tpr->tp_fund & allowed) || !(tpl->tp_fund & allowed) )	{
		arith ub;
		extern arith IsString();

		if( allowed & T_STRINGCONST && (ub = IsString(tpl)) )	{
			if( ub == IsString(tpr) )
				return 1;
			else	{
				node_error(expp, "\"%s\": incompatible types",
						symbol2str(expp->nd_symb));
				return 0;
			}
		}
		else if( allowed & T_STRING && tpl->tp_fund == T_STRING )
				return 1;

		node_error(expp, "\"%s\": illegal operand type(s)",
						symbol2str(expp->nd_symb));
		return 0;
	}

	if( Boolean(expp->nd_symb) && tpl != bool_type )	{
		node_error(expp, "\"%s\": illegal operand type(s)",
						symbol2str(expp->nd_symb));
		return 0;
	}

	if( allowed & T_NUMERIC )	{
		if( (tpl == int_type || tpl == long_type) &&
		    (tpr == real_type || expp->nd_symb == '/') ) {
			expp->nd_left =
				MkNode(Cast, NULLNODE, expp->nd_left, &dot);
			expp->nd_left->nd_type = tpl = real_type;
		}
		if( tpl == real_type &&
				(tpr == int_type || tpr == long_type))	{
			expp->nd_right =
				MkNode(Cast, NULLNODE, expp->nd_right, &dot);
			expp->nd_right->nd_type = tpr = real_type;
		}
		if( tpl == int_type && tpr == long_type) {
			expp->nd_left =
				MkNode(IntCoerc, NULLNODE, expp->nd_left, &dot);
			expp->nd_left->nd_type = long_type;
		}
		else if( tpl == long_type && tpr == int_type) {
			expp->nd_right =
				MkNode(IntCoerc, NULLNODE, expp->nd_right, &dot);
			expp->nd_right->nd_type = long_type;
		}
	}

	/* Operands must be compatible */
	if( !TstCompat(tpl, tpr) )	{
		node_error(expp, "\"%s\": incompatible types",
						symbol2str(expp->nd_symb));
		return 0;
	}

	if( tpl->tp_fund & T_SET )	{
		if( tpl == emptyset_type )
			left->nd_type = tpr;
		else if( tpr == emptyset_type )
			right->nd_type = tpl;

		if( expp->nd_type == emptyset_type )
			expp->nd_type = tpr;
		if( left->nd_class == Set && right->nd_class == Set )
			cstset(expp);
	}
	else if( tpl->tp_fund != T_REAL &&
		left->nd_class == Value && right->nd_class == Value )
			cstbin(expp);

	return 1;
}

STATIC int
ChkElement(expp, tp, set, cnt)
	register struct node *expp;
	register struct type **tp;
	arith **set;
	unsigned *cnt;
{
	/*	Check elements of a set. This routine may call itself
		recursively. Also try to compute the set!
	*/
	register struct node *left = expp->nd_left;
	register struct node *right = expp->nd_right;
	register int i;
	extern char *Malloc();

	if( expp->nd_class == Link && expp->nd_symb == UPTO )	{
		/* [ ... , expr1 .. expr2,  ... ]
		   First check expr1 and expr2, and try to compute them.
		*/
		if( !ChkElement(left, tp, set, cnt) ||
					!ChkElement(right, tp, set, cnt) )
			return 0;

		if( left->nd_class == Value &&
				right->nd_class == Value && *set )	{

			if( left->nd_INT > right->nd_INT )	{
				/* Remove lower and upper bound of the range.
				*/
				*cnt -= 2;
				(*set)[left->nd_INT/wrd_bits] &=
						~(1 << (left->nd_INT%wrd_bits));
				(*set)[right->nd_INT/wrd_bits] &=
					       ~(1 << (right->nd_INT%wrd_bits));
			}
			else
				/* We have a constant range. Put all elements
				   in the set.
				*/
			    for( i = left->nd_INT + 1; i < right->nd_INT; i++ )
				(*set)[i/wrd_bits] |= ( 1 << (i%wrd_bits) );
		}
		return 1;
	}

	/* Here, a single element is checked
	*/
	if( !ChkExpression(expp) ) return 0;
	MarkUsed(expp);

	if( *tp == emptyset_type )	{
		/* first element in set determines the type of the set */
		unsigned size;

		*tp = set_type(expp->nd_type, 0);
		size = (*tp)->tp_size * (sizeof(arith) / word_size);
		*set = (arith *) Malloc(size);
		clear((char *) *set, size);
	}
	else if( !TstCompat(ElementType(*tp), expp->nd_type) )	{
		node_error(expp, "set element has incompatible type");
		return 0;
	}

	if( expp->nd_class == Value )	{
		/* a constant element
		*/
		i = expp->nd_INT;

		if( expp->nd_type == int_type )	{
			/* Check only integer base-types because they are not
			   equal to the integer host-type. The other base-types
			   are equal to their host-types.
			*/

	    		if( i < 0 || i > max_intset )	{
				node_error(expp, "set element out of range");
				return 0;
			}
		}

		if( *set ) (*set)[i/wrd_bits] |= ( 1 << (i%wrd_bits));
		(*cnt)++;
	}
	else if( *set )	{
		free((char *) *set);
		*set = (arith *) 0;
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
	register struct node *nd = expp->nd_right;
	arith *set = (arith *) 0;
	unsigned cnt = 0;

	assert(expp->nd_symb == SET);

	expp->nd_type = emptyset_type;

	/* Now check the elements given, and try to compute a constant set.
	   First allocate room for the set, but only if it isn't empty.
	*/
	if( !nd )	{
		/* The resulting set IS empty, so we just return
		*/
		expp->nd_class = Set;
		expp->nd_set = (arith *) 0;
		return 1;
	}

	/* Now check the elements, one by one
	*/
	while( nd )	{
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		if( !ChkElement(nd->nd_left, &(expp->nd_type), &set, &cnt) )
			return 0;
		nd = nd->nd_right;
	}

	if( set )	{
		/* Yes, it was a constant set, and we managed to compute it!
		   Notice that at the moment there is no such thing as
		   partial evaluation. Either we evaluate the set, or we
		   don't (at all). Improvement not neccesary (???)
		   ??? sets have a contant part and a variable part ???
		*/
		expp->nd_class = Set;
		if( !cnt )	{
			/* after all the work we've done, the set turned out
			   out to be empty!
			*/
			free((char *) set);
			set = (arith *) 0;
		}
		expp->nd_set = set;
		FreeNode(expp->nd_right);
		expp->nd_right = NULLNODE;
	}

	return 1;
}

char *
ChkAllowedVar(nd, reading)		/* reading indicates read or readln */
	register struct node *nd;
{
	char *message = 0;

	switch( nd->nd_class )	{
	case Def:
		if( nd->nd_def->df_flags & D_INLOOP ) {
			message = "control variable";
			break;
		}
		if( nd->nd_def->df_kind != D_FIELD ) break;
		/* FALL THROUGH */

	case LinkDef:
		assert(nd->nd_def->df_kind == D_FIELD);

		if( nd->nd_def->fld_flags & F_PACKED )
			message = "field of packed record";
		else if( nd->nd_def->fld_flags & F_SELECTOR )
			message = "variant selector";
		break;

	case Arrsel:
		if( IsPacked(nd->nd_left->nd_type) )
			if( !reading ) message = "component of packed array";
		break;

	case Arrow:
		if( nd->nd_right->nd_type->tp_fund == T_FILE )
			message = "filebuffer variable";
		break;

	default:
		crash("(ChkAllowedVar)");
		/*NOTREACHED*/
	}
	MarkDef(nd, D_SET, 1);
	return message;
}

int
ChkVarPar(nd, name)
	register struct node *nd, *name;
{
	/* 	ISO 6.6.3.3 :
		An actual variable parameter shall not denote a field
		that is the selector of a variant-part or a component
		of a variable where that variable possesses a type
		that is designated packed.
	*/
	static char err_mes[80];
	char *message = (char *) 0;

	if( !ChkVariable(nd) ) return 0;

	message = ChkAllowedVar(nd, 0);

	if( message )	{
		sprint(err_mes, "%s can't be a variable parameter", message);
		Xerror(name, err_mes);
		return 0;
	}
	return 1;
}

STATIC struct node *
getarg(argp, bases, varaccess, name, paramtp)
	struct node **argp, *name;
	struct type *paramtp;
{
	/*	This routine is used to fetch the next argument from an
		argument list. The argument list is indicated by "argp".
		The parameter "bases" is a bitset indicating which types are
		allowed at this point, and "varaccess" is a flag indicating
		that the address from this argument is taken, so that it
		must be a varaccess and may not be a register variable.
	*/
	register struct node *arg = (*argp)->nd_right;
	register struct node *left;

	if( !arg )	{
		Xerror(name, "too few arguments supplied");
		return 0;
	}

	left = arg->nd_left;
	*argp = arg;

	if( paramtp && paramtp->tp_fund & T_ROUTINE )	{
		/* From the context it appears that the occurrence of the
		   procedure/function-identifier is not a call.
		*/
		if( left->nd_class != NameOrCall )	{
			Xerror(name, "illegal proc/func parameter");
			return 0;
		}
		else if( ChkLinkOrName(left->nd_left) ) {
			left->nd_type = left->nd_left->nd_type;
			MarkUsed(left->nd_left);
		}
		else return 0;
	}
	else if( varaccess ) {
	    if( !ChkVarPar(left, name) ) {
		MarkUsed(left);
		return 0;
	    }
	}
	else if( !ChkExpression(left) ) {
		MarkUsed(left);
		return 0;
	}

	MarkUsed(left);

	if( !varaccess &&  bases == T_INTEGER &&
		    BaseType(left->nd_type)->tp_fund == T_LONG) {
		arg->nd_left = MkNode(IntReduc, NULLNODE, left, &dot);
		arg->nd_left->nd_type = int_type;
		left = arg->nd_left;
	}

	if( bases && !(BaseType(left->nd_type)->tp_fund & bases) )	{
		Xerror(name, "unexpected parameter type");
		return 0;
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
	struct node *name;
	register struct paramlist *param;
	char ebuf[80];
	int retval = 1;
	int cnt = 0;
	int new_par_section;
	struct type *lasttp = NULLTYPE;

	name = left = expp->nd_left;

	if( left->nd_type == error_type )	{
		/* Just check parameters as if they were value parameters
		*/
		expp->nd_type = error_type;
		while( expp->nd_right )
			(void) getarg(&expp, 0, 0, name, NULLTYPE);
		return 0;
	}

	expp->nd_type = ResultType(left->nd_type);

	/* Check parameter list
	*/
	for( param = ParamList(left->nd_type); param; param = param->next ) {
		if( !(left = getarg(&expp, 0, (int) IsVarParam(param), name,
							TypeOfParam(param))) )
			return 0;

		cnt++;
		new_par_section = lasttp != TypeOfParam(param);
		if( !TstParCompat(TypeOfParam(param), left->nd_type,
			    (int) IsVarParam(param), left, new_par_section) ) {
			sprint(ebuf, "type incompatibility in parameter %d",
					cnt);
			Xerror(name, ebuf);
			retval = 0;
		}

		/* Convert between integers and longs.
		 */
		if( !IsVarParam(param) && options['d'] )	{
			if( left->nd_type->tp_fund == T_INTEGER &&
					TypeOfParam(param)->tp_fund == T_LONG) {
				expp->nd_left =
					MkNode(IntCoerc, NULLNODE, left, &dot);
				expp->nd_left->nd_type = long_type;
				left = expp->nd_left;
			}
			else if( left->nd_type->tp_fund == T_LONG &&
				    TypeOfParam(param)->tp_fund == T_INTEGER) {
				expp->nd_left =
					MkNode(IntReduc, NULLNODE, left, &dot);
				expp->nd_left->nd_type = int_type;
				left = expp->nd_left;
			}
		}

		if( left->nd_type == emptyset_type )
			/* type of emptyset determined by the context */
			left->nd_type = TypeOfParam(param);

		lasttp = TypeOfParam(param);
	}

	if( expp->nd_right )	{
		Xerror(name, "too many arguments supplied");
		while( expp->nd_right )
			(void) getarg(&expp, 0, 0, name, NULLTYPE);
		return 0;
	}

	return retval;
}

STATIC int ChkStandard();

int
ChkCall(expp)
	register struct node *expp;
{
	/*	Check something that looks like a procedure or function call.
		Of course this does not have to be a call at all,
		it may also be a standard procedure call.
	*/

	/* First, get the name of the function or procedure
	*/
	register struct node *left = expp->nd_left;

	expp->nd_type = error_type;

	if( ChkLinkOrName(left) )	{

		MarkUsed(left);
		if( IsProcCall(left) || left->nd_type == error_type )	{
			/* A call.
		   	   It may also be a call to a standard procedure
			*/

			if( left->nd_type == std_type )
				/* A standard procedure
				*/
				return ChkStandard(expp, left);

			/* Here, we have found a real procedure call. 
			*/
		}
		else	{
			node_error(left, "procedure or function expected");
			return 0;
		}
	}
	return ChkProcCall(expp);
}

STATIC int
ChkExCall(expp)
	register struct node *expp;
{
	if( !ChkCall(expp) ) return 0;

	if( !expp->nd_type )	{
		node_error(expp, "function call expected");
		return 0;
	}
	return 1;
}

STATIC int
ChkNameOrCall(expp)
	register struct node *expp;
{
	/* From the context it appears that the occurrence of the function-
	   identifier is a call to that function
	*/
	assert(expp->nd_class == NameOrCall);
	expp->nd_class = Call;

	return ChkExCall(expp);
}

STATIC int
ChkStandard(expp,left)
	register struct node *expp, *left;
{
	/*	Check a call of a standard procedure or function
	*/

	struct node *arg = expp;
	struct node *name = left;
	int req;

	assert(left->nd_class == Def);

	req = left->nd_def->df_value.df_reqname;

	switch( req )	{
	    case R_ABS:
	    case R_SQR:
		if( !(left = getarg(&arg, T_NUMERIC, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = left->nd_type;
		if( left->nd_class == Value &&
					expp->nd_type->tp_fund != T_REAL )
			cstcall(expp, req);
		break;

	    case R_SIN:
	    case R_COS:
	    case R_EXP:
	    case R_LN:
	    case R_SQRT:
	    case R_ARCTAN:
		if( !(left = getarg(&arg, T_NUMERIC, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = real_type;
		if( BaseType(left->nd_type)->tp_fund == T_INTEGER ||
			    BaseType(left->nd_type)->tp_fund == T_LONG)	{
			arg->nd_left = MkNode(Cast,NULLNODE, arg->nd_left,&dot);
			arg->nd_left->nd_type = real_type;
		}
		break;

	    case R_TRUNC:
	    case R_ROUND:
		if( !(left = getarg(&arg, T_REAL, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = int_type;
		break;

	    case R_ORD:
		if( !(left = getarg(&arg, T_ORDINAL, 0, name, NULLTYPE)) )
			return 0;
		if( BaseType(left->nd_type)->tp_fund == T_LONG )	{
			arg->nd_left = MkNode(IntReduc, NULLNODE, arg->nd_left, &dot);
			arg->nd_left->nd_type = int_type;
		}
		expp->nd_type = int_type;
		if( left->nd_class == Value )
			cstcall(expp, R_ORD);
		break;

	    case R_CHR:
		if( !(left = getarg(&arg, T_INTEGER, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = char_type;
		if( left->nd_class == Value )
			cstcall(expp, R_CHR);
		break;

	    case R_SUCC:
	    case R_PRED:
		if( !(left = getarg(&arg, T_ORDINAL, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = left->nd_type;
		if( left->nd_class == Value && options['R'] )
			cstcall(expp, req);
		break;

	    case R_ODD:
		if( !(left = getarg(&arg, T_INTEGER | T_LONG , 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = bool_type;
		if( left->nd_class == Value )
			cstcall(expp, R_ODD);
		break;

	    case R_EOF:
	    case R_EOLN:
	    case R_GET:
	    case R_PAGE:	{
		int st_out;

		if( req == R_PAGE )	{
			expp->nd_type = NULLTYPE;
			st_out = 1;
		}
		else	{
			st_out = 0;
			if (req == R_GET) {
				expp->nd_type = NULLTYPE;
			}
			else	expp->nd_type = bool_type;
		}
		if( !arg->nd_right )	{
			struct node *nd;

			if( !(nd = ChkStdInOut(name->nd_IDF->id_text, st_out)) )
				return 0;

			expp->nd_right = MkNode(Link, nd, NULLNODE, &dot);
			expp->nd_right->nd_symb = ',';
			arg = arg->nd_right;
		}
		else	{
			if( !(left = getarg(&arg, T_FILE, 1, name, NULLTYPE)) )
				return 0;
			if( (req == R_PAGE || req == R_EOLN)
			    && left->nd_type != text_type ) {
				Xerror(name, "textfile expected");
				return 0;
			}
		}
		break;

	    }
	    case R_REWRITE:
	    case R_PUT:
	    case R_RESET:
		if( !(left = getarg(&arg, T_FILE, 1, name, NULLTYPE)) )
			return 0;
		expp->nd_type = NULLTYPE;
		break;

	    case R_PACK:
	    case R_UNPACK:	{
		struct type *tp1, *tp2, *tp3;

		if( req == R_PACK )	{
			/* pack(a, i, z) */
			if( !(left = getarg(&arg, T_ARRAY, 1, name, NULLTYPE)) )
				return 0;
			tp1 = left->nd_type;		/* (a) */
			if( !(left = getarg(&arg, 0, 0, name, NULLTYPE)) )
				return 0;
			tp2 = left->nd_type;		/* (i) */
			if( !(left = getarg(&arg, T_ARRAY, 1, name, NULLTYPE)) )
				return 0;
			tp3 = left->nd_type;		/* (z) */
		}
		else	{
			/* unpack(z, a, i) */
			if( !(left = getarg(&arg, T_ARRAY, 1, name, NULLTYPE)) )
				return 0;
			tp3 = left->nd_type;		/* (z) */
			if( !(left = getarg(&arg, T_ARRAY, 1, name, NULLTYPE)) )
				return 0;
			tp1 = left->nd_type;		/* (a) */
			if( !(left = getarg(&arg, 0, 0, name, NULLTYPE)) )
				return 0;
			tp2 = left->nd_type;		/* (i) */
		}
		if( IsConformantArray(tp1) || IsPacked(tp1) )	{
			Xerror(name, "unpacked array expected");
			return 0;
		}
		if( !TstAssCompat(IndexType(tp1), tp2) )	{
			Xerror(name, "ordinal constant expected");
			return 0;
		}
		if( IsConformantArray(tp3) || !IsPacked(tp3) )	{
			Xerror(name, "packed array expected");
			return 0;
		}
		if( !TstTypeEquiv(tp1->arr_elem, tp3->arr_elem) )	{
			Xerror(name, "component types of arrays not equal");
			return 0;
		}
		expp->nd_type = NULLTYPE;
		break;
	    }

	    case R_NEW:
	    case R_DISPOSE:
		if( !(left = getarg(&arg, T_POINTER, 1, name, NULLTYPE)) )
			return 0;
		if( arg->nd_right )	{
			/* varargs new/dispose(p,c1,.....) */
			register struct selector *sel;
			register arith i;

			if( PointedtoType(left->nd_type)->tp_fund != T_RECORD )
				break;
			sel = PointedtoType(left->nd_type)->rec_sel;
			do	{
				if( !sel ) break;

				arg = arg->nd_right;
				left = arg->nd_left;

				/* ISO : COMPILETIME CONSTANTS NOT PERMITTED */
				if( !ChkConstant(left) ) return 0;

				if( !TstCompat(left->nd_type, sel->sel_type) ) {
					node_error(left,
					   "type incompatibility in caselabel");
					return 0;
				}

				i = left->nd_INT - sel->sel_lb;
				if( i < 0 || i >= sel->sel_ncst )	{
					node_error(left,
						"case constant: out of bounds");
					return 0;
				}

				sel = sel->sel_ptrs[i];
			} while( arg->nd_right );

			FreeNode(expp->nd_right->nd_right);
			expp->nd_right->nd_right = NULLNODE;
		}
		expp->nd_type = NULLTYPE;
		break;

	    case R_MARK:
	    case R_RELEASE:
		if( !(left = getarg(&arg, T_POINTER, 1, name, NULLTYPE)) )
			return 0;
		expp->nd_type = NULLTYPE;
		break;

	    case R_HALT:
		if( !arg->nd_right ) 		/* insert 0 parameter */
			arg->nd_right = ZeroParam();
		if( !(left = getarg(&arg, T_INTEGER, 0, name, NULLTYPE)) )
			return 0;
		expp->nd_type = NULLTYPE;
		break;

	    default:
		crash("(ChkStandard)");
	}
	
	if( arg->nd_right )	{
		Xerror(name, "too many arguments supplied");
		return 0;
	}

	return 1;
}

STATIC int
ChkArrow(expp)
	register struct node *expp;
{
	/*	Check an application of the '^' operator.
		The operand must be a variable of a pointer-type or a
		variable of a file-type.
	*/

	register struct type *tp;

	assert(expp->nd_class == Arrow);
	assert(expp->nd_symb == '^');

	expp->nd_type = error_type;

	if( !ChkVariable(expp->nd_right) ) return 0;

	MarkUsed(expp->nd_right);

	tp = expp->nd_right->nd_type;

	if( !(tp->tp_fund & (T_POINTER | T_FILE)) )	{
		node_error(expp, "\"^\": illegal operand");
		return 0;
	}

	expp->nd_type = PointedtoType(tp);
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

	/* Check the index first, so a[a[j]] is checked in order of
	 * evaluation. This to make sure that warnings are generated
	 * in the right order.
	 */
	retval = ChkExpression(expp->nd_right);
	MarkUsed(expp->nd_right);
	retval &= ChkVariable(expp->nd_left);

	tpl = expp->nd_left->nd_type;
	tpr = expp->nd_right->nd_type;
	if( tpl == error_type || tpr == error_type ) return 0;

	if( tpl->tp_fund != T_ARRAY )	{
		node_error(expp, "not indexing an ARRAY type");
		return 0;
	}

	/* Type of the index must be assignment compatible with
	   the index type of the array.
	*/
	if( !TstCompat(IndexType(tpl), tpr) )	{
		node_error(expp, "incompatible index type");
		return 0;
	}

	if( tpr == long_type ) {
		expp->nd_right = MkNode(IntReduc, NULLNODE, expp->nd_right, &dot);
		expp->nd_right->nd_type = int_type;
	}

	expp->nd_type = tpl->arr_elem;
	return retval;
}

STATIC int
done_before()
{
	return 1;
}

STATIC int
no_var_access(expp)
	struct node *expp;
{
	node_error(expp, "variable-access expected");
	return 0;
}

extern int	NodeCrash();

int (*ExprChkTable[])() = {
#ifdef DEBUG
	ChkValue,
#else
	done_before,
#endif
	ChkExLinkOrName,
	ChkUnOper,
	ChkBinOper,
	ChkSet,
	NodeCrash,
	ChkExCall,
	ChkNameOrCall,
	ChkArrow,
	ChkArr,
	NodeCrash,
	ChkExLinkOrName,
	NodeCrash,
	NodeCrash,
	NodeCrash,
	NodeCrash
};

int (*VarAccChkTable[])() = {
	no_var_access,
	ChkLinkOrName,
	no_var_access,
	no_var_access,
	no_var_access,
	NodeCrash,
	no_var_access,
	no_var_access,
	ChkArrow,
	ChkArr,
	done_before,
	ChkLinkOrName,
	done_before,
	no_var_access,
	no_var_access,
	no_var_access
};
