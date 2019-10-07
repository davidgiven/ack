/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* D E B U G G E R   S Y M B O L   T A B L E */

/* $Id$ */

#include	"parameters.h"

#ifdef	DBSYMTAB

#include	<alloc.h>
#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<flt_arith.h>
#include	<stb.h>

#include    "stab.h"
#include    "idf.h"
#include	"LLlex.h"
#include	"stack.h"
#include	"def.h"
#include	"type.h"
#include	"struct.h"
#include	"field.h"
#include	"Lpars.h"
#include	"level.h"
#include    "print.h"

#define INCR_SIZE	64

static struct db_str
{
	unsigned sz;
	char *base;
	char *currpos;
} db_str;

static void create_db_str(void)
{
	if (!db_str.base)
	{
		db_str.base = Malloc(INCR_SIZE);
		db_str.sz = INCR_SIZE;
	}
	db_str.currpos = db_str.base;
}

static void addc_db_str(int c)
{
	int df = db_str.currpos - db_str.base;
	if (df >= db_str.sz - 1)
	{
		db_str.sz += INCR_SIZE;
		db_str.base = Realloc(db_str.base, db_str.sz);
		db_str.currpos = db_str.base + df;
	}
	*db_str.currpos++ = c;
	*db_str.currpos = '\0';
}

static void adds_db_str(char *s)
{
	while (*s)
		addc_db_str(*s++);
}

static void stb_type(register struct type *tp)
{
	char buf[128], *range;
	static int stb_count;
	long l;
	int uns;

	if (tp->tp_dbindex > 0)
	{
		adds_db_str(sprint(buf, "%d", tp->tp_dbindex));
		return;
	}
	if (tp->tp_dbindex < 0 && tp->tp_size < 0)
	{
		adds_db_str(sprint(buf, "%d", -tp->tp_dbindex));
		return;
	}
	if (tp->tp_dbindex <= 0)
	{
		tp->tp_dbindex = ++stb_count;
	}
	adds_db_str(sprint(buf, "%d=", tp->tp_dbindex));
	switch (tp->tp_fund)
	{
	/* simple types ... */
	case VOID:
		adds_db_str(sprint(buf, "%d", void_type->tp_dbindex));
		break;
	case INT:
	case LONG:
	case LNGLNG:
	case CHAR:
	case SHORT:
		switch ((tp->tp_size << 3) + !tp->tp_unsigned)
		{
#define R(s) range = #s; break
		case 0010: R(0;255);
		case 0011: R(-128;127);
		case 0020: R(0;65535);
		case 0021: R(-32768;32767);
		default:   R(0;-1); /* acts as 0;4294967295 */
		case 0041: R(-2147483648;2147483647);
		/*	The stabs reader in gdb(1) needs an octal integer
			when its value doesn't fit in type long.
		*/
		case 0100: R(0;01777777777777777777777);
		case 0101: R(01000000000000000000000;0777777777777777777777);
#undef R
		}
		adds_db_str(sprint(buf, "r%d;%s", tp->tp_dbindex, range));
		break;
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		adds_db_str(
				sprint(buf, "r%d;%ld;0", tp->tp_dbindex, (long) tp->tp_size));
		break;

		/* constructed types ... */
	case POINTER:
		addc_db_str('*');
		stb_type(tp->tp_up);
		break;
	case ARRAY:
		if (tp->tp_size > 0)
		{
			adds_db_str("ar");
			stb_type(int_type);
			adds_db_str(
					sprint(buf, ";0;%ld;",
							tp->tp_size / tp->tp_up->tp_size - 1));
			stb_type(tp->tp_up);
		}
		break;
	case ENUM:
		if (tp->tp_size < 0)
		{
			adds_db_str(sprint(buf, "xe%s:", tp->tp_idf->id_text));
			tp->tp_dbindex = -tp->tp_dbindex;
			break;
		}
		addc_db_str('e');
		{
			register struct stack_entry *se = local_level->sl_entry;

			while (se)
			{
				register struct def *edef = se->se_idf->id_def;
				while (edef)
				{
					if (edef->df_type == tp && edef->df_sc == ENUM)
					{
						adds_db_str(
								sprint(buf, "%s:%ld,", se->se_idf->id_text,
										edef->df_address));
					}
					edef = edef->next;
				}
				se = se->next;
			}
		}
		addc_db_str(';');
		break;
	case STRUCT:
	case UNION:
		if (tp->tp_size < 0)
		{
			adds_db_str(
					sprint(buf, "x%c%s:", tp->tp_fund == STRUCT ? 's' : 'u',
							tp->tp_idf->id_text));
			tp->tp_dbindex = -tp->tp_dbindex;
			break;
		}
		adds_db_str(
				sprint(buf, "%c%ld", tp->tp_fund == STRUCT ? 's' : 'u',
						tp->tp_size));
		{
			register struct sdef *sdef = tp->tp_sdef;

			while (sdef)
			{
				adds_db_str(sdef->sd_idf->id_text);
				addc_db_str(':');
				if (sdef->sd_type->tp_fund == FIELD)
				{
					stb_type(sdef->sd_type->tp_up);
					adds_db_str(
							sprint(buf, ",%ld,%ld;",
									sdef->sd_offset * 8
											+ sdef->sd_type->tp_field->fd_shift,
									sdef->sd_type->tp_field->fd_width));
				}
				else
				{
					stb_type(sdef->sd_type);
					adds_db_str(
							sprint(buf, ",%ld,%ld;", sdef->sd_offset * 8,
									sdef->sd_type->tp_size * 8));
				}
				sdef = sdef->sd_sdef;
			}
		}
		addc_db_str(';');
		break;
	case FUNCTION:
		addc_db_str('f');
		stb_type(tp->tp_up);
	}
}

