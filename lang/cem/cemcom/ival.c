/* $Header$ */
/* CODE FOR THE INITIALISATION OF GLOBAL VARIABLES */

#include	"nofloat.h"
#include	<em.h>
#include	"debug.h"
#include	"nobitfield.h"
#include	"arith.h"
#include	"align.h"
#include	"label.h"
#include	"expr.h"
#include	"type.h"
#include	"struct.h"
#include	"field.h"
#include	"assert.h"
#include	"Lpars.h"
#include	"class.h"
#include	"sizes.h"
#include	"idf.h"
#include	"level.h"
#include	"def.h"

#define con_nullbyte()	C_con_ucon("0", (arith)1)

char *symbol2str();
char *long2str();
struct expr *do_array(), *do_struct(), *IVAL();
extern char options[];

/*	do_ival() performs the initialisation of a global variable
	of type tp with the initialisation expression expr by calling IVAL().
	Guided by type tp, the expression is evaluated.
*/
do_ival(tpp, ex)
	struct type **tpp;
	struct expr *ex;
{
	if (IVAL(tpp, ex) != 0)
		too_many_initialisers(ex);
}

/*	IVAL() recursively guides the initialisation expression through the
	different routines for the different types of initialisation:
	-	array initialisation
	-	struct initialisation
	-	fundamental type initialisation
	Upto now, the initialisation of a union is not allowed!
	An initialisation expression tree consists of normal expressions
	which can be joined together by ',' nodes, which operator acts
	like the lisp function "cons" to build lists.
	IVAL() returns a pointer to the remaining expression tree.
*/
struct expr *
IVAL(tpp, ex)
	struct type **tpp;		/* type of global variable	*/
	register struct expr *ex;	/* initialiser expression	*/
{
	register struct type *tp = *tpp;
	
	switch (tp->tp_fund) {
	case ARRAY: /* array initialisation	*/
		if (valid_type(tp->tp_up, "array element") == 0)
			return 0;
		if (ISCOMMA(ex)) /* list of initialisation expressions */
			return do_array(ex, tpp);
		if (tp->tp_up->tp_fund == CHAR && ex->ex_class == String)
			/* initialisation like char s[] = "I am a string" */
			ch_array(tpp, ex);
		else /* " int i[24] = 12;"	*/
			check_and_pad(ex, tpp);
		break;
	case STRUCT: /* struct initialisation */
		if (valid_type(tp, "struct") == 0)
			return 0;
		if (ISCOMMA(ex)) /* list of initialisation expressions */
			return do_struct(ex, tp);
		check_and_pad(ex, tpp); /* "struct foo f = 12;"	*/
		break;
	case UNION:
		error("union initialisation not allowed");
		break;
	case ERRONEOUS:
		break;
	default: /* fundamental type	*/
		if (ISCOMMA(ex)) { /* " int i = {12};"	*/
			if (IVAL(tpp, ex->OP_LEFT) != 0)
				too_many_initialisers(ex);
			/*	return remainings of the list for the
				other members of the aggregate, if this
				item belongs to an aggregate.
			*/
			return ex->OP_RIGHT;
		}
		check_ival(ex, tp); /* "int i = 12;" */
		break;
	}
	return 0;
}

/*	do_array() initialises the members of an array described
	by type tp with the expressions in expr.
	Two important cases:
	-	the number of members is known
	-	the number of members is not known
	In the latter case, do_array() digests the whole expression
	tree it is given.
	In the former case, do_array() eats as many members from
	the expression tree as are needed for the array.
	If there are not sufficient members for the array, the remaining
	members are padded with zeroes
*/
struct expr *
do_array(ex, tpp)
	register struct expr *ex;
	struct type **tpp;
{
	register struct type *tp = *tpp;
	register arith elem_count;
	
