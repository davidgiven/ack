/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*	DUMP ROUTINES	*/

#include <stddef.h>
#include <stdbool.h>
#include "parameters.h"
#ifdef DEBUG
#include <ack_string.h>
#include <alloc.h>
#include <flt_arith.h>
#include "arith.h"
#include "stack.h"
#include "def.h"
#include "idf.h"
#include "type.h"
#include "proto.h"
#include "struct.h"
#include "field.h"
#include "Lpars.h"
#include "label.h"
#include "expr.h"
/*#include	"static.h"*/
#include "declar.h"

/*	Some routines (symbol2str, type2str, qual2str) which should have
 *	yielded strings are written to yield a pointer to a transient piece
 *	of memory, containing the string, since this is the only reasonable
 *	thing to do in C. `Transient' means that the result may soon
 *	disappear, which is generally not a problem, since normally it is
 *	consumed immediately. Sometimes we need more than one of them, and
 *	MAXTRANS is the maximum number we will need simultaneously.
 */
#define MAXTRANS 6

extern char options[];

extern struct idf* idf_hashtable[];

enum sdef_kind
{
	selector,
	field
}; /* parameter for dumpsdefs */

static int dumplevel;

/* Forward declarations */
static void dumpstack(void);
static char* next_transient(void);
static char* qual2str(int);
static char* type2str(struct type*);
static void p1_indent(int);
static void dumpdefs(struct def*, int);
void dumpidf(struct idf*, int);
void dumptags(struct tag*);
void dumptype(struct type*);
void dumpsdefs(struct sdef*, enum sdef_kind);
static void p1_expr(int, struct expr*);

void newline(void)
{
	int dl = dumplevel;

	printf("\n");
	while (dl >= 2)
	{
		printf("\t");
		dl -= 2;
	}
	if (dl)
		printf("    ");
}

void dumpidftab(char msg[], int opt)
{
	/*	Dumps the identifier table in readable form (but in
	    arbitrary order).
	    Unless opt & 1, macros are not dumped.
	    Unless opt & 2, reserved identifiers are not dumped.
	    Unless opt & 4, universal identifiers are not dumped.
	*/

	printf(">>> DUMPIDF, %s (start)", msg);
	dumpstack();
	idfappfun(dumpidf, opt);
	newline();
	printf(">>> DUMPIDF, %s (end)\n", msg);
}

static void dumpstack(void)
{
	/*	Dumps the identifier stack, starting at the top.
	 */
	struct stack_level* stl = local_level;

	while (stl)
	{
		struct stack_entry* se = stl->sl_entry;

		newline();
		printf("%3d: ", stl->sl_level);
		while (se)
		{
			printf("%s ", se->se_idf->id_text);
			se = se->next;
		}
		stl = stl->sl_previous;
	}
	printf("\n");
}

void dumpidf(struct idf* idf, int opt)
{
	/*	All information about the identifier idf is divulged in a
	    hopefully readable format.
	*/
	int started = 0;

	if (!idf)
		return;
	if ((opt & 2) && idf->id_reserved)
	{
		if (!started++)
		{
			newline();
			printf("%s:", idf->id_text);
		}
		printf(" reserved: %d;", idf->id_reserved);
	}
	if (idf->id_def && ((opt & 4) || idf->id_def->df_level))
	{
		if (!started++)
		{
			newline();
			printf("%s:", idf->id_text);
		}
		dumpdefs(idf->id_def, opt);
	}
	if (idf->id_sdef)
	{
		if (!started++)
		{
			newline();
			printf("%s:", idf->id_text);
		}
		dumpsdefs(idf->id_sdef, selector);
	}
	if (idf->id_tag)
	{
		if (!started++)
		{
			newline();
			printf("%s:", idf->id_text);
		}
		dumptags(idf->id_tag);
	}
}

void dumpdefs(struct def* def, int opt)
{
	dumplevel++;
	while (def && ((opt & 4) || def->df_level))
	{
		newline();
		printf(
		    "L%d: %s %s%stype%s %lo; ", def->df_level, symbol2str(def->df_sc),
		    def->df_initialized ? "init'd " : "", def->df_used ? "used " : "",
		    def->df_sc == ENUM ? ", =" : " at", def->df_address);
		printf("%s, line %u", def->df_file ? def->df_file : "NO_FILE", def->df_line);
		dumptype(def->df_type);
		def = def->next;
	}
	dumplevel--;
}

