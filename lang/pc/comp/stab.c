/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E B U G G E R   S Y M B O L   T A B L E */

/* $Header$ */

#include	"dbsymtab.h"

#ifdef	DBSYMTAB

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<stb.h>

#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"const.h"
#include	"scope.h"
#include	"main.h"
#include	"node.h"

#define INCR_SIZE	64

extern int	proclevel;

extern char	*sprint();

static struct db_str {
	unsigned	sz;
	char		*base;
	char		*currpos;
} db_str;

static
create_db_str()
{
	if (! db_str.base) {
		db_str.base = Malloc(INCR_SIZE);
		db_str.sz = INCR_SIZE;
	}
	db_str.currpos = db_str.base;
}

static
addc_db_str(c)
	int	c;
{
	int df = db_str.currpos - db_str.base;
	if (df >= db_str.sz-1) {
		db_str.sz += INCR_SIZE;
		db_str.base = Realloc(db_str.base, db_str.sz);
		db_str.currpos = db_str.base + df;
	}
	*db_str.currpos++ = c;
	*db_str.currpos = '\0';
}

static
adds_db_str(s)
	char	*s;
{
	while (*s) addc_db_str(*s++);
}

static
stb_type(tp, assign_num)
	register struct type	*tp;
{
	char buf[128];
	static int	stb_count;

	if (tp->tp_dbindex > 0) {
		adds_db_str(sprint(buf, "%d", tp->tp_dbindex));
		return;
	}
	if (tp->tp_dbindex < 0) {
		if (tp->next == 0) {
			adds_db_str(sprint(buf, "%d", -tp->tp_dbindex));
			return;
		}
		tp->tp_dbindex = -tp->tp_dbindex;
	}
	if (tp->tp_dbindex == 0 && assign_num) {
		tp->tp_dbindex = ++stb_count;
	}
	if (tp->tp_dbindex > 0) {
		adds_db_str(sprint(buf, "%d=", tp->tp_dbindex));
	}
	if (tp == void_type) {
		adds_db_str(sprint(buf, "%d", tp->tp_dbindex));
		return;
	}
	switch(tp->tp_fund) {
	/* simple types ... */
	case T_INTEGER:
	case T_LONG: {
		arith l = full_mask[(int)tp->tp_size] & ~(1L << (tp->tp_size*8-1));
		adds_db_str(sprint(buf,
		       "r%d;%ld;%ld",
		       tp->tp_dbindex,
		       (long) -l-1,
		       (long) l));
		}
		break;
	case T_REAL:
		adds_db_str(sprint(buf,
		       "r%d;%ld;0",
		       tp->tp_dbindex,
		       (long)tp->tp_size));
		break;
	case T_CHAR:
		adds_db_str(sprint(buf,
		       "r%d;0;255",
		       tp->tp_dbindex));
		break;

	/* constructed types ... */
	case T_SUBRANGE:
		adds_db_str(sprint(buf,
		       "r%d;%ld;%ld",
		       tp->next->tp_dbindex,
		       (long) tp->sub_lb,
		       (long) tp->sub_ub));
		break;
	case T_POINTER:
		if (tp->next) {
			addc_db_str('*');
			stb_type(tp->next, 0);
			if (tp->tp_dbindex < 0) tp->tp_dbindex = -tp->tp_dbindex;
		}
		else {
			tp->tp_dbindex = - ++stb_count;
			adds_db_str(sprint(buf, "%d", -tp->tp_dbindex));
		}
		break;
	case T_SET:
		addc_db_str('S');
		stb_type(tp->next, 0);
		adds_db_str(sprint(buf, ";%ld;%ld;", (long) tp->tp_size, 0L));
		break;
	case T_ARRAY:
		addc_db_str('a');
		if (IsConformantArray(tp)) {
			addc_db_str('r');
			stb_type(tp->next, 0);
			adds_db_str(sprint(buf, ";A%ld;Z%ld", (long) tp->arr_cfdescr, (long) tp->arr_cfdescr));
		}
		else {
			stb_type(tp->next, 0);
		}
		addc_db_str(';');
		stb_type(tp->arr_elem, 0);
		break;
	case T_ENUMERATION:
		addc_db_str('e');
		{
			register struct def	*edef = tp->enm_enums;

			while (edef) {
				adds_db_str(sprint(buf, "%s:%ld,",
					edef->df_idf->id_text,
					(long) edef->enm_val));
				edef = edef->enm_next;
			}
		}
		addc_db_str(';');
		break;
	case T_RECORD:
		adds_db_str(sprint(buf, "s%ld", (long) tp->tp_size));
		{
			register struct def	*sdef = tp->rec_scope->sc_def;

			while (sdef) {
				adds_db_str(sdef->df_idf->id_text);
				addc_db_str(':');
				stb_type(sdef->df_type, 0);
				adds_db_str(sprint(buf,
					",%ld,%ld;",
					sdef->fld_off*8L,
					sdef->df_type->tp_size*8L));
				sdef = sdef->df_nextinscope;
			}
		}
		addc_db_str(';');
		break;
	case T_PROCEDURE:
	case T_FUNCTION:
		addc_db_str('Q');
		stb_type(tp->next ? tp->next : void_type, 0);
		{
			register struct paramlist *p = tp->prc_params;
			int paramcount = 0;

			while (p) {
				paramcount++;
				p = p->next;
			}
			adds_db_str(sprint(buf, ",%d;", paramcount));
			p = tp->prc_params;
			while (p) {
				addc_db_str(IsVarParam(p) 
					? 'v'
					: IsConformantArray(TypeOfParam(p)) 
						? 'i'
						: 'p');
				stb_type(TypeOfParam(p), 0);
				addc_db_str(';');
				p = p->next;
			}
		}
		break;
	case T_FILE:
		addc_db_str('L');
		stb_type(tp->next, 0);
		break;
	case T_STRING:
		addc_db_str('*');
		stb_type(char_type, 0);
		break;
	}
}