	ASSERT(tp->tp_fund == ARRAY && ISCOMMA(ex));
	/*	the following test catches initialisations like
		char c[] = {"just a string"};
		or
		char d[] = {{"just another string"}};
		The use of the brackets causes this problem.
		Note: although the implementation of such initialisations
		is completely foolish, we did it!! (no applause, thank you)
	*/
	if (tp->tp_up->tp_fund == CHAR) {
		register struct expr *f = ex->OP_LEFT, *g = NILEXPR;

		while (ISCOMMA(f)) {	/* eat the brackets!!!	*/
			g = f;
			f = f->OP_LEFT;
		}
		if (f->ex_class == String) { /* hallelujah, it's a string! */
			ch_array(tpp, f);
			return g ? g->OP_RIGHT : ex->OP_RIGHT;
		}
		/* else: just go on with the next part of this function */
		if (g != 0)
			ex = g;
	}
	if (tp->tp_size == (arith)-1) {
		/* declared with unknown size: [] */
		for (elem_count = 0; ex; elem_count++) {
			/* eat whole initialisation expression	*/
			if (ISCOMMA(ex->OP_LEFT)) { /* embraced member */
				if (IVAL(&(tp->tp_up), ex->OP_LEFT) != 0)
					too_many_initialisers(ex);
				ex = ex->OP_RIGHT;
			}
			else {
				if (aggregate_type(tp->tp_up))
					ex = IVAL(&(tp->tp_up), ex);
				else {
					check_ival(ex->OP_LEFT, tp->tp_up);
					ex = ex->OP_RIGHT;
				}
			}
		}
		/* set the proper size	*/
		*tpp = construct_type(ARRAY, tp->tp_up, elem_count);
	}
	else {		/* the number of members is already known	*/
		arith dim = tp->tp_size / tp->tp_up->tp_size;

		for (elem_count = 0; elem_count < dim && ex; elem_count++) {
			if (ISCOMMA(ex->OP_LEFT)) { /* embraced member */
				if (IVAL(&(tp->tp_up), ex->OP_LEFT) != 0)
					too_many_initialisers(ex);
				ex = ex->OP_RIGHT;
			}
			else {
				if (aggregate_type(tp->tp_up))
					ex = IVAL(&(tp->tp_up), ex);
				else {
					check_ival(ex->OP_LEFT, tp->tp_up);
					ex = ex->OP_RIGHT;
				}
			}
		}
		if (ex && elem_count == dim)
			/*	all the members are initialised but there
				remains a part of the expression tree which
				is returned
			*/
			return ex;
		if ((ex == 0) && elem_count < dim)
			/*	the expression tree is completely absorbed
				but there are still members which must be
				initialised with zeroes
			*/
			do
				pad(tp->tp_up);
			while (++elem_count < dim);
	}
	return 0;
}


/*	do_struct() initialises a struct of type tp with the expression expr.
	The main loop is just controlled by the definition of the selectors
	during which alignment is taken care of.
*/
struct expr *
do_struct(ex, tp)
	register struct expr *ex;
	register struct type *tp;
{
	register struct sdef *sd = tp->tp_sdef;
	arith bytes_upto_here = (arith)0;
	arith last_offset = (arith)-1;
	
	ASSERT(tp->tp_fund == STRUCT && ISCOMMA(ex));
	/* as long as there are selectors and there is an initialiser..	*/
	while (sd && ex) {
		if (ISCOMMA(ex->OP_LEFT)) {	/* embraced expression	*/
			if (IVAL(&(sd->sd_type), ex->OP_LEFT) != 0)
				too_many_initialisers(ex);
			ex = ex->OP_RIGHT;
		}
		else {
			if (aggregate_type(sd->sd_type))
				/* selector is an aggregate itself	*/
				ex = IVAL(&(sd->sd_type), ex);
			else {
#ifdef NOBITFIELD
				/* fundamental type, not embraced */
				check_ival(ex->OP_LEFT, sd->sd_type);
				ex = ex->OP_RIGHT;
#else
				if (is_anon_idf(sd->sd_idf))
					/*	a hole in the struct due to
						the use of ";:n;" in a struct
						definition.
					*/
					put_bf(sd->sd_type, (arith)0);
				else { /* fundamental type, not embraced */
					check_ival(ex->OP_LEFT, sd->sd_type);
					ex = ex->OP_RIGHT;
				}
#endif NOBITFIELD
			}
		}
		if (sd->sd_sdef) /* align upto the next selector boundary */
			bytes_upto_here += zero_bytes(sd);
		if (last_offset != sd->sd_offset) {
			/* don't take the field-width more than once	*/
			bytes_upto_here +=
				size_of_type(sd->sd_type, "selector");
			last_offset = sd->sd_offset;
		}
		sd = sd->sd_sdef;
	}
	/* perfect fit if (ex && (sd == 0)) holds	*/
	if ((ex == 0) && (sd != 0)) {
		/* there are selectors left which must be padded with zeroes */
		do {
			pad(sd->sd_type);
			/* take care of the alignment restrictions	*/
			if (sd->sd_sdef)
				bytes_upto_here += zero_bytes(sd);
			/* no field thrown-outs here	*/
			bytes_upto_here +=
				size_of_type(sd->sd_type, "selector");
		} while (sd = sd->sd_sdef);
	}
	/* keep on aligning...	*/
	while (bytes_upto_here++ < tp->tp_size)
		con_nullbyte();
	return ex;
}

