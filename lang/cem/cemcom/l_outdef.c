/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	Lint outdef construction	*/

#include	"lint.h"

#ifdef	LINT

#include	<alloc.h>
#include	"interface.h"
#ifdef ANSI
#include	<flt_arith.h>
#endif /* ANSI */
#include	"arith.h"
#include	"assert.h"
#include	"type.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"stack.h"
#include	"def.h"
#include	"struct.h"
#include	"field.h"
#include	"idf.h"
#include	"level.h"
#include	"label.h"
#include	"code.h"
#include	"expr.h"
#include	"l_lint.h"
#include	"l_comment.h"
#include	"l_outdef.h"
#include	"l_class.h"

extern char *bts2str();
extern char *symbol2str();
extern char *strindex();

int stat_number = 9999;			/* static scope number */
struct outdef OutDef;

PRIVATE struct outdef OutCall;

PRIVATE local_EFDC();
PRIVATE output_def();
PRIVATE outargs();
PRIVATE outarg();
PRIVATE outargstring();
PRIVATE outargtype();
PRIVATE add_expr_arg();

lint_declare_idf(idf, sc)
	struct idf *idf;
	int sc;
{
	register struct def *def = idf->id_def;
	register int is_function = def->df_type->tp_fund == FUNCTION;

	if (level == L_GLOBAL) {
		lint_ext_def(idf, sc);
		if (is_function)
			def2decl(sc);
		if (sc != TYPEDEF)
			outdef();
	}
	else
	if (level >= L_LOCAL && sc != STATIC && is_function) {
		local_EFDC(idf);
	}
}

lint_non_function_decl(ds, dc)
	struct decspecs *ds;
	struct declarator *dc;
{
	register struct def *def = dc->dc_idf->id_def;
	register int is_function = def->df_type->tp_fund == FUNCTION;

	if (is_function)
		def2decl(ds->ds_sc);
	if (def->df_sc != TYPEDEF)
		outdef();
}

lint_ext_def(idf, sc)
	struct idf *idf;
{
/* At this place the following fields of the output definition can be
 * filled:
 *		name, stat_number, class, file, line, type.
 * For variable definitions and declarations this will be all.
 * For functions the fields nrargs and argtps are filled after parsing
 * the arguments.
 * The returns-field is known at the end of the function definition.
 * sc indicates the storage class defined by the declaration specifier.
 */
	register struct def *def = idf->id_def;
	register struct type *type = def->df_type;

	OutDef.od_name = idf->id_text;
	OutDef.od_statnr = (sc == STATIC ? stat_number : 0);

	switch (type->tp_fund) {
	case ERRONEOUS:
		OutDef.od_class = XXDF;
		break;
	case FUNCTION:
		/* For the moment assume it will be a definition.
		 * If no compound_statement follows, it is a declaration,
		 * in which case the class will be adjusted by def2decl().
		 */
		OutDef.od_class = (sc == STATIC ? SFDF : EFDF);
		break;
	default:	/* a variable */
		OutDef.od_class =
			sc == EXTERN ? EVDC :
			sc == STATIC ? SVDF : EVDF;
		break;
	}
	OutDef.od_file = def->df_file;
	OutDef.od_line = def->df_line;
	OutDef.od_type = (type->tp_fund == FUNCTION ? type->tp_up : type);
	OutDef.od_valreturned = NORETURN;
}

def2decl(sc)
	int sc;
{
/* It was assumed we were parsing a function definition.
 * There was no compound statement following, so actually it was a
 * declaration. This function updates the class.
 */
	OutDef.od_class = (sc == STATIC ? XXDF : EFDC);
}

set_od_valreturned(n)
{
	OutDef.od_valreturned = n;
}

PRIVATE
local_EFDC(idf)
	struct idf *idf;
{
	struct outdef od;

