/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include	"l_class.h"
#include	"class.h"

int class[MAX_CLASS_CONST-MIN_CLASS_CONST+1];

init_class()
{
	/* mapping of class values onto bit patterns */
	class[LFDF-LFDF] = CL_LIB | CL_FUNC | CL_DEF;
	class[LVDF-LFDF] = CL_LIB | CL_VAR | CL_DEF;
	class[PFDF-LFDF] = CL_EXT | CL_FUNC | CL_DEF;
	class[EFDF-LFDF] = CL_EXT | CL_FUNC | CL_DEF;
	class[EVDF-LFDF] = CL_EXT | CL_VAR | CL_DEF;
	class[EFDC-LFDF] = CL_EXT | CL_FUNC | CL_DECL;
	class[EVDC-LFDF] = CL_EXT | CL_VAR | CL_DECL;
	class[IFDC-LFDF] = CL_IMPL | CL_FUNC | CL_DECL;
	class[SFDF-LFDF] = CL_STAT | CL_FUNC | CL_DEF;
	class[SVDF-LFDF] = CL_STAT | CL_VAR | CL_DEF;
	class[FC-LFDF] = CL_FUNC | CL_USAGE;
	class[VU-LFDF] = CL_VAR | CL_USAGE;
	class[XXDF-LFDF] = 0;
}

