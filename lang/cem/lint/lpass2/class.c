/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	"../lpass1/l_class.h"
#include	"class.h"

int class[] = {
	/* mapping of class values onto bit patterns */
	/* LFDF */ CL_LIB|CL_FUNC|CL_DEF,
	/* LVDF */ CL_LIB|CL_VAR|CL_DEF,
	/* EFDF */ CL_EXT|CL_FUNC|CL_DEF,
	/* EVDF */ CL_EXT|CL_VAR|CL_DEF,
	/* EFDC */ CL_EXT|CL_FUNC|CL_DECL,
	/* EVDC */ CL_EXT|CL_VAR|CL_DECL,
	/* IFDC */ CL_IMPL|CL_FUNC|CL_DECL,
	/* SFDF */ CL_STAT|CL_FUNC|CL_DEF,
	/* SVDF */ CL_STAT|CL_VAR|CL_DEF,
	/* FC */ CL_FUNC|CL_USAGE,
	/* VU */ CL_VAR|CL_USAGE
};

static int val[] = {
	LFDF,
	LVDF,
	EFDF,
	EVDF,
	EFDC,
	EVDC,
	IFDC,
	SFDF,
	SVDF,
	FC,
	VU
};

init_class()
{
	/*	The initialization of class[] has been taken care of above.
		For it to work, we have to test that the class values are
		in the right order.  This is also necessary for the scanning
		sequence in lpass2.c to work properly.
	*/
	int i;

	for (i = 0; i+1 < sizeof(val)/sizeof(val[0]); i++) {
		if (val[i] >= val[i+1])
			panic("class values out of sequence");
	}
}

