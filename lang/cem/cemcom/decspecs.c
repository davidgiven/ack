/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	D E C L A R A T I O N   S P E C I F I E R   C H E C K I N G	*/

#include	"nofloat.h"
#include	"assert.h"
#include	"Lpars.h"
#include	"decspecs.h"
#include	"arith.h"
#include	"type.h"
#include	"level.h"
#include	"def.h"
#include	"noRoption.h"

extern char options[];
extern int level;
extern char *symbol2str();

struct decspecs null_decspecs;

do_decspecs(ds)
	register struct decspecs *ds;
{
	/*	The provisional decspecs ds as obtained from the program
		is turned into a legal consistent decspecs.
	*/
	register struct type *tp = ds->ds_type;
	
	ASSERT(level != L_FORMAL1);
	
	if (	level == L_GLOBAL &&
		(ds->ds_sc == AUTO || ds->ds_sc == REGISTER)
	)	{
		warning("no global %s variable allowed",
			symbol2str(ds->ds_sc));
		ds->ds_sc = GLOBAL;
	}

	if (level == L_FORMAL2)	{
		if (ds->ds_sc_given &&
		    ds->ds_sc != REGISTER){
			error("%s formal illegal", symbol2str(ds->ds_sc));
			ds->ds_sc = FORMAL;
		}
	}
	/*	The tests concerning types require a full knowledge of the
		type and will have to be postponed to declare_idf.
	*/

	/* some adjustments as described in RM 8.2 */
	if (tp == 0 && ds->ds_size == 0 && ds->ds_unsigned == 0) {
		ds->ds_notypegiven = 1;
	}
	if (tp == 0) {
		tp = int_type;
	}
	switch (ds->ds_size)	{
	case SHORT:
		if (tp == int_type)
			tp = short_type;
		else
			error("short with illegal type");
		break;
	case LONG:
		if (tp == int_type)
			tp = long_type;
		else
#ifndef NOFLOAT
		if (tp == float_type)
			tp = double_type;
		else
#endif /* NOFLOAT */
			error("long with illegal type");
		break;
	}
	if (ds->ds_unsigned)	{
		switch (tp->tp_fund)	{
		case CHAR:
#ifndef NOROPTION
			if (options['R'])
				warning("unsigned char not allowed");
#endif
			tp = uchar_type;
			break;
		case SHORT:
#ifndef NOROPTION
			if (options['R'])
				warning("unsigned short not allowed");
#endif
			tp = ushort_type;
			break;
		case INT:
			tp = uint_type;
			break;
		case LONG:
#ifndef NOROPTION
			if (options['R'])
				warning("unsigned long not allowed");
#endif
			tp = ulong_type;
			break;
		default:
			error("unsigned with illegal type");
			break;
		}
	}
	ds->ds_type = tp;
}
