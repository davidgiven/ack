/*
 * Sources of the "ARRAY" group instructions
 */

/* $Header$ */

#include	<em_abs.h>
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"mem.h"
#include	"text.h"
#include	"fra.h"

#define	LAR		1
#define	SAR		2
#define	AAR		3

PRIVATE arr();

DoLARl2(arg)
	size arg;
{
	/* LAR w: Load array element, descriptor contains integers of size w */
	register size l = (L_arg_2() * arg);

	LOG(("@A6 DoLARl2(%ld)", l));
	arr(LAR, arg_wi(l));
}

DoLARm(arg)
	size arg;
{
	/* LAR w: Load array element, descriptor contains integers of size w */
	LOG(("@A6 DoLARm(%ld)", arg));
	arr(LAR, arg_wi(arg));
}

DoLARz()
{
	/* LAR w: Load array element, descriptor contains integers of size w */
	register size l = uwpop();

	LOG(("@A6 DoLARz(%ld)", l));
	arr(LAR, arg_wi(l));
}

DoSARl2(arg)
	size arg;
{
	/* SAR w: Store array element */
	register size l = (L_arg_2() * arg);

	LOG(("@A6 DoSARl2(%ld)", l));
	arr(SAR, arg_wi(l));
}

DoSARm(arg)
	size arg;
{
	/* SAR w: Store array element */
	LOG(("@A6 DoSARm(%ld)", arg));
	arr(SAR, arg_wi(arg));
}

DoSARz()
{
	/* SAR w: Store array element */
	register size l = uwpop();

	LOG(("@A6 DoSARz(%ld)", l));
	arr(SAR, arg_wi(l));
}

DoAARl2(arg)
	size arg;
{
	/* AAR w: Load address of array element */
	register size l = (L_arg_2() * arg);

	LOG(("@A6 DoAARl2(%ld)", l));
	arr(AAR, arg_wi(l));
}

DoAARm(arg)
	size arg;
{
	/* AAR w: Load address of array element */
	LOG(("@A6 DoAARm(%ld)", arg));
	arr(AAR, arg_wi(arg));
}

DoAARz()
{
	/* AAR w: Load address of array element */
	register size l = uwpop();

	LOG(("@A6 DoAARz(%ld)", l));
	arr(AAR, arg_wi(l));
}

/********************************************************
*		Array arithmetic			*
*							*
*	1. The address of the descriptor is popped.	*
*	2. The index is popped.				*
*	3. Calculate index - lower bound.		*
*	4. Check if in range.				*
*	5. Calculate object size.			*
*	6. Perform the correct function.		*
*********************************************************/

PRIVATE arr(type, elm_size)
	int type;			/* operation TYPE */
	size elm_size;			/* ELeMent SIZE */
{
	register ptr desc = dppop();	/* array DESCriptor */
	register size obj_size;		/* OBJect SIZE */
	register long diff =		/* between index and lower bound */
		spop(elm_size) - mem_lds(desc, elm_size);
	register ptr arr_addr = dppop();/* ARRay ADDRess */

	if (must_test && !(IgnMask&BIT(EARRAY))) {
		if (diff < 0 || diff > mem_lds(desc + elm_size, elm_size)) {
			trap(EARRAY);
		}
	}
	obj_size = mem_lds(desc + (2*elm_size), elm_size);
	obj_size = arg_o(((long) obj_size));
	spoilFRA();			/* array functions don't retain FRA */
	switch (type) {
		case LAR:
			push_m(arr_addr + diff * obj_size, obj_size);
			break;
		case SAR:
			pop_m(arr_addr + diff * obj_size, obj_size);
			break;
		case AAR:
			dppush(arr_addr + diff * obj_size);
			break;
	}
}
