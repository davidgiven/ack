/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	ADMINISTRATION OF STRUCT AND UNION DECLARATIONS	*/

#include	"nobitfield.h"
#include	"debug.h"
#include	"botch_free.h"
#include	<alloc.h>
#include	"arith.h"
#include	"stack.h"
#include	"idf.h"
#include	"def.h"
#include	"type.h"
#include	"struct.h"
#include	"field.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"align.h"
#include	"level.h"
#include	"assert.h"
#include	"sizes.h"
#include	"noRoption.h"

/*	Type of previous selector declared with a field width specified,
	if any.  If a selector is declared with no field with it is set to 0.
*/
static field_busy = 0;

extern char options[];
int lcm();

/*	The semantics of the identification of structure/union tags is
	obscure.  Some highly regarded compilers are found out to accept,
	e.g.:
		f(xp) struct aap *xp;	{
			struct aap {char *za;};
			xp->za;
		}
	Equally highly regarded software uses this feature, so we shall
	humbly oblige.
	The rules we use are:
	1.	A structure definition applies at the level where it is
		found, unless there is a structure declaration without a
		definition on an outer level, in which case the definition
		is applied at that level.
	2.	A selector is applied on the same level as on which its
		structure is being defined.

	If below struct is mentioned, union is implied (and sometimes enum
	as well).
*/

add_sel(stp, tp, idf, sdefpp, szp, fd)	/* this is horrible */
	register struct type *stp;	/* type of the structure */
	struct type *tp;		/* type of the selector */
	register struct idf *idf;	/* idf of the selector */
	struct sdef ***sdefpp;	/* address of hook to selector definition */
	arith *szp;		/* pointer to struct size upto here */
	struct field *fd;
{
	/*	The selector idf with type tp is added to two chains: the
		selector identification chain starting at idf->id_sdef,
		and to the end of the member list starting at stp->tp_sdef.
		The address of the hook in the latest member (sdef) is
		given in sdefpp; the hook itself must still be empty.
	*/
	arith offset;
#ifndef NOBITFIELD
	extern arith add_field();
#endif NOBITFIELD

	struct tag *tg = stp->tp_idf->id_struct;	/* or union */
	struct sdef *sdef = idf->id_sdef;
	register struct sdef *newsdef;
	int lvl = tg->tg_level;
	
#ifndef NOROPTION
	if (options['R'] && !is_anon_idf(idf))	{
		/* a K & R test */
		if (idf->id_struct && idf->id_struct->tg_level == level)
			warning("%s is also a struct/union tag", idf->id_text);
	}
#endif

	if (stp->tp_fund == STRUCT)	{
#ifndef NOBITFIELD
		if (fd == 0)	{	/* no field width specified	*/
			offset = align(*szp, tp->tp_align);
			field_busy = 0;
		}
		else	{
			/*	if something is wrong, the type of the
				specified selector remains unchanged; its
				bitfield specifier, however, is thrown away.
			*/
			offset = add_field(szp, fd, &tp, idf, stp);
		}
#else NOBITFIELD
		offset = align(*szp, tp->tp_align);
		field_busy = 0;
#endif NOBITFIELD
	}
	else	{	/* (stp->tp_fund == UNION)		*/
		if (fd)	{
			error("fields not allowed in unions");
			free_field(fd);
			fd = 0;
		}
		offset = (arith)0;
	}
	
	check_selector(idf, stp);
#ifndef NOROPTION
	if (options['R'])	{
		if (	sdef && sdef->sd_level == lvl &&
			( sdef->sd_offset != offset || sdef->sd_type != tp )
		)				/* RM 8.5 */
			warning("selector %s redeclared", idf->id_text);
	}
#endif

	newsdef = new_sdef();
	newsdef->sd_sdef = (struct sdef *) 0;

	/*	link into selector descriptor list of this id
	*/
	newsdef->next = sdef;
	idf->id_sdef = newsdef;

	newsdef->sd_level = lvl;
	newsdef->sd_idf = idf;
	newsdef->sd_stype = stp;
	newsdef->sd_type = tp;
	newsdef->sd_offset = offset;

#ifndef NOBITFIELD
	if (tp->tp_fund == FIELD)
		tp->tp_field->fd_sdef = newsdef;
#endif NOBITFIELD

	stack_idf(idf, stack_level_of(lvl));

	/*	link into selector definition list of the struct/union
	*/
	**sdefpp = newsdef;
	*sdefpp = &newsdef->sd_sdef;

	/* update the size of the struct/union upward	*/
	if (stp->tp_fund == STRUCT && fd == 0)	{
		/*	Note: the case that a bitfield is declared is
			handled by add_field() !
		*/
		*szp = offset + size_of_type(tp, "member");
		stp->tp_align = lcm(stp->tp_align, tp->tp_align);
	}
	else
	if (stp->tp_fund == UNION)	{
		arith sel_size = size_of_type(tp, "member");

		if (*szp < sel_size)
			*szp = sel_size;
		stp->tp_align = lcm(stp->tp_align, tp->tp_align);
	}
}

