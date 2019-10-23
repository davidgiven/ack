/** @file
 *  Sources of the "ARRAY" group instructions
 */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"log.h"
#include	"trap.h"
#include	"mem.h"
#include	"text.h"
#include	"fra.h"
#include	"switch.h"

#define	LAR		1
#define	SAR		2
#define	AAR		3

PRIVATE void arr(int, size);


void DoLAR(size arg)
{
	/* LAR w: Load array element, descriptor contains integers of size w */
	LOG(("@A6 DoLAR(%ld)", arg));
	arr(LAR, arg_wi(arg));
}

void DoSAR(size arg)
{
	/* SAR w: Store array element */
	LOG(("@A6 DoSAR(%ld)", arg));
	arr(SAR, arg_wi(arg));
}

void DoAAR(size arg)
{
	/* AAR w: Load address of array element */
	LOG(("@A6 DoAAR(%ld)", arg));
	arr(AAR, arg_wi(arg));
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

PRIVATE void arr(int type, /* operation TYPE */
		size elm_size /* ELeMent SIZE */
		)
{
	register ptr desc = dppop();	/* array DESCriptor */
	register size obj_size;		/* OBJect SIZE */
	long index = spop(elm_size);
	long diff =		/* between index and lower bound */
		index - mem_lds(desc, elm_size);
	register ptr arr_addr = dppop();/* ARRay ADDRess */

	if (must_test && !(IgnMask&BIT(EARRAY))) {
		if (diff < 0 || index > mem_lds(desc + elm_size, elm_size)) {
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