	od.od_class = EFDC;
	od.od_statnr = 0;
	od.od_name = idf->id_text;
	od.od_file = idf->id_def->df_file;
	od.od_line = idf->id_def->df_line;
	od.od_type = idf->id_def->df_type->tp_up;
	output_def(&od);
	/* The other fields are not used for this class. */
}

lint_formals()
{
/* Make a list of 'struct argument's containing the types of the formal
 * parameters of the function definition just parsed.
 */
	register struct stack_entry *se = stack_level_of(L_FORMAL1)->sl_entry;
	register struct argument **hook = &OutDef.od_arg;
	register int nrargs = 0;

	while (se) {
		register struct type *type = se->se_idf->id_def->df_type;
		register struct argument *arg = new_argument();

		/*	Do the conversions on the formals that could not be
			done in declare_idf().
			It is, unfortunately, impossible not to do them,
			since the corresponding actuals will have been
			converted to generate proper code and we do not
			want to duplicate the whole of expression handling
			for lint.
		*/
		switch (type->tp_fund) {
		case CHAR:
		case SHORT:
			type = (type->tp_unsigned ? uint_type : int_type);
			break;
		case FLOAT:
			type = double_type;
			break;
		}

		if (f_FORMAT && nrargs == f_FORMATn) {
			if (	!f_FORMATvar
			&&	(	type->tp_fund != POINTER
				||	type->tp_up->tp_fund != CHAR
				)
			) {
				warning("format parameter %d is not pointer to char",
					nrargs);
			}
			arg->ar_type = string_type;
			arg->ar_class = ArgString;
			arg->CAS_VALUE = f_FORMAT;
			arg->CAS_LEN = strlen(f_FORMAT);
			f_FORMAT = 0;
		}
		else {
			arg->ar_type = type;
			arg->ar_class = ArgFormal;
		}
		*hook = arg;
		hook = &arg->next;

		nrargs++;
		se = se->next;
	}

	if (f_FORMAT) {
		/*	f_FORMAT has not been consumed, perhaps due to
			a varargs-like construction; add erroneous ArgFormals
			until f_FORMATn, then an ArgString, if necessary.
		*/
		if (!f_FORMATvar) {
			warning("FORMAT%d function has only %d argument%s",
				f_FORMATn, nrargs, nrargs == 1 ? "" : "s"
			);
		}

		while (nrargs < f_FORMATn) {
			register struct argument *arg = new_argument();
			
			arg->ar_type = error_type;
			arg->ar_class = ArgFormal;
			*hook = arg;
			hook = &arg->next;
			nrargs++;
		}
		if (nrargs == f_FORMATn) {
			register struct argument *arg = new_argument();
			
			arg->ar_type = string_type;
			arg->ar_class = ArgString;
			arg->CAS_VALUE = f_FORMAT;
			arg->CAS_LEN = strlen(f_FORMAT);
			f_FORMAT = 0;
			*hook = arg;
			hook = &arg->next;
			nrargs++;
		}
		/* life is full of duplicated code; this is no good */
	}

	if (f_VARARGSn > nrargs) {
		warning("VARARGS%d function has only %d argument%s",
			f_VARARGSn, nrargs, nrargs == 1 ? "" : "s"
		);
		f_VARARGSn = nrargs;
	}
	OutDef.od_nrargs = nrargs;
}

output_use(idf)
	struct idf *idf;
{
/* Output the usage-definition of the variable described by idf.
 */
	OutDef.od_name = idf->id_text;
	OutDef.od_statnr = (idf->id_def->df_sc == STATIC ? stat_number : 0);
	OutDef.od_class = VU;
	OutDef.od_file = FileName;
	OutDef.od_line = LineNumber;
	OutDef.od_type = idf->id_def->df_type;
	outdef();
}

outdef()
{
	output_def(&OutDef);
}

outcall()
{
	output_def(&OutCall);
}