check_selector(idf, stp)
	register struct idf *idf;
	struct type *stp;	/* the type of the struct */
{
	/*	checks if idf occurs already as a selector in
		struct or union *stp.
	*/
	register struct sdef *sdef = stp->tp_sdef;
	
	while (sdef)	{
		if (sdef->sd_idf == idf)
			error("multiple selector %s", idf->id_text);
		sdef = sdef->sd_sdef;
	}
}

declare_struct(fund, idf, tpp)
	register struct idf *idf;
	struct type **tpp;
{
	/*	A struct, union or enum (depending on fund) with tag (!)
		idf is declared, and its type (incomplete as it may be) is
		returned in *tpp.
		The idf may be missing (i.e. idf == 0), in which case an
		anonymous struct etc. is defined.
	*/
	extern char *symbol2str();
	register struct tag **tgp;
	register struct tag *tg;

	if (!idf)
		idf = gen_idf();
	tgp = (fund == ENUM ? &idf->id_enum : &idf->id_struct);
	
#ifndef NOROPTION
	if (options['R'] && !is_anon_idf(idf))	{
		/* a K & R test */
		if (	fund != ENUM &&
			idf->id_sdef && idf->id_sdef->sd_level == level
		)	{
			warning("%s is also a selector", idf->id_text);
		}
		if (	fund == ENUM &&
			idf->id_def && idf->id_def->df_level == level
		)	{
			warning("%s is also a variable", idf->id_text);
		}
	}
#endif
	
	tg = *tgp;
	if (tg && tg->tg_type->tp_size < 0 && tg->tg_type->tp_fund == fund) {
		/*	An unfinished declaration has preceded it, possibly on
			an earlier level.  We just fill in the answer.
		*/
		if (tg->tg_busy) {
			error("recursive declaration of struct/union %s",
				idf->id_text);
			declare_struct(fund, gen_idf(), tpp);
		}
		else {
#ifndef NOROPTION
			if (options['R'] && tg->tg_level != level)
				warning("%s declares %s in different range",
					idf->id_text, symbol2str(fund));
#endif
			*tpp = tg->tg_type;
		}
	}
	else
	if (tg && tg->tg_level == level)	{
		/*	There is an already defined struct/union of this name
			on our level!
		*/
		error("redeclaration of struct/union %s", idf->id_text);
		declare_struct(fund, gen_idf(), tpp);
		/* to allow a second struct_declaration_pack */
	}
	else	{
		/* The struct is new. */
		/* Hook in a new struct tag */
		tg = new_tag();
		tg->next = *tgp;
		*tgp = tg;
		tg->tg_level = level;
		/* and supply room for a type */
		tg->tg_type = create_type(fund);
		tg->tg_type->tp_align =
			fund == ENUM ? int_align :
			fund == STRUCT ? struct_align :
			/* fund == UNION */ union_align;
		tg->tg_type->tp_idf = idf;
		*tpp = tg->tg_type;
		stack_idf(idf, local_level);
	}
}

apply_struct(fund, idf, tpp)
	register struct idf *idf;
	struct type **tpp;
{
	/*	The occurrence of a struct, union or enum (depending on
		fund) with tag idf is noted. It may or may not have been
		declared before. Its type (complete or incomplete) is
		returned in *tpp.
	*/
	register struct tag **tgp;

	tgp = (is_struct_or_union(fund) ? &idf->id_struct : &idf->id_enum);

	if (*tgp)
		*tpp = (*tgp)->tg_type;
	else
		declare_struct(fund, idf, tpp);
}

struct sdef *
idf2sdef(idf, tp)
	register struct idf *idf;
	struct type *tp;
{
	/*	The identifier idf is identified as a selector, preferably
		in the struct tp, but we will settle for any unique
		identification.
		If the attempt fails, a selector of type error_type is
		created.
	*/
	register struct sdef **sdefp = &idf->id_sdef, *sdef;
	
	/* Follow chain from idf, to meet tp. */
	while ((sdef = *sdefp))	{
		if (sdef->sd_stype == tp)
			return sdef;
		sdefp = &(*sdefp)->next;
	}
	
	/* Tp not met; any unique identification will do. */
	if (sdef = idf->id_sdef)	{
		/* There is an identification */
		if (uniq_selector(sdef))	{
			/* and it is unique, so we accept */
			warning("selector %s applied to alien type",
					idf->id_text);
		}
		else	{
			/* it is ambiguous */
			error("ambiguous use of selector %s", idf->id_text);
		}
		return sdef;
	}
	
	/* No luck; create an error entry. */
	if (!is_anon_idf(idf))
		error("unknown selector %s", idf->id_text);
	*sdefp = sdef = new_sdef();
	sdef->sd_idf = idf;
	sdef->sd_stype = sdef->sd_type = error_type;
	return sdef;
}