/*	check_and_pad() is given a simple initialisation expression
	where the type can be either a simple or an aggregate type.
	In the latter case, only the first member is initialised and
	the rest is zeroed.
*/
check_and_pad(ex, tpp)
	register struct expr *ex;
	struct type **tpp;
{
	/* ex is of a fundamental type	*/
	register struct type *tp = *tpp;

	if (tp->tp_fund == ARRAY) {
		if (valid_type(tp->tp_up, "array element") == 0)
			return;
		check_and_pad(ex, &(tp->tp_up));	/* first member	*/
		if (tp->tp_size == (arith)-1)
			/*	no size specified upto here: just
				set it to the size of one member.
			*/
			tp = *tpp = construct_type(ARRAY, tp->tp_up, (arith)1);
		else {
			register int dim = tp->tp_size / tp->tp_up->tp_size;
			/* pad remaining members with zeroes */
			while (--dim > 0)
				pad(tp->tp_up);
		}
	}
	else
	if (tp->tp_fund == STRUCT) {
		register struct sdef *sd = tp->tp_sdef;

		if (valid_type(tp, "struct") == 0)
			return;
		check_and_pad(ex, &(sd->sd_type));
		/* next selector is aligned by adding extra zeroes */
		if (sd->sd_sdef)
			zero_bytes(sd);
		while (sd = sd->sd_sdef) { /* pad remaining selectors	*/
			pad(sd->sd_type);
			if (sd->sd_sdef)
				zero_bytes(sd);
		}
	}
	else	/* simple type	*/
		check_ival(ex, tp);
}

/*	pad() fills an element of type tp with zeroes.
	If the element is an aggregate, pad() is called recursively.
*/
pad(tp)
	register struct type *tp;
{
	register arith sz = tp->tp_size;

	switch (tp->tp_fund) {
	case ARRAY:
		if (valid_type(tp->tp_up, "array element") == 0)
			return;
		break;
	case STRUCT:
		if (valid_type(tp, "struct") == 0)
			return;
		break;
	case UNION:
		if (valid_type(tp, "union") == 0)
			return;
		if (options['R']) {
			warning("initialisation of unions not allowed");
		}
		break;
#ifndef NOBITFIELD
	case FIELD:
		put_bf(tp, (arith)0);
		return;
#endif NOBITFIELD
	case ERRONEOUS:
		return;
	}

	while (sz >= word_size) {
		C_con_cst((arith) 0);
		sz -= word_size;
	}
	while (sz) {
		C_con_icon("0", (arith) 1);
		sz--;
	}
}

/*	check_ival() checks whether the initialisation of an element
	of a fundamental type is legal and, if so, performs the initialisation
	by directly generating the necessary code.
	No further comment is needed to explain the internal structure
	of this straightforward function.
*/
check_ival(expr, tp)
	register struct expr *expr;
	register struct type *tp;
{
	/*	The philosophy here is that ch7cast puts an explicit
		conversion node in front of the expression if the types
		are not compatible.  In this case, the initialisation
		expression is no longer a constant.
	*/
	struct expr *ex = expr;
	
	switch (tp->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
	case POINTER:
		ch7cast(&ex, '=', tp);
		expr = ex;
#ifdef DEBUG
		print_expr("init-expr after cast", expr);
#endif DEBUG
		if (!is_ld_cst(expr))
			illegal_init_cst(expr);
		else
		if (expr->VL_CLASS == Const)
			con_int(expr);
		else
		if (expr->VL_CLASS == Name) {
			register struct idf *idf = expr->VL_IDF;

			if (idf->id_def->df_level >= L_LOCAL)
				illegal_init_cst(expr);
			else	/* e.g., int f(); int p = f; */
			if (idf->id_def->df_type->tp_fund == FUNCTION)
				C_con_pnam(idf->id_text);
			else	/* e.g., int a; int *p = &a; */
				C_con_dnam(idf->id_text, expr->VL_VALUE);
		}
		else {
			ASSERT(expr->VL_CLASS == Label);
			C_con_dlb(expr->VL_LBL, expr->VL_VALUE);
		}
		break;
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
		ch7cast(&ex, '=', tp);
		expr = ex;
#ifdef DEBUG
		print_expr("init-expr after cast", expr);
#endif DEBUG
		if (expr->ex_class == Float)
			C_con_fcon(expr->FL_VALUE, expr->ex_type->tp_size);
		else
		if (expr->ex_class == Oper && expr->OP_OPER == INT2FLOAT) {
			/* float f = 1; */
			expr = expr->OP_RIGHT;
			if (is_cp_cst(expr))
				C_con_fcon(long2str((long)expr->VL_VALUE, 10),
					tp->tp_size);
			else 
				illegal_init_cst(expr);
		}
		else
			illegal_init_cst(expr);
		break;
#endif NOFLOAT

#ifndef NOBITFIELD
	case FIELD:
		ch7cast(&ex, '=', tp->tp_up);
		expr = ex;
#ifdef DEBUG
		print_expr("init-expr after cast", expr);
#endif DEBUG
		if (is_cp_cst(expr))
			put_bf(tp, expr->VL_VALUE);
		else
			illegal_init_cst(expr);
		break;
#endif NOBITFIELD

	case ERRONEOUS:
		break;
	default:
		crash("check_ival");
	}
}