stb_string(df, kind)
	register struct def *df;
	long kind;
{
	register struct type	*tp = df->df_type;
	char buf[64];

	create_db_str();
	adds_db_str(df->df_idf->id_text);
	addc_db_str(':');
	if (kind == D_MODULE) {
		adds_db_str(sprint(buf, "M%d;", df->prc_vis->sc_count));
		C_ms_stb_pnam(db_str.base, N_FUN, proclevel, "_m_a_i_n");
		return;
	}
	switch((int)kind) {
	case D_PROCEDURE:
	case D_FUNCTION:
		adds_db_str(sprint(buf, "Q%d;", df->prc_vis->sc_count));
		stb_type(tp->next ? tp->next : void_type, 0);
		addc_db_str(';');
		C_ms_stb_pnam(db_str.base, N_FUN, proclevel, df->df_idf->id_text);
		{
			register struct paramlist *p = tp->prc_params;
			while (p) {
				stb_string(p->par_def, D_VARIABLE);
				p = p->next;
			}
		}
		for (df = df->prc_vis->sc_scope->sc_def; df; df = df->df_nextinscope) {
			if (df->df_kind == D_LBOUND ||
			    df->df_kind == D_UBOUND) {
				stb_string(df, df->df_kind);
			}
		}
		break;
	case D_END:
	case D_PEND:
		adds_db_str(sprint(buf, "E%d;", df->prc_vis->sc_count));
		C_ms_stb_cst(db_str.base, N_SCOPE, proclevel, (arith)0);
		break;
	case D_VARIABLE:
		if (df->df_flags & D_VARPAR) {	/* VAR parameter */
			addc_db_str('v');
			stb_type(tp, 0);
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_PSYM, 0, df->var_off);
		}
		else if (df->df_flags & D_VALPAR) {	/* value parameter */
			addc_db_str(IsConformantArray(tp)
				? 'i'
				: 'p');
			stb_type(tp, 0);
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_PSYM, 0, df->var_off);
		}
		else if (!proclevel) {
			addc_db_str('G');
			stb_type(tp, 0);
			addc_db_str(';');
			C_ms_stb_dnam(db_str.base, N_LCSYM, 0, df->var_name, (arith) 0);
		}
		else {	/* local variable */
			stb_type(tp, 1);	/* assign type num to avoid
						   difficult to parse string */
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_LSYM, 0, df->var_off);
		}
		break;
	case D_LBOUND:
	case D_UBOUND:
		addc_db_str(kind == D_LBOUND ? 'A' : 'Z');
		addc_db_str('p');
		stb_type(tp, 0);
		addc_db_str(';');
		C_ms_stb_cst(db_str.base, N_PSYM, 0, df->bnd_type->arr_cfdescr);
		break;
	case D_TYPE:
		addc_db_str('t');
		stb_type(tp, 1);
		addc_db_str(';');
		C_ms_stb_cst(db_str.base,
			     N_LSYM,
			     tp == void_type || tp->tp_size > 32767
			       ? 0
			       : (IsPacked(tp) ? (int) tp->tp_psize : (int)tp->tp_size),
			     (arith) 0);
		break;
	case D_CONST:
		addc_db_str('c');
		addc_db_str('=');
		tp = BaseType(tp);
		switch(tp->tp_fund) {
		case T_INTEGER:
		case T_LONG:
		case T_POINTER:
		case T_PROCEDURE:
			adds_db_str(sprint(buf, "i%ld;", (long) df->con_const->nd_INT));
			break;
		case T_CHAR:
			adds_db_str(sprint(buf, "c%ld;", (long) df->con_const->nd_INT));
			break;
		case T_REAL:
			addc_db_str('r');
			adds_db_str(df->con_const->nd_REL);
			addc_db_str(';');
			break;
		case T_STRINGCONST: {
			register char *p = df->con_const->nd_STR;

			adds_db_str("s'");
			while (*p) {
				if (*p == '\'' || *p == '\\') {
					addc_db_str('\\');
				}
				addc_db_str(*p++);
			}
			adds_db_str("';");
			}
			break;
		case T_ENUMERATION:
			addc_db_str('e');
			stb_type(tp, 0);
			adds_db_str(sprint(buf, ",%ld;", (long) df->con_const->nd_INT));
			break;
		case T_SET: {
			register int i;

			addc_db_str('S');
			stb_type(tp, 0);
			for (i = 0; i < tp->tp_size; i++) {
				adds_db_str(sprint(buf, ",%ld",
					(long) (df->con_const->nd_set[i/(int) word_size] >> (8*(i%(int)word_size)))&0377));
			}
			addc_db_str(';');
			}
			break;
		}
		C_ms_stb_cst(db_str.base,
			     N_LSYM,
			     tp->tp_size <= 32767 ? (int)tp->tp_size : 0,
			     (arith) 0);
		break;
	}
}

#endif /* DBSYMTAB */