void dumptags(struct tag* tag)
{
	dumplevel++;
	while (tag)
	{
		struct type* tp = tag->tg_type;
		int fund = tp->tp_fund;

		newline();
		printf(
		    "L%d: %s %s", tag->tg_level,
		    fund == STRUCT      ? "struct"
		        : fund == UNION ? "union"
		        : fund == ENUM  ? "enum"
		                        : "<UNKNOWN>",
		    tp->tp_idf->id_text);
		if (is_struct_or_union(fund))
		{
			printf(" {");
			dumpsdefs(tp->tp_sdef, field);
			newline();
			printf("}");
		}
		printf(";");
		tag = tag->next;
	}
	dumplevel--;
}

void dumpsdefs(struct sdef* sdef, enum sdef_kind sdk)
{
	/*	Since sdef's are members of two chains, there are actually
	    two dumpsdefs's, one following the chain of all selectors
	    belonging to the same idf, starting at idf->id_sdef;
	    and the other following the chain of all selectors belonging
	    to the same struct, starting at stp->tp_sdef.
	*/

	dumplevel++;
	while (sdef)
	{
		newline();
		printf("L%d: ", sdef->sd_level);
#ifndef NOBITFIELD
		if (sdk == selector)
#endif /* NOBITFIELD */
			printf(
			    "selector %s at offset %lu in %s;", type2str(sdef->sd_type), sdef->sd_offset,
			    type2str(sdef->sd_stype));
#ifndef NOBITFIELD
		else
			printf("field %s at offset %lu;", type2str(sdef->sd_type), sdef->sd_offset);
#endif /* NOBITFIELD */
		sdef = (sdk == selector ? sdef->next : sdef->sd_sdef);
	}
	dumplevel--;
}

void dumpproto(struct proto* pl)
{
	struct type* type;
	int argcnt = 0;

	newline();
	printf("dump proto type list (start)");
	newline();
	while (pl)
	{
		printf(
		    "%d: %s", argcnt++,
		    pl->pl_flag & PL_FORMAL ? (pl->pl_flag & PL_VOID ? "void" : "formal")
		                            : (pl->pl_flag & PL_ELLIPSIS ? "ellipsis" : "unknown"));
		newline();
		if ((type = pl->pl_type))
		{
			dumptype(type);
			newline();
		}
		if (pl->pl_idf)
		{
			dumplevel++;
			printf("idf:");
			dumpidf(pl->pl_idf, 7);
			dumplevel--;
		}
		newline();
		pl = pl->next;
	}
	printf("dump proto type list (end)\n");
}

void dumptype(struct type* tp)
{
	int ops = 1;

	dumplevel++;
	newline();
	if (!tp)
	{
		printf("<NILTYPE>");
		newline();
		dumplevel--;
		return;
	}

	printf("(@%lx, #%ld, &%d) ", tp, (long)tp->tp_size, tp->tp_align);

	while (ops)
	{
		printf("%s", qual2str(tp->tp_typequal));
		switch (tp->tp_fund)
		{
			case POINTER:
				printf("pointer to ");
				break;
			case ARRAY:
				printf("array [%ld] of ", tp->tp_size);
				break;
			case FUNCTION:
				printf("function ");
				if (tp->tp_proto)
				{
					printf("with prototype");
					dumplevel++;
					dumpproto(tp->tp_proto);
					dumplevel--;
					newline();
				}
				printf("yielding ");
				break;
			default:
				printf("%s%s ", tp->tp_unsigned ? "unsigned " : "", symbol2str(tp->tp_fund));
				if (tp->tp_idf)
					printf("%s ", tp->tp_idf->id_text);
#ifndef NOBITFIELD
				if (tp->tp_fund == FIELD && tp->tp_field)
				{
					struct field* fd = tp->tp_field;

					printf("[s=%ld,w=%ld] of ", fd->fd_shift, fd->fd_width);
				}
				else
#endif /* NOBITFIELD */
					ops = 0;
				break;
		}
		if (ops)
			tp = tp->tp_up;
	}
	dumplevel--;
}

