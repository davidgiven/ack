/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*		L A B E L   H A N D L I N G		*/

#include	"Lpars.h"
#include	"level.h"
#include	"idf.h"
#include	"label.h"
#include	"arith.h"
#include	"def.h"
#include	"type.h"

extern char options[];

enter_label(idf, defining)
	register struct idf *idf;
{
	/*	The identifier idf is entered as a label. If it is new,
		it is entered into the idf list with the largest possible
		scope, i.e., on the lowest possible level.
		If defining, the label comes from a label statement.
	*/
	register struct def *def = idf->id_def;

	if (def)	{
		if (def->df_sc == LABEL)	{
			if (defining && def->df_initialized)
				error("redeclaration of label %s",
								idf->id_text);
		}
		else	{		/* there may still be room for it */
			if (def->df_level == level)	/* but alas, no */
				error("%s is not a label", idf->id_text);
			else	{
				add_def(idf, LABEL, label_type, L_LOCAL);
			}
		}
	}
	else	{
		add_def(idf, LABEL, label_type, L_LOCAL);
	}
	def = idf->id_def;	/* might be changed by 1 of the 2 add_defs */
	if (def->df_address == 0)
		def->df_address = (arith) text_label();
	if (defining)
		def->df_initialized = 1;
}

unstack_label(idf)
	register struct idf *idf;
{
	/*	The scope in which the label idf occurred is left.
	*/
	if (!idf->id_def->df_initialized && !is_anon_idf(idf))
		error("label %s not defined", idf->id_text);
}
