/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
	int from_fund = fundamental(from_type);
	int to_fund = fundamental(to_type);

	if ((int)to_size < (int)word_size) to_size = word_size;
	if ((int)from_size == (int)to_size && from_fund == to_fund)
		return;
	switch (from_fund)	{
	case T_SIGNED:
		switch (to_fund)	{
		case T_SIGNED:
			C_loc(from_size);
			C_loc(to_size);
			C_cii();
			break;
		case T_UNSIGNED:
#ifndef NOFLOAT
		case T_FLOATING:
#endif NOOFLOAT
			if ((int)from_size < (int)word_size) {
				C_loc(from_size);
				C_loc(word_size);
				C_cii();
				from_size = word_size;
			}
			C_loc(from_size);
			C_loc(to_size);
			if (to_fund == T_UNSIGNED) C_ciu();
			else C_cif();
			break;
		}
		break;
	case T_UNSIGNED:
		if ((int)from_size < (int)word_size) from_size = word_size;
		C_loc(from_size);
		C_loc(to_size);
		switch (to_fund)	{
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
		C_loc(to_size);
		switch (to_fund)	{
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
	if ((int)(to_type->tp_size) < (int)word_size
#ifndef NOFLOAT
	    && to_fund != T_FLOATING
#endif NOFLOAT
	    ) {
		extern long full_mask[];

		if (to_fund == T_SIGNED) {
			C_loc(to_type->tp_size);
			C_loc(word_size);
			C_cii();
		}
		else {
			C_loc((arith) full_mask[(int)(to_type->tp_size)]);
			C_and(word_size);
		}
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
