/* $Id$ */
#ifndef NOFLOAT
extern double frexp();

double
ldexp(fl,exp)
	double fl;
	int exp;
{
	int sign = 1;
	int currexp;

	if (fl<0) {
		fl = -fl;
		sign = -1;
	}
	fl = frexp(fl,&currexp);
	exp += currexp;
	if (exp > 0) {
		while (exp>30) {
			fl *= (double) (1L << 30);
			exp -= 30;
		}
		fl *= (double) (1L << exp);
	}
	else	{
		while (exp<-30) {
			fl /= (double) (1L << 30);
			exp += 30;
		}
		fl /= (double) (1L << -exp);
	}
	return sign * fl;
}
#endif
