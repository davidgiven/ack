/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	DUMP ROUTINES	*/

#include	"debug.h"

#ifdef	DEBUG
#include	"nofloat.h"
#include	"nopp.h"
#include	"nobitfield.h"
#include	"arith.h"
#include	"stack.h"
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"struct.h"
#include	"field.h"
#include	"Lpars.h"
#include	"label.h"
#include	"expr.h"
#include	"static.h"

/*	Some routines (symbol2str, token2str, type2str) which should have
 *	yielded strings are written to yield a pointer to a transient piece
 *	of memory, containing the string, since this is the only reasonable
 *	thing to do in C. `Transient' means that the result may soon
 *	disappear, which is generally not a problem, since normally it is
 *	consumed immediately. Sometimes we need more than one of them, and
 *	MAXTRANS is the maximum number we will need simultaneously.
 */
#define	MAXTRANS	6

extern char options[];

extern struct idf *idf_hashtable[];
extern char *symbol2str(), *type2str(), *next_transient();

enum sdef_kind {selector, field};		/* parameter for dumpsdefs */

static int dumplevel;

static
newline()	{
	register int dl = dumplevel;
	
	print("\n");
	while (dl >= 2)	{
		print("\t");
		dl -= 2;
	}
	if (dl)
		print("    ");
}

dumpidftab(msg, opt)
	char msg[];
{
	/*	Dumps the identifier table in readable form (but in
		arbitrary order).
		Unless opt & 1, macros are not dumped.
		Unless opt & 2, reserved identifiers are not dumped.
		Unless opt & 4, universal identifiers are not dumped.
	*/
	int i;

	print(">>> DUMPIDF, %s (start)", msg);
	dumpstack();
	for (i = 0; i < HASHSIZE; i++)	{
		register struct idf *notch = idf_hashtable[i];

		while (notch)	{
			dumpidf(notch, opt);
			notch = notch->next;
		}
	}
	newline();
	print(">>> DUMPIDF, %s (end)\n", msg);
}

dumpstack()
{
	/*	Dumps the identifier stack, starting at the top.
	*/
	register struct stack_level *stl = local_level;
	
	while (stl)	{
		register struct stack_entry *se = stl->sl_entry;
		
		newline();
		print("%3d: ", stl->sl_level);
		while (se)	{
			print("%s ", se->se_idf->id_text);
			se = se->next;
		}
		stl = stl->sl_previous;
	}
	print("\n");
}

dumpidf(idf, opt)
	register struct idf *idf;
{
	/*	All information about the identifier idf is divulged in a
		hopefully readable format.
	*/
	int started = 0;
	
	if (!idf)
		return;
#ifndef NOPP
	if ((opt&1) && idf->id_macro)	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		print(" macro");
	}
#endif /* NOPP */
	if ((opt&2) && idf->id_reserved)	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		print(" reserved: %d;", idf->id_reserved);
	}
	if (idf->id_def && ((opt&4) || idf->id_def->df_level))	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		dumpdefs(idf->id_def, opt);
	}
	if (idf->id_sdef)	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		dumpsdefs(idf->id_sdef, selector);
	}
	if (idf->id_struct)	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		dumptags(idf->id_struct);
	}
	if (idf->id_enum)	{
		if (!started++)	{
			newline();
			print("%s:", idf->id_text);
		}
		dumptags(idf->id_enum);
	}
}

dumpdefs(def, opt)
	register struct def *def;
{
	dumplevel++;
	while (def && ((opt&4) || def->df_level))	{
		newline();
		print("L%d: %s %s%s%s%s %lo;",
			def->df_level,
			symbol2str(def->df_sc),
			def->df_initialized ? "init'd " : "",
			def->df_used ? "used " : "",
			type2str(def->df_type),
			def->df_sc == ENUM ? ", =" : " at",
			def->df_address
		);
		print("%s, line %u",
			def->df_file ? def->df_file : "NO_FILE", def->df_line);
		def = def->next;
	}
	dumplevel--;
}

dumptags(tag)
	register struct tag *tag;
{
	dumplevel++;
	while (tag)	{
		register struct type *tp = tag->tg_type;
		register int fund = tp->tp_fund;

		newline();
		print("L%d: %s %s",
			tag->tg_level,
			fund == STRUCT ? "struct" :
			fund == UNION ? "union" :
			fund == ENUM ? "enum" : "<UNKNOWN>",
			tp->tp_idf->id_text
		);
		if (is_struct_or_union(fund))	{
			print(" {");
			dumpsdefs(tp->tp_sdef, field);
			newline();
			print("}");
		}
		print(";");
		tag = tag->next;
	}
	dumplevel--;
}

dumpsdefs(sdef, sdk)
	register struct sdef *sdef;
	enum sdef_kind sdk;
{
	/*	Since sdef's are members of two chains, there are actually
		two dumpsdefs's, one following the chain of all selectors
		belonging to the same idf, starting at idf->id_sdef;
		and the other following the chain of all selectors belonging
		to the same struct, starting at stp->tp_sdef.
	*/

