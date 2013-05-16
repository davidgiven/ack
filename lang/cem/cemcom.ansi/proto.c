/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*  P R O T O T Y P E   F I D D L I N G  */

#include	"parameters.h"
#include	<alloc.h>
#include    "idf.h"
#include	"Lpars.h"
#include	"level.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"align.h"
#include	"stack.h"
#include	"def.h"
#include	"type.h"
#include	"struct.h"
#include	"label.h"
#include	"expr.h"
#include	"declar.h"
#include	"decspecs.h"
#include	"proto.h"
#include	"assert.h"

extern char options[];

check_for_void(pl)
	register struct proto *pl;
{
	register int errcnt = 0;

	if (!pl) return;
	if ((pl->pl_flag & PL_VOID) && !(pl->next)) return;

	while (pl) {
		if (pl->pl_flag & PL_VOID) {
			if (!errcnt && !(pl->pl_flag & PL_ERRGIVEN))
				error("illegal use of void in argument list");
			pl->pl_flag |= PL_ERRGIVEN;
			errcnt++;
		}
		pl = pl->next;
	}
}

add_proto(pl, ds, dc, lvl)
	struct proto *pl;
	struct decspecs *ds;
	struct declarator *dc;
	int lvl;
{
	/*	The full typed identifier or abstract type, described
		by the structures decspecs and declarator are turned
		a into parameter type list structure.
		The parameters will be declared at level L_FORMAL2,
		later on it's decided whether they were prototypes
		or actual declarations.
	*/
	register struct idf *idf = dc->dc_idf;
	register struct def *def = idf ? idf->id_def : (struct def *)0;
	register int sc = ds->ds_sc;
	register struct type *type;
	char formal_array = 0;

	ASSERT(ds->ds_type != (struct type *)0);

	pl->pl_flag = PL_FORMAL;
	type = declare_type(ds->ds_type, dc);
	if (type->tp_size < (arith)0 && actual_declaration(sc, type)) {
		extern char *symbol2str();
		if (type->tp_fund != VOID)
			error("unknown %s-type", symbol2str(type->tp_fund));
		else {
			if (idf != (struct idf *)0
			    || ds->ds_sc_given
			    || ds->ds_typequal) {
				error("illegal use of void in argument list");
				pl->pl_flag |= PL_ERRGIVEN;
			}
			/* set PL_VOID anyway */
			pl->pl_flag |= PL_VOID;
		}
	}
	if (ds->ds_sc_given && ds->ds_sc != REGISTER) {
		if (!(pl->pl_flag & PL_ERRGIVEN)) {
		    if (ds->ds_sc != AUTO) {
			error("illegal storage class in parameter declaration");
		    } else {
			warning("illegal storage class in parameter declaration");
		    }
		}
	}

	/*	Perform some special conversions for parameters.
	*/
	if (type->tp_fund == FUNCTION) {
		type = construct_type(POINTER, type, 0, (arith) 0, NO_PROTO);
	} else if (type->tp_fund == ARRAY) {
		type = construct_type(POINTER, type->tp_up, 0, (arith) 0, NO_PROTO);
		formal_array = 1;
	}

	/*	According to the standard we should ignore the storage
		class of a parameter, unless it's part of a function
		definition.
		However, in the routine declare_protos we don't know decspecs,
		and therefore we can't complain up there. So we build up the
		storage class, and keep quiet until we reach declare_protos.
	*/
	sc = (ds->ds_sc_given && ds->ds_sc != REGISTER) ?
				0 : sc == 0 ? FORMAL : REGISTER;

	if (def && (def->df_level == lvl /* || def->df_level < L_PROTO */ )) {
		/* redeclaration at the same level */
		error("parameter %s redeclared", idf->id_text);
	} else if (idf != (struct idf *)0) {
		/*	New definition, redefinition hides earlier one
		*/
		register struct def *newdef = new_def();
		
		newdef->next = def;
		newdef->df_level = lvl;
		newdef->df_sc = sc;
		newdef->df_type = type;
		newdef->df_formal_array = formal_array;
		newdef->df_file = idf->id_file;
		newdef->df_line = idf->id_line;
#ifdef LINT
		newdef->df_set = (type->tp_fund == ARRAY);
		/* newdef->df_firstbrace = 0; */
#endif
		/*	We can't put the idf onto the stack, since these kinds
			of declaration may occurs at any level, and the idf
			does not necessarily go at this level. E.g.

			f() {
			...
				{ int func(int a, int b);
				...
				}
			}

			The idf's a and b declared in the prototype declaration
			do not go at any level, they are simply ignored.
			However, in

			f(int a, int b) {
			...
			}

			They should go at level L_FORMAL2. But at this stage
			we don't know whether we have a prototype or function
			definition. So, this process is postponed.
		*/
		idf->id_def = newdef;
		update_ahead(idf);
	}

	pl->pl_idf = idf;
	pl->pl_type = type;
}