PRIVATE
output_def(od)
	struct outdef *od;
{
/* As the types are output the 'struct argument's are removed, because they
 * are then not needed anymore.
 */
	if (od->od_class == XXDF || !od->od_name || od->od_name[0] == '#')
		return;

	if (LINTLIB) {
		switch (od->od_class) {
		case EFDF:
			od->od_class = LFDF;
			break;
		case EVDF:
			od->od_class = LVDF;
			break;
		case SFDF:
			/* remove 'struct argument's */
			while (od->od_arg) {
				register struct argument *tmp = od->od_arg;
				od->od_arg = od->od_arg->next;
				free_argument(tmp);
			}
			return;
		default:
			return;
		}
	}
	printf("%s:%d:%c", od->od_name, od->od_statnr, od->od_class);
	switch (od->od_class) {
	case EFDF:
	case SFDF:
	case LFDF:
		if (f_VARARGSn != -1) {
			printf(":%d", -1 - f_VARARGSn);
			outargs(od->od_arg, f_VARARGSn);
		}
		else {
			printf(":%d", od->od_nrargs);
			outargs(od->od_arg, od->od_nrargs);
		}
		od->od_arg = 0;
		printf(":%d", od->od_valreturned);
		break;
	case FC:
		printf(":%d", od->od_nrargs);
		outargs(od->od_arg, od->od_nrargs);
		od->od_arg = 0;
		printf(":%d", od->od_valused);
		break;
	case EVDF:
	case SVDF:
	case LVDF:
	case EFDC:
	case EVDC:
	case IFDC:
	case VU:
		break;
	default:
		NOTREACHED();
		/*NOTREACHED*/
	}
	printf(":");
	outargtype(od->od_type);
	printf(":%u:%s\n", od->od_line, od->od_file);
}

PRIVATE
outargs(arg, n)
	struct argument *arg;
{
/* Output the n arguments in the argument list and remove them */

	register struct argument *tmp;

	while (n--) {
		ASSERT(arg);
		outarg(arg);
		tmp = arg;
		arg = arg->next;
		free_argument(tmp);
	}
	/* remove the remaining entries */
	while (arg) {
		tmp = arg;
		arg = arg->next;
		free_argument(tmp);
	}
}

PRIVATE
outarg(arg)
	struct argument *arg;
{
	printf(":");
	switch (arg->ar_class) {
	case ArgConst:
		if (arg->CAA_VALUE >= 0) {
			/* constant non-negative actual parameter */
			printf("+");
		}
		outargtype(arg->ar_type);
		break;

	case ArgString:
		outargstring(arg);
		break;

	case ArgFormal:
	case ArgExpr:
		outargtype(arg->ar_type);
		if (arg->ar_type->tp_fund == FUNCTION) {
			/* UGLY PATCH !!! ??? */
			/*	function names as operands are sometimes
				FUNCTION and sometimes POINTER to FUNCTION,
				depending on opaque circumstances.  E.g., in
					f(main, main);
				the first main is PtF and the second is F.
			*/
			printf("*");
		}
		break;

	default:
		NOTREACHED();
		/*NOTREACHED*/
	}
}

PRIVATE
outargstring(arg)
	struct argument *arg;
{
	char buff[1000];
	register char *p;

	bts2str(arg->CAS_VALUE, arg->CAS_LEN, buff);
	for (p = &buff[0]; *p; p++) {
		if (*p == '"' || *p == ':')
			*p = ' ';
	}
	printf("\"%s\"", buff);
}

