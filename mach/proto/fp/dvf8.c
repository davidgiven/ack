/*
	DIVIDE TWO FLOATS - DOUBLE Precision
*/

#include	"FP_types.h"

dvf8(s2,s1)
_double	s1,s2;
{
	EXTEND	e1,e2;

	extend(&s1,&e1,sizeof(_double));
	extend(&s2,&e2,sizeof(_double));

		/* do a divide */
	div_ext(&e1,&e2);
	compact(&e1,&s1,sizeof(_double));
}
