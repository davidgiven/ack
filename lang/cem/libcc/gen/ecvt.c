#ifndef NOFLOAT
extern double modf();
static char *cvt();
#define NDIGITS	128

char *
ecvt(value, ndigit, decpt, sign)
	double value;
	int ndigit, *decpt, *sign;
{
	return cvt(value, ndigit, decpt, sign, 1);
}

char *
fcvt(value, ndigit, decpt, sign)
	double value;
	int ndigit, *decpt, *sign;
{
	return cvt(value, ndigit, decpt, sign, 0);
}

static char *
cvt(value, ndigit, decpt, sign, ecvtflag)
	double value;
	int ndigit, *decpt, *sign;
{
	double intpart, fractpart;
	static char buf[NDIGITS];
	char buf1[NDIGITS];
	register char *pe = buf1;
	register  char *pb = buf;
	int pointpos = 0;


	if (ndigit < 0) ndigit = 0;
	if (ndigit >= NDIGITS - 1) ndigit = NDIGITS - 2;

	*sign = 0;
	if (value < 0) {
		*sign = 1;
		value = -value;
	}

	fractpart = modf(value, &intpart);
	if (intpart != 0) {
		do {	/* get digits of integer part, low order digit
			   first
			*/
			value = modf(intpart/10, &intpart);
			/* compensate for rounding errors, because
			   the conversion to "int" truncates
			*/
			*pe++ = (int)((value+.05) * 10) + '0';
			pointpos++;
		} while (intpart != 0);
		while (pe > buf1) *pb++ = *--pe;
	}
	else if (value > 0) {
		fractpart = value;
		while ((value = value*10) < 1) {
			fractpart = value;
			pointpos--;
		}
	}
	pe = &buf[ndigit];
	if (! ecvtflag) {
		/* for fcvt() we need ndigit digits behind the dot */
		pe += pointpos;
		if (pe < buf) {
			/* pointpos was too far left of the beginning */
			buf[0] = 0;
			*decpt = pointpos;
			return buf;
		}
		if (pe > &buf[NDIGITS]) pe = &buf[NDIGITS];
	}
	while (pb <= pe) {
		fractpart = modf(fractpart * 10, &value);
		*pb++ = (int)value + '0';
	}
	pb = pe;
	*pb += 5;	/* round of at the end */
	while (*pb > '9') {
		*pb = '0';
		if (pb > buf) ++*--pb;
		else {
			*pb = '1';
			pointpos++;
			if (! ecvtflag) {
				/* maybe add another digit at the end,
				   because the point was shifted right
				*/
				if (pe > buf) *pe = '0';
				pe++;
			}
		}
	}
	*decpt = pointpos;
	*pe = '\0';
	return(buf);
}
#endif
