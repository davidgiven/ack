/*	T Y P E   D E F I N I T I O N   M E C H A N I S M	 */

#include    "parameters.h"
#include	"debug.h"

#include	<alloc.h>
#include	<assert.h>
#include	<em.h>

#include	<pc_file.h>

#include	"LLlex.h"
#include	"const.h"
#include	"chk_expr.h"
#include	"def.h"
#include	"idf.h"
#include	"main.h"
#include	"node.h"
#include	"scope.h"
#include	"lookup.h"
#include	"type.h"
#include	"typequiv.h"
#include	"error.h"
#ifdef DBSYMTAB
#include	"stab.h"
#endif

#ifndef NOCROSS
int
	word_align	= AL_WORD,
	int_align	= AL_INT,
	long_align	= AL_LONG,
	pointer_align	= AL_POINTER,
	real_align	= AL_REAL,
	struct_align	= AL_STRUCT;

arith
	word_size	= SZ_WORD,
	int_size	= SZ_INT,
	long_size	= SZ_LONG,
	pointer_size	= SZ_POINTER,
	real_size	= SZ_REAL;
#endif /* NOCROSS */

extern arith	max_int;

struct type
	*bool_type,
	*char_type,
	*int_type,
	*long_type,
	*real_type,
	*string_type,
	*std_type,
	*text_type,
	*nil_type,
	*emptyset_type,
	*void_type,
	*error_type;


/* Local forward declarations */
static arith ArrayElSize(register struct type *, int);
static void FreeForward(register struct forwtype *);
static int gcd(int, int);



static void CheckTypeSizes(void)
{
	/* first, do some checking
	*/
	if( int_size != word_size )
		fatal("integer size not equal to word size");
	if( word_size != 2 && word_size != 4 )
		fatal("illegal wordsize");
	if( pointer_size != 2 && pointer_size != 4 )
		fatal("illegal pointersize");
	if( options['d'] ) {
		if( long_size < int_size )
			fatal("longsize should be at least the integersize");
		if( long_size > 2 * int_size)
			fatal("longsize should be at most twice the integersize");
	}
	if( pointer_size < word_size )
		fatal("pointersize should be at least the wordsize");
	if( real_size != 4 && real_size != 8 )
		fatal("illegal realsize");
}

void InitTypes(void)
{
	/* First check the sizes of some basic EM-types
	*/
	CheckTypeSizes();
	if( options['s'] ) {
		options['c'] = 0;
		options['d'] = 0;
		options['u'] = 0;
		options['C'] = 0;
		options['U'] = 0;
	}

	/*	Initialize the predefined types
	*/

	/* character type
	*/
	char_type = standard_type(T_CHAR, 1, (arith) 1);
	char_type->enm_ncst = 256; /* all bytes */

	/* boolean type
	*/
	bool_type = standard_type(T_ENUMERATION, 1, (arith) 1);
	bool_type->enm_ncst = 2;

	/* integer type
	*/
	int_type = standard_type(T_INTEGER, int_align, int_size);

	/* real type
	*/
	real_type = standard_type(T_REAL, real_align, real_size);

	/* long type
	*/
	if( options['d'] )
		long_type = standard_type(T_LONG, long_align, long_size);

	/* string type
	*/
	if( options['c'] )
		string_type = standard_type(T_STRING, pointer_align, pointer_size);

	/* an unique type for standard procedures and functions
	*/
	std_type = construct_type(T_PROCEDURE, NULLTYPE);

	/* text (file of char) type
	*/
	text_type = construct_type(T_FILE, char_type);
	text_type->tp_flags |= T_HASFILE;

	/* an unique type indicating an error
	*/
	error_type = standard_type(T_ERROR, 1, (arith) 1);
	void_type = error_type;

	/* the nilvalue has an unique type
	*/
	nil_type = construct_type(T_POINTER, error_type);

	/* the type of an empty set is generic
	*/
	emptyset_type = construct_type(T_SET, error_type);
	emptyset_type->tp_size = word_size;
	emptyset_type->tp_align = word_align;
}

static int fit(arith sz, int nbytes)
{
	return ((sz) + ((arith)0x80<<(((nbytes)-1)*8)) & ~full_mask[(nbytes)]) == 0;
}

struct type *standard_type(int fund, int algn, arith size)
{
	register struct type *tp = new_type();

	tp->tp_fund = fund;
	tp->tp_palign = algn ? algn : 1;
	tp->tp_psize = size;
	tp->tp_align = word_align;
	tp->tp_size = WA(size);

	return tp;
}

struct type *construct_type(int fund, register struct type *tp)
{
	/*	fund must be a type constructor.
	 *	The pointer to the constructed type is returned.
	 */
	register struct type *dtp = new_type();

