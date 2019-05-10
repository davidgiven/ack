/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E B U G G E R   S Y M B O L   T A B L E */

/* $Id$ */

#include "parameters.h"

#ifdef DBSYMTAB

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<flt_arith.h>
#include	<stb.h>

#include	"LLlex.h"
#include	"def.h"
#include	"type.h"
#include	"idf.h"
#include	"scope.h"
#include	"error.h"
#include	"stab.h"
#include	"main.h"

extern int	gdb_flag;

#define INCR_SIZE	64

extern int	proclevel;
extern char	*sprint();

static struct db_str {
	unsigned	sz;
	char		*base;
	char		*currpos;
} db_str;

static void create_db_str(void)
{
	if (! db_str.base) {
		db_str.base = Malloc(INCR_SIZE);
		db_str.sz = INCR_SIZE;
	}
	db_str.currpos = db_str.base;
}

static void addc_db_str(int c)
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

static void adds_db_str(char *s)
{
	while (*s) addc_db_str(*s++);
}

static void stb_type(register struct type *tp, int assign_num)
{
	char buf[128];
	static int	stb_count;

	if (tp->tp_dbindex > 0) {
		adds_db_str(sprint(buf, "%d", tp->tp_dbindex));
		return;
	}
	if (tp->tp_dbindex < 0) {
		if (tp->tp_next == 0) {
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
		adds_db_str(sprint(buf,
		       "r%d;%ld;%ld",
		       tp->tp_dbindex,
		       (long) min_int[(int)tp->tp_size],
		       (long) max_int[(int)tp->tp_size]));
		break;
	case T_CARDINAL:
		adds_db_str(sprint(buf,
		       "r%d;0;-1",
		       tp->tp_dbindex));
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
	case T_WORD:
		if (tp->tp_size == word_size) {
			adds_db_str(sprint(buf,
		       		"r%d;0;-1",
		       		tp->tp_dbindex));
		}
		else {
			adds_db_str(sprint(buf,
		       		"r%d;0;255",
		       		tp->tp_dbindex));
		}
		break;

	/* constructed types ... */
	case T_SUBRANGE:
		adds_db_str(sprint(buf,
		       "r%d;%ld;%ld",
		       tp->tp_next->tp_dbindex,
		       (long) tp->sub_lb,
		       (long) tp->sub_ub));
		break;
	case T_EQUAL:
		stb_type(tp->tp_next, 0);
		if (tp->tp_dbindex < 0) tp->tp_dbindex = -tp->tp_dbindex;
		break;
	case T_HIDDEN:
		if (DefinitionModule && CurrVis == Defined->mod_vis) {
			tp->tp_dbindex = - ++stb_count;
			adds_db_str(sprint(buf, "%d", -tp->tp_dbindex));
		}
		else {
			/* ??? what to do here??? */
			addc_db_str('*');
			stb_type(void_type, 0);
			/* ??? this certainly is not correct */
		}
		break;
	case T_POINTER:
		if (tp->tp_next) {
			addc_db_str('*');
			stb_type(tp->tp_next, 0);
			if (tp->tp_dbindex < 0) tp->tp_dbindex = -tp->tp_dbindex;
		}
		else {
			tp->tp_dbindex = - ++stb_count;
			adds_db_str(sprint(buf, "%d", -tp->tp_dbindex));
		}
		break;
	case T_SET:
		addc_db_str('S');
		stb_type(tp->tp_next, 0);
		adds_db_str(sprint(buf, ";%ld;%ld;", tp->tp_size, tp->set_low));
		break;
	case T_ARRAY:
		addc_db_str('a');
		if (IsConformantArray(tp)) {
			addc_db_str('r');
			stb_type(tp->tp_next, 0);
			adds_db_str(sprint(buf, ";0;A%ld", tp->arr_high));
		}
		else {
			stb_type(tp->tp_next, 0);
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
					edef->enm_val));
				edef = edef->enm_next;
			}
		}
		addc_db_str(';');
		break;
	case T_RECORD:
		adds_db_str(sprint(buf, "s%ld", tp->tp_size));
		{
			register struct def	*sdef = tp->rec_scope->sc_def;

			while (sdef) {
				adds_db_str(sdef->df_idf->id_text);
				addc_db_str(':');
				stb_type(sdef->df_type, 0);
				adds_db_str(sprint(buf,
					",%ld,%ld;",
					sdef->fld_off*8,
					sdef->df_type->tp_size*8));
				sdef = sdef->df_nextinscope;
			}
		}
		addc_db_str(';');
		break;
	case T_PROCEDURE:
		if (gdb_flag) {
			addc_db_str('f');
			stb_type(tp->tp_next ? tp->tp_next : void_type, 0);
			break;
		}
		addc_db_str('Q');
		stb_type(tp->tp_next ? tp->tp_next : void_type, 0);
		{
			register struct paramlist *p = tp->prc_params;
			int paramcount = 0;

			while (p) {
				paramcount++;
				p = p->par_next;
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
				p = p->par_next;
			}
		}
	}
}

void stb_addtp(char *s, struct type *tp)
{
	create_db_str();
	adds_db_str(s);
	addc_db_str(':');
	addc_db_str('t');
	stb_type(tp, 1);
	addc_db_str(';');
	C_ms_stb_cst(db_str.base,
		     N_LSYM,
		     tp == void_type || tp->tp_size >= max_int[2]
		       ? 0
		       : (int)tp->tp_size,
		     (arith) 0);
}