	dumplevel++;
	while (sdef)	{
		newline();
		print("L%d: ", sdef->sd_level);
#ifndef NOBITFIELD
		if (sdk == selector)
#endif /* NOBITFIELD */
			print("selector %s at offset %lu in %s;",
				type2str(sdef->sd_type),
				sdef->sd_offset, type2str(sdef->sd_stype)
			);
#ifndef NOBITFIELD
		else	print("field %s at offset %lu;",
				type2str(sdef->sd_type), sdef->sd_offset
			);
#endif /* NOBITFIELD */
		sdef = (sdk == selector ? sdef->next : sdef->sd_sdef);
	}
	dumplevel--;
}

char *
type2str(tp)
	register struct type *tp;
{
	/*	Yields a pointer to a one-line description of the type tp.
	*/
	char *buf = next_transient();
	int ops = 1;

	buf[0] = '\0';
	if (!tp)	{
		sprint(buf, "<NILTYPE>");
		return buf;
	}

	sprint(buf, "%s(#%ld, &%d) ", buf, (long)tp->tp_size, tp->tp_align);
	while (ops)	{
		switch (tp->tp_fund)	{
		case POINTER:
			sprint(buf, "%spointer to ", buf);
			break;
		case ARRAY:
			sprint(buf, "%sarray [%ld] of ", buf, tp->tp_size);
			break;
		case FUNCTION:
			sprint(buf, "%sfunction yielding ", buf);
			break;
		default:
			sprint(buf, "%s%s%s", buf,
					tp->tp_unsigned ? "unsigned " : "",
					symbol2str(tp->tp_fund)
			);
			if (tp->tp_idf)
				sprint(buf, "%s %s", buf,
					tp->tp_idf->id_text);
#ifndef NOBITFIELD
			if (tp->tp_field)	{
				struct field *fd = tp->tp_field;
				
				sprint(buf, "%s [s=%ld,w=%ld] of ", buf,
					fd->fd_shift, fd->fd_width);
			}
			else
#endif /* NOBITFIELD */
			ops = 0;
			break;
		}
		tp = tp->tp_up;
	}
	return buf;
}

GSTATIC char trans_buf[MAXTRANS][300];

char *		/* the ultimate transient buffer supplier */
next_transient()
{
	static int bnum;

	if (++bnum == MAXTRANS)
		bnum = 0;
	return trans_buf[bnum];
}

print_expr(msg, expr)
	char msg[];
	struct expr *expr;
{
	/*	Provisional routine to print an expression preceded by a
		message msg.
	*/
	if (options['x'])	{
		print("\n%s: ", msg);
		print("(L=line, T=type, r/lV=r/lvalue, F=flags, D=depth)\n");
		p1_expr(0, expr);
	}
}

p1_expr(lvl, expr)
	register struct expr *expr;
{
	p1_indent(lvl);
	if (!expr)	{
		print("NILEXPR\n");
		return;
	}
	print("expr: L=%u, T=%s, %cV, F=%03o, D=%d, %s: ",
		expr->ex_line,
		type2str(expr->ex_type),
		expr->ex_lvalue ? 'l' : 'r',
		expr->ex_flags & 0xFF,
		expr->ex_depth,
		expr->ex_class == Value ? "Value" :
		expr->ex_class == String ? "String" :
#ifndef NOFLOAT
		expr->ex_class == Float ? "Float" :
#endif /* NOFLOAT */
		expr->ex_class == Oper ? "Oper" :
		expr->ex_class == Type ? "Type" : "UNKNOWN CLASS"
	);
	switch (expr->ex_class)	{
		struct oper *o;
	case Value:
		switch (expr->VL_CLASS) {
		case Const:
			print("(Const) ");
			break;
		case Name:
			print("(Name) %s + ", expr->VL_IDF->id_text);
			break;
		case Label:
			print("(Label) .%lu + ", expr->VL_LBL);
			break;
		default:
			print("(Unknown) ");
			break;
		}
		print(expr->ex_type->tp_unsigned ? "%lu\n" : "%ld\n",
			expr->VL_VALUE);
		break;
	case String:
	{
		char *bts2str();

		print(
			"\"%s\"\n",
			bts2str(expr->SG_VALUE, expr->SG_LEN-1,
							next_transient())
		);
		break;
	}
#ifndef NOFLOAT
	case Float:
		print("%s\n", expr->FL_VALUE);
		break;
#endif /* NOFLOAT */
	case Oper:
		o = &expr->ex_object.ex_oper;
		print("\n");
		p1_expr(lvl+1, o->op_left);
		p1_indent(lvl);
		print("%s <%s>\n", symbol2str(o->op_oper),
			type2str(o->op_type)
		);
		p1_expr(lvl+1, o->op_right);
		break;
	case Type:
		print("\n");
		break;
	default:
		print("UNKNOWN CLASS\n");
		break;
	}
}

p1_indent(lvl)
	register int lvl;
{
	while (lvl--)
		print("  ");
}
#endif	/* DEBUG */