	switch( dtp->tp_fund = fund )	{
		case T_PROCEDURE:
		case T_FUNCTION:
			dtp->tp_align = pointer_align;
			dtp->tp_size = 2 * pointer_size;
			break;

		case T_POINTER:
			dtp->tp_align = dtp->tp_palign = pointer_align;
			dtp->tp_size = dtp->tp_psize = pointer_size;
			break;

		case T_SET:
		case T_ARRAY:
			break;

		case T_FILE:
			dtp->tp_align = dtp->tp_palign = word_align;
			dtp->tp_size = dtp->tp_psize = sizeof(struct file);
			break;

		case T_SUBRANGE:
			assert(tp != 0);
			dtp->tp_align = tp->tp_align;
			dtp->tp_size = tp->tp_size;
			dtp->tp_palign = tp->tp_palign;
			dtp->tp_psize = tp->tp_psize;
			break;

		default:
			crash("funny type constructor");
	}

	dtp->next = tp;
	return dtp;
}

struct type *proc_type(struct paramlist *parameters, arith n_bytes_params)
{
	register struct type *tp = construct_type(T_PROCEDURE, NULLTYPE);

	tp->prc_params = parameters;
	tp->prc_nbpar = n_bytes_params;
	return tp;
}

struct type *func_type(struct paramlist * parameters, arith n_bytes_params, struct type *resulttype)
{
	register struct type *tp = construct_type(T_FUNCTION, resulttype);

	tp->prc_params = parameters;
	tp->prc_nbpar = n_bytes_params;
	return tp;
}

void chk_type_id(register struct type **ptp, register struct node *nd)
{
	register struct def *df;

	*ptp = error_type;
	if( ChkLinkOrName(nd) )	{
		if( nd->nd_class != Def )
			node_error(nd, "type expected");
		else	{
			/* register struct def *df = nd->nd_def; */
			df = nd->nd_def;

			df->df_flags |= D_USED;
			if( df->df_kind & (D_TYPE | D_FTYPE | D_ERROR) ) {
				if( !df->df_type )
				    node_error(nd, "type \"%s\" not declared",
							df->df_idf->id_text);
				else
				    *ptp = df->df_type;
			}
			else
				node_error(nd,"identifier \"%s\" is not a type",
							df->df_idf->id_text);
		}
	}
}

struct type *subr_type(register struct node *lb, register struct node *ub)
{
	/*	Construct a subrange type from the constant expressions
		indicated by "lb" and "ub", but first perform some checks
	*/

	register struct type *tp = lb->nd_type, *res;

	if( !TstTypeEquiv(lb->nd_type, ub->nd_type) )	{
		node_error(ub, "types of subrange bounds not equal");
		return error_type;
	}

	/* Check base type
	*/
	if( !(tp->tp_fund & T_ORDINAL) )	{
		node_error(ub, "illegal base type for subrange");
		return error_type;
	}

	/* Check bounds
	*/
	if( lb->nd_INT > ub->nd_INT )
		node_error(ub, "lower bound exceeds upper bound");

	/* Now construct resulting type
	*/
	res = construct_type(T_SUBRANGE, tp);
	res->sub_lb = lb->nd_INT;
	res->sub_ub = ub->nd_INT;
	if (res->sub_lb >= 0) {
		if (ufit(res->sub_ub, 1)) {
			res->tp_psize = 1;
			res->tp_palign = 1;
		}
		else if (ufit(res->sub_ub, 2)) {
			res->tp_psize = 2;
			res->tp_palign = 2 < word_align ? 2 : word_align;
		}
	}
	else {
		if (fit(res->sub_lb, 1) && fit(res->sub_ub, 1)) {
			res->tp_psize = 1;
			res->tp_palign = 1;
		}
		else if (fit(res->sub_lb, 2) && fit(res->sub_ub, 2)) {
			res->tp_psize = 2;
			res->tp_palign = 2 < word_align ? 2 : word_align;
		}
	}

	return res;
}

void getbounds(register struct type *tp, arith *plo, arith *phi)
{
	/*	Get the bounds of a bounded type
	*/

	assert(bounded(tp));

	if( tp->tp_fund & T_SUBRANGE )	{
		*plo = tp->sub_lb;
		*phi = tp->sub_ub;
	}
	else if( tp->tp_fund & T_INTEGER ) {
		*plo = -max_int;
		*phi = max_int;
	}
	else {
		*plo = 0;
		*phi = tp->enm_ncst - 1;
	}
}

struct type *set_type(register struct type *tp, unsigned short packed)
{
	/*	Construct a set type with base type "tp", but first
		perform some checks
	*/
	struct type *basetype;
	static struct type *int_set = 0;
	arith lb, ub;