int
uniq_selector(idf_sdef)
	register struct sdef *idf_sdef;
{
	/*	Returns true if idf_sdef (which is guaranteed to exist)
		is unique for this level, i.e there is no other selector
		on this level with the same name or the other selectors
		with the same name have the same offset.
		See /usr/src/cmd/sed/sed.h for an example of this absurd
		case!
	*/
	
	register struct sdef *sdef = idf_sdef->next;
	
	while (sdef && sdef->sd_level == idf_sdef->sd_level)	{
		if (	sdef->sd_type != idf_sdef->sd_type
		||	sdef->sd_offset != idf_sdef->sd_offset
		)	{
			return 0;		/* ambiguity found */
		}
		sdef = sdef->next;
	}
	return 1;
}

#ifndef NOBITFIELD
arith
add_field(szp, fd, fdtpp, idf, stp)
	arith *szp;			/* size of struct upto here	*/
	register struct field *fd;	/* bitfield, containing width	*/
	register struct type **fdtpp;	/* type of selector		*/
	struct idf *idf;		/* name of selector		*/
	register struct type *stp;	/* current struct descriptor	*/
{
	/*	The address where this selector is put is returned. If the
		selector with specified width does not fit in the word, or
		an explicit alignment is given, a new address is needed.
		Note that the fields are packed into machine words (according
		to the RM.)
	*/
#ifdef word_size
#define bits_in_type	((int)(8*word_size))
#else
	int bits_in_type = word_size * 8;
#endif
	static int field_offset = (arith)0;
	static struct type *current_struct = 0;
	static int bits_declared;	/* nr of bits used in *field_offset */

	if (current_struct != stp)	{
		/*	This struct differs from the last one
		*/
		field_busy = 0;
		current_struct = stp;
	}

	if (	fd->fd_width < 0 ||
		(fd->fd_width == 0 && !is_anon_idf(idf)) ||
		fd->fd_width > bits_in_type
	)	{
		error("illegal field-width specified");
		*fdtpp = error_type;
		return field_offset;
	}

	switch ((*fdtpp)->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case ENUM:
	case LONG:
		/* right type; size OK? */
		if ((int) (*fdtpp)->tp_size > (int) word_size) {
			error("bit field type %s does not fit in a word",
				symbol2str((*fdtpp)->tp_fund));
			*fdtpp = error_type;
			return field_offset;
		}
		break;

	default:
		/* wrong type altogether */
		error("field type cannot be %s",
				symbol2str((*fdtpp)->tp_fund));
		*fdtpp = error_type;
		return field_offset;
	}

	if (field_busy == 0)	{
		/*	align this selector on the next boundary :
			the previous selector wasn't a bitfield.
		*/
		field_offset = align(*szp, int_align);
		*szp = field_offset + int_size;
		stp->tp_align = lcm(stp->tp_align, int_align);
		bits_declared = 0;
		field_busy = 1;
	}

	if (fd->fd_width > bits_in_type - bits_declared)	{
		/*	field overflow: fetch next memory unit
		*/
		field_offset = align(*szp, int_align);
		*szp = field_offset + int_size;
		stp->tp_align = lcm(stp->tp_align, int_align);
		bits_declared = fd->fd_width;
	}
	else
	if (fd->fd_width == 0)
		/*	next field should be aligned on the next boundary.
			This will take care that no field will fit in the
			space allocated upto here.
		*/
		bits_declared = bits_in_type + 1;
	else	/* the bitfield fits in the current field	*/
		bits_declared += fd->fd_width;
	
	/*	Arrived here, the place where the selector is stored in the
		struct is computed.
		Now we need a mask to use its value in expressions.
	*/
	*fdtpp = construct_type(FIELD, *fdtpp, (arith)0);
	(*fdtpp)->tp_field = fd;

	/*	Set the mask right shifted. This solution avoids the
		problem of having sign extension when using the mask for
		extracting the value from the field-int.
		Sign extension could occur on some machines when shifting
		the mask to the left.
	*/
	if (fd->fd_width >= 8*sizeof(arith)) fd->fd_mask = -1;
	else fd->fd_mask = (1L << fd->fd_width) - 1;

	if (options['r'])	/* adjust the field at the right	*/
		fd->fd_shift = bits_declared - fd->fd_width;
	else			/* adjust the field at the left		*/
		fd->fd_shift = bits_in_type - bits_declared;
	
	return field_offset;
}
#endif NOBITFIELD

/* some utilities */
int
is_struct_or_union(fund)
	register int fund;
{
	return fund == STRUCT || fund == UNION;
}

/*	Greatest Common Divisor
 */
int
gcd(m, n)
	register int m, n;
{
	register int r;

	while (n)	{
		r = m % n;
		m = n;
		n = r;
	}
	return m;
}

/*	Least Common Multiple
 */
int
lcm(m, n)
	register int m, n;
{
	return m * (n / gcd(m, n));
}