struct tag *
gettag(tp, idpp)
struct type *tp;
struct idf **idpp;
{
	struct tag *tg = (struct tag *)0;
	register int fund = tp->tp_fund;

        while (fund == FIELD || fund == POINTER
                || fund == ARRAY || fund == FUNCTION) {
                tp = tp->tp_up;
                fund = tp->tp_fund;
        }
	*idpp = tp->tp_idf;
	switch(tp->tp_fund) {
	case ENUM:
	case UNION:
	case STRUCT: tg = tp->tp_idf->id_tag; break;
	}
	return tg;
}


declare_protos(dc)
	register struct declarator *dc;
{
	/*	At this points we know that the idf's in protolist are formal
		parameters. So it's time to declare them at level L_FORMAL2.
	*/
	struct stack_level *stl = stack_level_of(L_FORMAL1);
	register struct decl_unary *du;
	register struct type *type;
	register struct proto *pl;
	register struct def *def;

#ifdef	DEBUG
	if (options['t'])
		dumpidftab("start declare_protos", 0);
#endif	/* DEBUG */
	du = dc->dc_decl_unary;
	while (du) {
		if (du->du_fund == FUNCTION) {
			if (du->next != (struct decl_unary *) 0) {
				remove_proto_idfs(du->du_proto);
				du->du_proto = 0;
			} else break;
		}
		du = du->next;
	}
	pl = du ? du->du_proto : NO_PROTO;
	if (pl) {
#if	0 /* the id_proto member is deleted (???) */
		idf->id_proto = 0;
#endif	/* 0 */
		do {
			struct tag *tg;
			struct idf *idp = 0;

			type = pl->pl_type;

			/* `...' only for type checking */
			if (pl->pl_flag & PL_ELLIPSIS) {
				pl = pl->next;
				continue;
			}

			/* special case: int f(void) { ; } */
			if (type->tp_fund == VOID)
				break;

			if (!pl->pl_idf || !(def = pl->pl_idf->id_def)) {
				error("no parameter identifier supplied");
				pl = pl->next;
				continue;
			}

			/*	Postponed storage class checking.
			*/
			if (def->df_sc == 0)
				error("illegal storage class in parameter declaration");

			def->df_level = L_FORMAL2;
			stack_idf(pl->pl_idf, stl);
			pl = pl->next;

			tg = gettag(type, &idp);
			if (tg && tg->tg_level <= L_PROTO) {
				tg->tg_level = L_FORMAL2;
				stack_idf(idp, stl);
			}
		} while (pl);
	}
#ifdef	DEBUG
	if (options['t'])
		dumpidftab("end declare_protos", 0);
#endif	/* DEBUG */
}


update_proto(tp, otp)
	register struct type *tp, *otp;
{
	/*	This routine performs the proto type updates.
		Consider the following code:

		int f(double g());
		int f(double g(int f(), int));
		int f(double g(int f(long double), int));

		The most accurate definition is the third line.
		This routine will silently update all lists,
		and removes the redundant occupied space.
	*/
	register struct proto *pl, *opl;

	if (tp == otp) return;
	if (!tp || !otp) return;

	while (tp->tp_fund != FUNCTION) {
		if (tp->tp_fund != POINTER && tp->tp_fund != ARRAY) return;
		tp = tp->tp_up;
		otp = otp->tp_up;
		if (!tp) return;
	}

	pl = tp->tp_proto;
	opl = otp->tp_proto;
	if (pl && opl) {
		/* both have prototypes */
		while (pl && opl) {
			update_proto(pl->pl_type, opl->pl_type);
			pl = pl->next;
			opl = opl->next;
		}
		/* Do not free the old prototype list. It might be part of
		 * a typedef.
		 */
		otp->tp_proto = tp->tp_proto;
	} else if (opl) {
		/* old decl has type */
	} else if (pl) {
		otp->tp_proto = pl;
	}

	update_proto(tp->tp_up, otp->tp_up);
}

/* struct/union and enum tags can be declared inside prototypes
 * remove them from the symbol-table
 */
remove_proto_tag(tp)
struct type *tp;
{
	register struct idf *ident;
	register struct tag *tgp, **tgpp;
	register int fund = tp->tp_fund;