	if( tp == int_type )	{
		/* SET OF INTEGER */
		if( !int_set )	{
			struct node *lbn = new_node();
			struct node *ubn = new_node();

			lbn->nd_type = ubn->nd_type = int_type;
			/* the bounds are implicit */
			lbn->nd_INT = 0;
			ubn->nd_INT = max_intset;

			int_set = subr_type(lbn, ubn);
		}
		lb = 0;
		ub = max_intset;
		tp = int_set;
	}
	else	{
		/* SET OF subrange/enumeration/char */
		if( !bounded(tp) )	{
			error("illegal base type of set");
			return error_type;
		}

		basetype = BaseType(tp);
		if( basetype == int_type )	{
			/* subrange of integers */
			getbounds(tp, &lb, &ub);
			if( lb < 0 || ub > max_intset )	{
				error("illegal integer base type of set");
				return error_type;
			}
			lb = 0;
			ub = max_intset;
		}
		else getbounds(basetype, &lb, &ub);
	}

	assert(lb == 0);
	/* at this point lb and ub denote the bounds of the host-type of the
	 * base-type of the set
	 */

	tp = construct_type(T_SET, tp);
	tp->tp_flags |= packed;

	tp->tp_psize = (ub - lb + 8) >> 3;
	tp->tp_size = WA(tp->tp_psize);
	tp->tp_align = word_align;
	if( !packed || word_size % tp->tp_psize != 0 )	{
		tp->tp_psize = tp->tp_size;
		tp->tp_palign = word_align;
	}
	else tp->tp_palign = tp->tp_psize;

	return tp;
}

static arith ArrayElSize(register struct type *tp, int packed)
{
	/* Align element size to alignment requirement of element type.
	   Also make sure that its size is either a dividor of the word_size,
	   or a multiple of it.
	*/
	register arith algn;

	if( tp->tp_fund & T_ARRAY && !(tp->tp_flags & T_CHECKED) )
		ArraySizes(tp);

	if( !packed )
		return tp->tp_size;

	algn = align(tp->tp_psize, tp->tp_palign);
	if( word_size % algn != 0 )	{
		/* algn is not a dividor of the word size, so make sure it
		   is a multiple
		*/
		algn = WA(algn);
	}
	if( !fit(algn, (int) word_size) ) {
		error("element of array too large");
	}
	return algn;
}

void ArraySizes(register struct type *tp)
{
	/*	Assign sizes to an array type, and check index type
	*/
	register struct type *index_type = IndexType(tp);
	register struct type *elem_type = tp->arr_elem;
	arith lo, hi, diff;

	tp->tp_flags |= T_CHECKED;
	tp->arr_elsize = ArrayElSize(elem_type,(int) IsPacked(tp));

	/* check index type
	*/
	if( !bounded(index_type) )	{
		error("illegal index type");
		tp->tp_psize = tp->tp_size = tp->arr_elsize;
		tp->tp_palign = tp->tp_align = elem_type->tp_align;
		tp->next = error_type;
		return;
	}

	getbounds(index_type, &lo, &hi);
	diff = hi - lo;

	if( diff < 0 || !fit(diff, (int) word_size) ) {
		error("too many elements in array");
	}

	if( (unsigned long)full_mask[(int) pointer_size]/(diff + 1) <
	    tp->arr_elsize ) {
		error("array too large");
	}
	tp->tp_psize = (diff + 1) * tp->arr_elsize;
	tp->tp_palign = (word_size % tp->tp_psize) ? word_align : tp->tp_psize;
	tp->tp_size = WA(tp->tp_psize);
	tp->tp_align = word_align;

	/* generate descriptor and remember label.
	*/
	tp->arr_ardescr = ++data_label;
	C_df_dlb(data_label);
	C_rom_cst(lo);
	C_rom_cst(diff);
	C_rom_cst(tp->arr_elsize);
}

static void FreeForward(register struct forwtype *for_type)
{
	if( !for_type ) return;

	FreeForward(for_type->f_next);
	free_node(for_type->f_node);
	free_forwtype(for_type);
}