static char* type2str(struct type* tp)
{
	/*	Yields a pointer to a one-line description of the type tp.
	 */
	char* buf = next_transient();
	int ops = 1;

	buf[0] = '\0';
	if (!tp)
	{
		sprintf(buf, "<NILTYPE>");
		return buf;
	}
	sprintf(buf, "%s(@%lx, #%ld, &%d) ", buf, tp, (long)tp->tp_size, tp->tp_align);

	while (ops)
	{
		sprintf(buf, "%s%s", buf, qual2str(tp->tp_typequal));
		switch (tp->tp_fund)
		{
			case POINTER:
				sprintf(buf, "%spointer to ", buf);
				break;
			case ARRAY:
				sprintf(buf, "%sarray [%ld] of ", buf, tp->tp_size);
				break;
			case FUNCTION:
				sprintf(buf, "%sfunction yielding ", buf);
				break;
			default:
				sprintf(
				    buf, "%s%s%s ", buf, tp->tp_unsigned ? "unsigned " : "",
				    symbol2str(tp->tp_fund));
				if (tp->tp_idf)
					sprintf(buf, "%s %s ", buf, tp->tp_idf->id_text);
#ifndef NOBITFIELD
				if (tp->tp_fund == FIELD && tp->tp_field)
				{
					struct field* fd = tp->tp_field;

					sprintf(buf, "%s [s=%ld,w=%ld] of ", buf, fd->fd_shift, fd->fd_width);
				}
				else
#endif /* NOBITFIELD */
					ops = 0;
				break;
		}
		if (ops)
			tp = tp->tp_up;
	}
	return buf;
}

static char* qual2str(int qual)
{
	char* buf = next_transient();

	*buf = '\0';
	if (qual == 0)
		sprintf(buf, "(none)");
	if (qual & TQ_CONST)
		sprintf(buf, "%sconst ", buf);
	if (qual & TQ_VOLATILE)
		sprintf(buf, "%svolatile ", buf);

	return qual == 0 ? "" : buf;
}

GSTATIC char trans_buf[MAXTRANS][300];

static char* /* the ultimate transient buffer supplier */
next_transient(void)
{
	static int bnum;

	if (++bnum == MAXTRANS)
		bnum = 0;
	return trans_buf[bnum];
}

void print_expr(char msg[], struct expr* expr)
{
	/*	Provisional routine to print an expression preceded by a
	    message msg.
	*/
	if (options['x'])
	{
		printf("\n%s: ", msg);
		printf("(L=line, T=type, r/lV=r/lvalue, F=flags, D=depth)\n");
		p1_expr(0, expr);
	}
}

static void p1_expr(int lvl, struct expr* expr)
{
	p1_indent(lvl);
	if (!expr)
	{
		printf("NILEXPR\n");
		return;
	}
	printf(
	    "expr: L=%u, T=%s, %cV, F=%03o, D=%d, %s: ", expr->ex_line, type2str(expr->ex_type),
	    expr->ex_lvalue ? 'l' : 'r', expr->ex_flags & 0xFF, expr->ex_depth,
	    expr->ex_class == Value        ? "Value"
	        : expr->ex_class == String ? "String"
	        : expr->ex_class == Float  ? "Float"
	        : expr->ex_class == Oper   ? "Oper"
	        : expr->ex_class == Type   ? "Type"
	                                   : "UNKNOWN CLASS");
	switch (expr->ex_class)
	{
		struct oper* o;
		case Value:
			switch (expr->VL_CLASS)
			{
				case Const:
					printf("(Const) ");
					break;
				case Name:
					printf("(Name) %s + ", expr->VL_IDF->id_text);
					break;
				case Label:
					printf("(Label) .%lu + ", expr->VL_LBL);
					break;
				default:
					printf("(Unknown) ");
					break;
			}
			printf("%s\n", writh2str(expr->VL_VALUE, expr->ex_type->tp_unsigned));
			break;
		case String:
		{
			printf("\"%s\"\n", bts2str(expr->SG_VALUE, expr->SG_LEN - 1, next_transient()));
			break;
		}
		case Float:
		{
			char buf[FLT_STRLEN];

			flt_flt2str(&(expr->FL_ARITH), buf, FLT_STRLEN);
			printf("%s\n", buf);
			break;
		}
		case Oper:
			o = &expr->ex_object.ex_oper;
			printf("\n");
			p1_expr(lvl + 1, o->op_left);
			p1_indent(lvl);
			printf("%s <%s>\n", symbol2str(o->op_oper), type2str(o->op_type));
			p1_expr(lvl + 1, o->op_right);
			break;
		case Type:
			printf("\n");
			break;
		default:
			printf("UNKNOWN CLASS\n");
			break;
	}
}

static void p1_indent(int lvl)
{
	while (lvl--)
		printf("  ");
}
#endif /* DEBUG */
