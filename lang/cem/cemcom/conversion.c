/* $Header$ */
/*	C O N V E R S I O N - C O D E  G E N E R A T O R	*/

#include	"nofloat.h"
#include	<em.h>
#include	"arith.h"
#include	"type.h"
#include	"sizes.h"
#include	"Lpars.h"

#define	T_SIGNED		1
#define	T_UNSIGNED		2
#ifndef NOFLOAT
#define	T_FLOATING		3
#endif NOFLOAT

/*	conversion() generates the EM code for a conversion between
	the types char, short, int, long, float, double and pointer.
	In case of integral type, the notion signed / unsigned is
	taken into account.
	The EM code to obtain this conversion looks like:
		LOC sizeof(from_type)
		LOC sizeof(to_type)
		C??
*/

conversion(from_type, to_type)
	register struct type *from_type, *to_type;
{
	register arith from_size = from_type->tp_size;
	register arith to_size = to_type->tp_size;

	if (from_type == to_type)	/* a little optimisation */
		return;
	switch (fundamental(from_type))	{
	case T_SIGNED:
		switch (fundamental(to_type))	{
		case T_SIGNED:
			C_loc(from_size);
			C_loc(to_size < word_size ? word_size : to_size);
			C_cii();
			break;
		case T_UNSIGNED:
			C_loc(from_size < word_size ? word_size : from_size);
			C_loc(to_size < word_size ? word_size : to_size);
			C_ciu();
			break;
#ifndef NOFLOAT
		case T_FLOATING:
			C_loc(from_size < word_size ? word_size : from_size);
			C_loc(to_size);
			C_cif();
			break;
#endif NOFLOAT
		}
		break;
	case T_UNSIGNED:
		C_loc(from_size < word_size ? word_size : from_size);
		C_loc(to_size < word_size ? word_size : to_size);
		switch (fundamental(to_type))	{
		case T_SIGNED:
			C_cui();
			break;
		case T_UNSIGNED:
			C_cuu();
			break;
#ifndef NOFLOAT
		case T_FLOATING:
			C_cuf();
			break;
#endif NOFLOAT
		}
		break;
#ifndef NOFLOAT
	case T_FLOATING:
		C_loc(from_size);
		C_loc(to_size < word_size ? word_size : to_size);
		switch (fundamental(to_type))	{
		case T_SIGNED:
			C_cfi();
			break;
		case T_UNSIGNED:
			C_cfu();
			break;
		case T_FLOATING:
			C_cff();
			break;
		}
		break;
#endif NOFLOAT
	default:
		crash("(conversion) illegal type conversion");
	}
}

/*	fundamental() returns in which category a given type falls:
	signed, unsigned or floating
*/
int
fundamental(tp)
	register struct type *tp;
{
	switch (tp->tp_fund)	{
	case CHAR:
	case SHORT:
	case INT:
	case ERRONEOUS:
	case LONG:
	case ENUM:
		return tp->tp_unsigned ? T_UNSIGNED : T_SIGNED;
#ifndef NOFLOAT
	case FLOAT:
	case DOUBLE:
		return T_FLOATING;
#endif NOFLOAT
	case POINTER:	/* pointer : signed / unsigned	???	*/
		return T_SIGNED;
	}
	return 0;
}