void chk_forw_types(void)
{
	/* check all forward references (in pointer types) */

	register struct def *df = CurrentScope->sc_def;
	register struct def *ldf = NULLDEF;
	struct type *tp;

	while( df )	{
		if( df->df_kind & (D_FORWTYPE | D_FTYPE) )	{
		    register struct forwtype *fw_type = df->df_fortype;

		    if( df->df_kind == D_FORWTYPE )	{
			/* forward type not in this scope declared */
			register struct scopelist *scl = nextvisible(CurrVis);
			struct def *df1 = 0;

			while( scl )	{
				/* look in enclosing scopes */
				df1 = lookup(df->df_fortype->f_node->nd_IDF,
					     scl->sc_scope, D_INUSE);
				if( df1 ) break;
				scl = nextvisible( scl );
			}

			if( !df1  || df1->df_kind != D_TYPE ) {
					/* bad forward type */
				tp = error_type;
			}
			else	{	/* ok */
				tp = df1->df_type;

				/* remove the def struct in the current scope */
				if( !ldf )
				      CurrentScope->sc_def = df->df_nextinscope;
				else
				      ldf->df_nextinscope = df->df_nextinscope;

				/* remove the def struct from symbol-table */
				remove_def(df);
			}
		    }
		    else		/* forward type was resolved */
			tp = df->df_type;

		    while( fw_type )	{
			if( tp == error_type )
				node_error(fw_type->f_node,
				  	   "identifier \"%s\" is not a type",
					   df->df_idf->id_text);
			fw_type->f_type->next = tp;
#ifdef DBSYMTAB
			if (options['g']) {
				stb_addtp("(forward_type)", fw_type->f_type);
			}
#endif
			fw_type = fw_type->f_next;
		    }

		    FreeForward( df->df_fortype );
		    df->df_flags |= D_USED;
		    if( tp == error_type )
				df->df_kind = D_ERROR;
		    else
				df->df_kind = D_TYPE;
		}
		ldf = df;
		df = df->df_nextinscope;
	}
}

void TstCaseConstants(register struct node *nd, register struct selector *sel,
		register struct selector *sel1)
{
	/* Insert selector of nested variant (sel1) in tagvalue-table of
	   current selector (sel).
	*/
	while( nd )	{
		if( !TstCompat(nd->nd_type, sel->sel_type) )
			node_error(nd, "type incompatibility in caselabel");
		else if( sel->sel_ptrs )	{
			arith i = nd->nd_INT - sel->sel_lb;

			if( i < 0 || i >= sel->sel_ncst )
				node_error(nd, "case constant: out of bounds");
			else if( sel->sel_ptrs[i] != sel )
				node_error(nd,
				  "record variant: multiple defined caselabel");
			else
				sel->sel_ptrs[i] = sel1;
		}
		nd = nd->nd_next;
	}
}

arith align(arith pos, int al)
{
	arith i;

	return pos + ((i = pos % al) ? al - i : 0);
}

static int gcd(int m, int n)
{
	/*	Greatest Common Divisor
 	*/
	register int r;

	while( n )	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

int lcm(int m, int n)
{
	/*	Least Common Multiple
 	*/
	return m * (n / gcd(m, n));
}

#ifdef DEBUG
void DumpType(register struct type *tp)
{
	if( !tp ) return;

	print("align:%d; size:%ld;", tp->tp_align, (long) tp->tp_size);

	print(" fund:");
	switch( tp->tp_fund )	{
	case T_ENUMERATION:
		print("ENUMERATION; ncst:%d", tp->enm_ncst); break;
	case T_INTEGER:
		print("INTEGER"); break;
	case T_LONG:
		print("LONG"); break;
	case T_REAL:
		print("REAL"); break;
	case T_CHAR:
		print("CHAR"); break;
	case T_STRING:
		print("STRING"); break;
	case T_PROCEDURE:
	case T_FUNCTION:
		{
		register struct paramlist *par = ParamList(tp);

		if( tp->tp_fund == T_PROCEDURE )
			print("PROCEDURE");
		else
			print("FUNCTION");
		if( par )	{
			print("(");
			while( par )	{
				if( IsVarParam(par) ) print("VAR ");
				DumpType(TypeOfParam(par));
				par = par->next;
			}
		}
		break;
		}
	case T_FILE:
		print("FILE"); break;
	case T_STRINGCONST:
		print("STRINGCONST"); break;
	case T_SUBRANGE:
		print("SUBRANGE %ld-%ld", (long) tp->sub_lb, (long) tp->sub_ub);
		break;
	case T_SET:
		print("SET"); break;
	case T_ARRAY:
		print("ARRAY");
		print("; element:");
		DumpType(tp->arr_elem);
		print("; index:");
		DumpType(tp->next);
		print(";");
		return;
	case T_RECORD:
		print("RECORD"); break;
	case T_POINTER:
		print("POINTER"); break;
	default:
		crash("DumpType");
	}
	if( tp->next && tp->tp_fund != T_POINTER )	{
		/* Avoid printing recursive types!
		*/
		print(" next:(");
		DumpType(tp->next);
		print(")");
	}
	print(";");
}
#endif