void stb_string(register struct def *df, int kind)
{
	register struct type *tp = df->df_type;
	char buf[64];

	create_db_str();
	adds_db_str(df->df_idf->id_text);
	addc_db_str(':');
	switch(kind) {
	case D_MODULE:
		if (gdb_flag) {
			addc_db_str('F');
			stb_type(void_type, 0);
		}
		else {
			adds_db_str(sprint(buf, "M%d;", df->mod_vis->sc_count));
		}
		C_ms_stb_pnam(db_str.base, N_FUN, gdb_flag ? 0 : proclevel, df->mod_vis->sc_scope->sc_name);
		break;
	case D_PROCEDURE:
		if (gdb_flag) {
			addc_db_str('f');
		}
		else	adds_db_str(sprint(buf, "Q%d;", df->prc_vis->sc_count));
		stb_type(tp->tp_next ? tp->tp_next : void_type, 0);
		if (gdb_flag) {
			struct scopelist *sc = df->prc_vis;
			sc = enclosing(sc);
			while (sc) {
				struct def *d = sc->sc_scope->sc_definedby;

				if (d && d->df_kind == D_PROCEDURE) {
					adds_db_str(sprint(buf, ",%s", d->df_idf->id_text));
					break;
				}
				sc = enclosing(sc);
			}
		}
		else addc_db_str(';');
		C_ms_stb_pnam(db_str.base, N_FUN, gdb_flag ? 0 : proclevel, df->prc_vis->sc_scope->sc_name);
		break;
	case D_END:
		if (gdb_flag) break;
		adds_db_str(sprint(buf, "E%d;", df->mod_vis->sc_count));
		C_ms_stb_cst(db_str.base, N_SCOPE, proclevel, (arith) 0);
		break;
	case D_PEND:
		if (gdb_flag) break;
		adds_db_str(sprint(buf, "E%d;", df->prc_vis->sc_count));
		C_ms_stb_cst(db_str.base, N_SCOPE, proclevel, (arith) 0);
		break;
	case D_VARIABLE:
		if (DefinitionModule && CurrVis != Defined->mod_vis) break;
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
		else if (!proclevel ||
			 (df->df_flags & D_ADDRGIVEN)) {	/* global */
			int knd = N_LCSYM;
			if (df->df_flags & D_EXPORTED) {
				knd = N_GSYM;
				addc_db_str('G');
			}
			else {
				addc_db_str('S');
			}
			stb_type(tp, 0);
			addc_db_str(';');
			if (df->df_flags & D_ADDRGIVEN) {
				C_ms_stb_cst(db_str.base, knd, 0, df->var_off);
			}
			else {
				C_ms_stb_dnam(db_str.base, knd, 0, df->var_name, (arith) 0);
			}
		}
		else {	/* local variable */
			stb_type(tp, 1);	/* assign type num to avoid
						   difficult to parse string */
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_LSYM, 0, df->var_off);
		}
		break;
	case D_TYPE:
		addc_db_str('t');
		stb_type(tp, 1);
		addc_db_str(';');
		C_ms_stb_cst(db_str.base,
			     N_LSYM,
			     tp == void_type || tp->tp_size >= max_int[2]
			       ? 0
			       : (int)tp->tp_size,
			     (arith) 0);
		break;
	case D_CONST:
		if (DefinitionModule && CurrVis != Defined->mod_vis) break;
		addc_db_str('c');
		addc_db_str('=');
		tp = BaseType(tp);
		switch(tp->tp_fund) {
		case T_INTEGER:
		case T_INTORCARD:
		case T_CARDINAL:
		case T_WORD:
		case T_POINTER:
		case T_PROCEDURE:
			adds_db_str(sprint(buf, "i%ld;", df->con_const.TOK_INT));
			break;
		case T_CHAR:
			adds_db_str(sprint(buf, "c%ld;", df->con_const.TOK_INT));
			break;
		case T_REAL:
			addc_db_str('r');
			if (! df->con_const.TOK_RSTR) {
				char buf2[FLT_STRLEN];

				flt_flt2str(&df->con_const.TOK_RVAL, buf2, FLT_STRLEN);
				adds_db_str(buf2);
			}
			else adds_db_str(df->con_const.TOK_RSTR);
			addc_db_str(';');
			break;
		case T_STRING: {
			register char *p = df->con_const.TOK_STR;

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
			adds_db_str(sprint(buf, ",%ld;", df->con_const.TOK_INT));
			break;
		case T_SET: {
			register int i;

			addc_db_str('S');
			stb_type(tp, 0);
			for (i = 0; i < tp->tp_size; i++) {
				adds_db_str(sprint(buf, ",%ld",
					(df->con_const.tk_data.tk_set[i/(int) word_size] >> (8*(i%(int)word_size)))&0377));
			}
			addc_db_str(';');
			}
			break;
		}
		C_ms_stb_cst(db_str.base,
			     N_LSYM,
			     tp->tp_size < max_int[2] ? (int)tp->tp_size : 0,
			     (arith) 0);
		break;
	}
}

#endif /* DBSYMTAB */
