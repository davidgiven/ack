# include "FP_types.h"

prt_dbl(dbl,size)
DOUBLE	*dbl;
int	size;
{
#ifdef	PRT_DBL
	unsigned long	*l;

	fprintf(stderr,"PRT_DBL SIZE = %d ",size);
	fprintf(stderr,"_s.p1.fract = 0x%08X ",dbl->_s.p1.fract);
	if (size == 8)
		fprintf(stderr,"_s.p2 = 0x%08X",dbl->_s.p2);
	l = (unsigned long *) dbl;
#ifdef	PRT_LONG
	fprintf(stderr,"\nl[0] = 0x%08X ",*l++);
	if (size == 8)
		fprintf(stderr,"l[1] = 0x%08X",*l);
#endif	PRT_LONG
	putc('\r',stderr);
	putc('\n',stderr);
	fflush(stderr);
#endif
}

