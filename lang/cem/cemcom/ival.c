/* $Header$ */
/* CODE FOR THE INITIALISATION OF GLOBAL VARIABLES */

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
	struct expr *ex;		/* initialiser expression	*/
{
	register struct type *tp = *tpp;
	
	switch (tp->tp_fund) {
	case ARRAY:
		/* array initialisation	*/
		if (valid_type(tp->tp_up, "array element") == 0)
			return 0;
		if (ISCOMMA(ex)) /* list of initialisation expressions */
			return do_array(ex, tpp);
		/* catch initialisations like char s[] = "I am a string" */
		if (tp->tp_up->tp_fund == CHAR && ex->ex_class == String)
			init_string(tpp, ex);
		else /* " int i[24] = 12;"	*/
			check_and_pad(ex, tpp);
		break;
	case STRUCT:
		/* struct initialisation */
		if (valid_type(tp, "struct") == 0)
			return 0;
		if (ISCOMMA(ex)) /* list of initialisation expressions */
			return do_struct(ex, tp);
		/* "struct foo f = 12;"	*/
		check_and_pad(ex, tpp);
		break;
	case UNION:
		error("union initialisation not allowed");
		break;
	case ERRONEOUS:
		break;
	default: /* fundamental type	*/
		if (ISCOMMA(ex))	{ /* " int i = {12};"	*/
			if (IVAL(tpp, ex->OP_LEFT) != 0)
				too_many_initialisers(ex);
			/*	return remainings of the list for the
				other members of the aggregate, if this
				item belongs to an aggregate.
			*/
			return ex->OP_RIGHT;
		}
		/* "int i = 12;"	*/
		check_ival(ex, tp);
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
	struct expr *ex;
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
		register struct expr *f = ex->OP_LEFT;
		register struct expr *g = 0;

		while (ISCOMMA(f)) {	/* eat the brackets!!!	*/
			g = f;
			f = f->OP_LEFT;
		}
		if (f->ex_class == String) { /* hallelujah, it's a string! */
			init_string(tpp, f);
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
			if (ISCOMMA(ex->OP_LEFT)) {
				/* the member expression is embraced	*/
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
			if (ISCOMMA(ex->OP_LEFT)) {
				/* embraced member initialisation	*/
				if (IVAL(&(tp->tp_up), ex->OP_LEFT) != 0)
					too_many_initialisers(ex);
				ex = ex->OP_RIGHT;
			}
			else {
				if (aggregate_type(tp->tp_up))
					/* the member is an aggregate	*/
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
		if ((ex == 0) && elem_count < dim) {
			/*	the expression tree is completely absorbed
				but there are still members which must be
				initialised with zeroes
			*/
			do
				pad(tp->tp_up);
			while (++elem_count < dim);
		}
	}
	return 0;
}


/*	do_struct() initialises a struct of type tp with the expression expr.
	The main loop is just controlled by the definition of the selectors
	during which alignment is taken care of.
*/
struct expr *
do_struct(ex, tp)
	struct expr *ex;
	struct type *tp;
{
	struct sdef *sd = tp->tp_sdef;
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
				else {
					/* fundamental type, not embraced */
					check_ival(ex->OP_LEFT,
							sd->sd_type);
					ex = ex->OP_RIGHT;
				}
#endif NOBITFIELD
			}
		}
		/* align upto the next selector	boundary	*/
		if (sd->sd_sdef)
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
		/*	there are selectors left which must be padded with
			zeroes
		*/
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
	struct expr *ex;
	struct type **tpp;
{
	/* ex is of a fundamental type	*/
	struct type *tp = *tpp;

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
			register dim = tp->tp_size / tp->tp_up->tp_size;
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
		/* Next selector is aligned by adding extra zeroes */
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
	struct type *tp;
{
	switch (tp->tp_fund) {
	case ARRAY:
	{
		register long dim;

		if (valid_type(tp->tp_up, "array element") == 0)
			return;

		dim = tp->tp_size / tp->tp_up->tp_size;

		/* Assume the dimension is known	*/
		while (dim-- > 0)
			pad(tp->tp_up);
		break;
	}
	case STRUCT:
	{
		register struct sdef *sdef = tp->tp_sdef;

		if (valid_type(tp, "struct") == 0)
			return;

		do {
			pad(sdef->sd_type);
			if (sdef->sd_sdef)
				zero_bytes(sdef);
		} while (sdef = sdef->sd_sdef);
		break;
	}
#ifndef NOBITFIELD
	case FIELD:
		put_bf(tp, (arith)0);
		break;
#endif NOBITFIELD
	case INT:
	case SHORT:
	case LONG:
	case CHAR:
	case ENUM:
	case POINTER:
		C_con_ucon("0",  tp->tp_size);
		break;
	case FLOAT:
	case DOUBLE:
		C_con_fcon("0", tp->tp_size);
		break;
	case UNION:
		error("initialisation of unions not allowed");
		break;
	case ERRONEOUS:
		break;
	default:
		crash("(generate) bad fundamental type %s\n",
			symbol2str(tp->tp_fund));
	}
}

/*	check_ival() checks whether the initialisation of an element
	of a fundamental type is legal and, if so, performs the initialisation
	by directly generating the necessary code.
	No further comment is needed to explain the internal structure
	of this straightforward function.
*/
check_ival(ex, tp)
	struct expr *ex;
	struct type *tp;
{
	/*	The philosophy here is that ch7cast puts an explicit
		conversion node in front of the expression if the types
		are not compatible.  In this case, the initialisation
		expression is no longer a constant.
	*/
	
	switch (tp->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
	case POINTER:
		ch7cast(&ex, '=', tp);
#ifdef DEBUG
		print_expr("init-expr after cast", ex);
#endif DEBUG
		if (!is_ld_cst(ex))
			illegal_init_cst(ex);
		else
		if (ex->VL_CLASS == Const)
			con_int(ex);
		else
		if (ex->VL_CLASS == Name) {
			register struct idf *id = ex->VL_IDF;
			register struct def *df = id->id_def;

			if (df->df_level >= L_LOCAL)
				illegal_init_cst(ex);
			else	/* e.g., int f(); int p = f; */
			if (df->df_type->tp_fund == FUNCTION)
				C_con_pnam(id->id_text);
			else	/* e.g., int a; int *p = &a; */
				C_con_dnam(id->id_text, ex->VL_VALUE);
		}
		else {
			ASSERT(ex->VL_CLASS == Label);
			C_con_dlb(ex->VL_LBL, ex->VL_VALUE);
		}
		break;
	case FLOAT:
	case DOUBLE:
		ch7cast(&ex, '=', tp);
#ifdef DEBUG
		print_expr("init-expr after cast", ex);
#endif DEBUG
		if (ex->ex_class == Float)
			C_con_fcon(ex->FL_VALUE, ex->ex_type->tp_size);
		else
		if (ex->ex_class == Oper && ex->OP_OPER == INT2FLOAT) {
			/* float f = 1; */
			ex = ex->OP_RIGHT;
			if (is_cp_cst(ex))
				C_con_fcon(
					long2str((long)ex->VL_VALUE, 10),
					tp->tp_size
				);
			else 
				illegal_init_cst(ex);
		}
		else
			illegal_init_cst(ex);
		break;

#ifndef NOBITFIELD
	case FIELD:
		ch7cast(&ex, '=', tp->tp_up);
#ifdef DEBUG
		print_expr("init-expr after cast", ex);
#endif DEBUG
		if (is_cp_cst(ex))
			put_bf(tp, ex->VL_VALUE);
		else
			illegal_init_cst(ex);
		break;
#endif NOBITFIELD

	case ERRONEOUS:
		break;
	default:
		crash("check_ival");
	}
}

/*	init_string() initialises an array of characters by specifying
	a string constant.
	Alignment is taken care of.
*/
init_string(tpp, ex)
	struct type **tpp;	/* type tp = array of characters	*/
	struct expr *ex;
{
	register struct type *tp = *tpp;
	register arith length;
	char *s = ex->SG_VALUE;
	arith ntopad;

	ASSERT(ex->ex_class == String);
	length = ex->SG_LEN;
	if (tp->tp_size == (arith)-1)	{
		/* set the dimension	*/
		tp = *tpp = construct_type(ARRAY, tp->tp_up, length);
		ntopad = align(tp->tp_size, word_align) - tp->tp_size;
	}
	else {
		arith dim = tp->tp_size / tp->tp_up->tp_size;

		ntopad = align(dim, word_align) - length;
		if (length > dim)
			expr_error(ex,
				"too many characters in initialiser string");
	}
	/* throw out the characters of the already prepared string	*/
	do
		C_con_ucon(long2str((long)*s++ & 0xFF, 10), (arith)1);
	while (--length > 0);
	/* pad the allocated memory (the alignment has been calculated)	*/
	while (ntopad-- > 0)
		con_nullbyte();
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
	struct sdef *sd;
{
	/*	fills the space between a selector of a struct
		and the next selector of that struct with zero-bytes.
	*/
	register int n =
		sd->sd_sdef->sd_offset - sd->sd_offset -
		size_of_type(sd->sd_type, "struct member");
	register count = n;

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
	struct type *tp;
{
	return tp->tp_fund == ARRAY || tp->tp_fund == STRUCT;
}