void stb_tag(register struct tag *tg, char *str)
{
	create_db_str();
	adds_db_str(str);
	adds_db_str(":T");
	stb_type(tg->tg_type);
	addc_db_str(';');
	C_ms_stb_cst(db_str.base,
	N_LSYM,
			tg->tg_type == void_type || tg->tg_type->tp_size >= 32767 ?
					0 : (int) tg->tg_type->tp_size, (arith) 0);
}

void stb_typedef(register struct type *tp, char *str)
{
	create_db_str();
	adds_db_str(str);
	adds_db_str(":t");
	stb_type(tp);
	addc_db_str(';');
	C_ms_stb_cst(db_str.base,
	N_LSYM, tp == void_type || tp->tp_size >= 32767 ? 0 : (int) tp->tp_size,
			(arith) 0);
}

void stb_string(register struct def *df, int kind, char* str)
{
	register struct type *tp = df->df_type;

	create_db_str();
	adds_db_str(str);
	addc_db_str(':');
	switch (kind)
	{
	case FUNCTION:
		addc_db_str(df->df_sc == STATIC ? 'f' : 'F');
		stb_type(tp->tp_up);
		addc_db_str(';');
		C_ms_stb_pnam(db_str.base, N_FUN, 1 /* proclevel */, str);
		break;
	default:
		if (df->df_sc == FORMAL
				|| (df->df_sc == REGISTER && df->df_address >= 0))
		{
			/* value parameter */
			addc_db_str('p');
			stb_type(tp);
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_PSYM, 0, df->df_address);
		}
		else if (df->df_sc != AUTO && df->df_sc != REGISTER)
		{
			/* global */
			int stabtp = df->df_initialized ? N_STSYM : N_LCSYM;
			if (df->df_sc == STATIC)
			{
				if (df->df_level >= L_LOCAL)
				{
					addc_db_str('V');
				}
				else
				{
					addc_db_str('S');
				}
			}
			else
			{
				addc_db_str('G');
			}
			stb_type(tp);
			addc_db_str(';');
			if (df->df_sc == STATIC && df->df_level >= L_LOCAL)
			{
				C_ms_stb_dlb(db_str.base, stabtp, 0, (label) df->df_address,
						(arith) 0);
			}
			else
			{
				C_ms_stb_dnam(db_str.base, stabtp, 0, str, (arith) 0);
			}
		}
		else
		{ /* local variable */
			stb_type(tp); /* assign type num to avoid
			 difficult to parse string */
			addc_db_str(';');
			C_ms_stb_cst(db_str.base, N_LSYM, 0, df->df_address);
		}
		break;
	}
}

#endif /* DBSYMTAB */