/*	ch_array() initialises an array of characters when given
	a string constant.
	Alignment is taken care of.
*/
ch_array(tpp, ex)
	struct type **tpp;	/* type tp = array of characters	*/
	struct expr *ex;
{
	register struct type *tp = *tpp;
	register arith length;
	register char *s = ex->SG_VALUE;
	register arith ntopad;

	ASSERT(ex->ex_class == String);
	length = ex->SG_LEN;
	if (tp->tp_size == (arith)-1) {
		/* set the dimension	*/
		tp = *tpp = construct_type(ARRAY, tp->tp_up, length);
		ntopad = align(tp->tp_size, word_align) - tp->tp_size;
	}
	else {
		arith dim = tp->tp_size / tp->tp_up->tp_size;
		extern char options[];

		if (length > dim) {
			if (options['R'])
				too_many_initialisers(ex);
			else { /* don't take the null byte into account */
				if (length > dim + 1)
					expr_warning(ex,
						"too many initialisers");
				length = dim;
			}
		}
		ntopad = align(dim, word_align) - length;
	}
	/* throw out the characters of the already prepared string	*/
	while (length-- > 0)
		C_con_ucon(long2str((long)*s++ & 0xFF, 10), (arith)1);
	/* pad the allocated memory (the alignment has been calculated)	*/
	while (ntopad-- > 0)
		con_nullbyte();
}

str_cst(str, len)
	register char *str;
	register int len;
{
	while (len-- > 0)
		C_con_ucon(long2str((long)*str++ & 0xFF, 10), (arith)1);
}

#ifndef NOBITFIELD
/*	put_bf() takes care of the initialisation of (bit-)field
	selectors of a struct: each time such an initialisation takes place,
	put_bf() is called instead of the normal code generating routines.
	Put_bf() stores the given integral value into "field" and
	"throws" the result of "field" out if the current selector
	is the last of this number of fields stored at the same address.
*/
put_bf(tp, val)
	struct type *tp;
	arith val;
{
	static long field = (arith)0;
	static arith offset = (arith)-1;
	register struct field *fd = tp->tp_field;
	register struct sdef *sd =  fd->fd_sdef;
	static struct expr exp;

	ASSERT(sd);
	if (offset == (arith)-1) {
		/* first bitfield in this field	*/
		offset = sd->sd_offset;
		exp.ex_type = tp->tp_up;
		exp.ex_class = Value;
		exp.VL_CLASS = Const;
	}
	if (val != 0)	/* insert the value into "field"	*/
		field |= (val & fd->fd_mask) << fd->fd_shift;
	if (sd->sd_sdef == 0 || sd->sd_sdef->sd_offset != offset) {
		/* the selector was the last stored at this address	*/
		exp.VL_VALUE = field;
		con_int(&exp);
		field = (arith)0;
		offset = (arith)-1;
	}
}
#endif NOBITFIELD

int
zero_bytes(sd)
	register struct sdef *sd;
{
	/*	fills the space between a selector of a struct
		and the next selector of that struct with zero-bytes.
	*/
	register int n = sd->sd_sdef->sd_offset - sd->sd_offset -
		size_of_type(sd->sd_type, "struct member");
	register int count = n;

	while (n-- > 0)
		con_nullbyte();
	return count;
}

int
valid_type(tp, str)
	struct type *tp;
	char *str;
{
	if (tp->tp_size < 0) {
		error("size of %s unknown", str);
		return 0;
	}
	return 1;
}

con_int(ex)
	register struct expr *ex;
{
	register struct type *tp = ex->ex_type;

	ASSERT(is_cp_cst(ex));
	if (tp->tp_unsigned)
		C_con_ucon(long2str((long)ex->VL_VALUE, -10), tp->tp_size);
	else
		C_con_icon(long2str((long)ex->VL_VALUE, 10), tp->tp_size);
}

illegal_init_cst(ex)
	struct expr *ex;
{
	expr_error(ex, "illegal initialisation constant");
}

too_many_initialisers(ex)
	struct expr *ex;
{
	expr_error(ex, "too many initialisers");
}

aggregate_type(tp)
	register struct type *tp;
{
	return tp->tp_fund == ARRAY || tp->tp_fund == STRUCT;
}