PRIVATE
outargtype(tp)
	struct type *tp;
{
	switch (tp->tp_fund) {
	case POINTER:
		outargtype(tp->tp_up);
		printf("*");
		break;

	case ARRAY:
		outargtype(tp->tp_up);
		printf("*");	/* compatible with [] */
		break;

	case FUNCTION:
		outargtype(tp->tp_up);
		printf("()");
		break;

	case STRUCT:
	case UNION:
	case ENUM:
		/* watch out for anonymous identifiers; the count field does
		   not have to be the same for all compilation units.
		   Remove it, so that pass 2 does not see it. The only
		   problem with this is that pass2 will not see a difference
		   between two non-tagged types declared on the same line.
		*/
		printf("%s ", symbol2str(tp->tp_fund));
		if (is_anon_idf(tp->tp_idf)) {
			/* skip the #<num>, replace it by '#anonymous id' */
			printf("#anonymous id%s", strindex(tp->tp_idf->id_text, ' '));
		}
		else printf(tp->tp_idf->id_text);
		break;

	case CHAR:
	case INT:
	case SHORT:
	case LONG:
	case FLOAT:
	case DOUBLE:
	case VOID:
	case ERRONEOUS:
		if (tp->tp_unsigned)
			printf("unsigned ");
		printf("%s", symbol2str(tp->tp_fund));
		break;
	default:
		NOTREACHED();
		/*NOTREACHED*/
	}
}

#ifdef	IMPLICIT
PRIVATE
implicit_func_decl(idf, file, line)
	struct idf *idf;
	char *file;
	unsigned int line;
{
	struct outdef od;

	od.od_class = IFDC;
	od.od_statnr = 0;
	od.od_name = idf->id_text;
	od.od_file = file;
	od.od_line = line;
	od.od_type = idf->id_def->df_type->tp_up;
	output_def(&od);
	/* The other fields are not used for this class. */
}
#endif	/* IMPLICIT */

fill_outcall(ex, used)
	struct expr *ex;
	int used;
{
	register struct idf *idf = ex->OP_LEFT->VL_IDF;
	register struct def *def = idf->id_def;

#ifdef	IMPLICIT
	if (def->df_sc == IMPLICIT && !idf->id_def->df_used) {
		/* IFDC, first time */
		implicit_func_decl(idf, ex->ex_file, ex->ex_line);
	}
#endif	/* IMPLICIT */

	OutCall.od_type = def->df_type->tp_up;
	OutCall.od_statnr = (def->df_sc == STATIC ? stat_number : 0);
	OutCall.od_class = FC;
	OutCall.od_name = idf->id_text;
	OutCall.od_file = ex->ex_file;
	OutCall.od_line = ex->ex_line;
	OutCall.od_arg = (struct argument *)0;
	OutCall.od_nrargs = 0;

	if ((ex = ex->OP_RIGHT) != 0) {
		/* function call with arguments */
		/* store types of argument expressions in 'struct argument's */
		while (ex->ex_class == Oper && ex->OP_OPER == PARCOMMA) {
			add_expr_arg(ex->OP_RIGHT);
			ex = ex->OP_LEFT;
		}
		add_expr_arg(ex);
	}
	OutCall.od_valused = used;	/* USED, IGNORED or VOIDED */
}

PRIVATE
add_expr_arg(e)
	struct expr *e;
{
	register struct argument *arg;

	arg = new_argument();
	arg->ar_type = e->ex_type;
	if (is_cp_cst(e)) {
		arg->ar_class = ArgConst;
		arg->CAA_VALUE = e->VL_VALUE;
	}
	else if (	e->ex_type == string_type
		&&	e->ex_class == Value
		&&	e->VL_CLASS == Label
		) {
		/* it may be a string; let's look it up */
		register struct string_cst *sc = str_list;

		while (sc) {
			if (sc->sc_dlb == e->VL_LBL)
				break;
			sc = sc->next;
		}
		if (sc) {
			/* it was a string */
			arg->ar_class = ArgString;
			arg->CAS_VALUE = sc->sc_value;
			arg->CAS_LEN = sc->sc_len - 1;	/* included the \0 */
		}
		else {
			arg->ar_class = ArgExpr;
		}
	}
	else {
		arg->ar_class = ArgExpr;
	}
	arg->next = OutCall.od_arg;
	OutCall.od_arg = arg;
	OutCall.od_nrargs++;
}

#endif	/* LINT */