	while (fund == FIELD || fund == POINTER
		|| fund == ARRAY || fund == FUNCTION) {
		tp = tp->tp_up;
		fund = tp->tp_fund;
	}

	ident = tp->tp_idf;
	switch (tp->tp_fund) {
	case ENUM:
	case STRUCT:
	case UNION: tgpp = &(ident->id_tag); break;
	default: return;
	}

	while((*tgpp) && (*tgpp)->tg_type != tp) {
		tgpp = &((*tgpp)->next);
	}
	if (!*tgpp) return;

	tgp = *tgpp;
	if (tgp->tg_level > L_PROTO) return;

#ifdef DEBUG
	if (options['t'])
		print("Removing idf %s from list\n",
			ident->id_text);
#endif

	(*tgpp) = tgp->next;
	free_tag(tgp);
}

remove_proto_idfs(pl)
	register struct proto *pl;
{
	/*	Remove all the identifier definitions from the
		prototype list.
	*/
	register struct def *def;

	while (pl) {
		if (pl->pl_idf) {
#ifdef DEBUG
			if (options['t'])
				print("Removing idf %s from list\n",
					pl->pl_idf->id_text);
#endif
			def = pl->pl_idf->id_def;
			if (def && def->df_level <= L_PROTO) {
				pl->pl_idf->id_def = def->next;
				free_def(def);
			}
			pl->pl_idf = (struct idf *) 0;
		}
		if (pl->pl_type) {
			remove_proto_tag(pl->pl_type);
		}
		pl = pl->next;
	}
}

call_proto(expp)
	register struct expr **expp;
{
	/*	If the function specified by (*expp)->OP_LEFT has a prototype,
		the parameters are converted according the rules specified in
		par. 3.3.2.2. E.i. the parameters are converted to the prototype
		counter parts as if by assignment. For the parameters falling
		under ellipsis clause the old parameters conversion stuff
		applies.
	*/
	register struct expr *left = (*expp)->OP_LEFT;
	register struct expr *right = (*expp)->OP_RIGHT;
	register struct proto *pl = NO_PROTO;
	static struct proto ellipsis = { 0, 0, 0, PL_ELLIPSIS };

	if (left != NILEXPR) {		/* in case of an error */
		register struct type *tp = left->ex_type;

		while (tp && tp->tp_fund != FUNCTION && tp != error_type)
			tp = tp->tp_up;
		if (tp && tp->tp_proto)
			pl = tp->tp_proto;
	}

	if (right != NILEXPR) {		/* function call with parameters */
		register struct expr **ep = &((*expp)->OP_RIGHT);
		register int ecnt = 0, pcnt = 0;
		struct expr **estack[NPARAMS];
		struct proto *pstack[NPARAMS];

		/* stack up the parameter expressions */
		while (right->ex_class == Oper && right->OP_OPER == PARCOMMA) {
			if (ecnt == STDC_NPARAMS)
				expr_strict(right, "number of parameters exceeds ANSI limit");
			if (ecnt >= NPARAMS-1) {
				expr_error(right, "too many parameters");
				return;
			}
			estack[ecnt++] = &(right->OP_RIGHT);
			ep = &(right->OP_LEFT);
			right = right->OP_LEFT;
		}
		estack[ecnt] = ep;

		/*	Declarations like int f(void) do not expect any
			parameters.
		*/
		if (pl && pl->pl_flag & PL_VOID) {
			expr_strict(*expp, "no parameters expected");
			pl = NO_PROTO;
		}

		/* stack up the prototypes */
		if (pl) {
			pcnt--;
			do {
				/* stack prototypes */
				pstack[++pcnt] = pl;
				pl = pl->next;
			} while (pl);
		}
		else {
			pstack[0] = &ellipsis;
		}

		for (ecnt; ecnt >= 0; ecnt--) {
			/*	Only the parameters specified in the prototype
				are checked and converted. The parameters that
				fall under the ellipsis clause are neither
				checked nor converted !
			*/
			if (pcnt < 0) {
				expr_error(*expp, "more parameters than specified in prototype");
				break;
			}
			else if (!(pstack[pcnt]->pl_flag & PL_ELLIPSIS)) {
				ch3cast(estack[ecnt],CASTAB,pstack[pcnt]->pl_type);
				pcnt--;
			} else
				any2parameter(estack[ecnt]);
		}
		if (pcnt > 0 || (pcnt == 0 && !(pstack[0]->pl_flag & PL_ELLIPSIS)))
			expr_error(*expp, "fewer parameters than specified in prototype");
	} else {
		if (pl && !(pl->pl_flag & PL_VOID))
			expr_error(*expp, "fewer parameters than specified in prototype");
	}
}
