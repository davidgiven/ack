/* unfortunately, we cannot just do an atof and convert result to float
   is neccessary, because that might result in over/underflow at an
   unexpected moment. For the time being, overflow in floating point
   constants goes undetected
*/
con_float(argval, str, res)
	char *str, *res;
{
	double f;
	double atof();
	int i;
	double frexp(), ldexp();
	int j;
	float fl;
	char *p;

	f = atof(str);
	if (f == 0 || argval == 8) {
		p = (char *) &f;
		while (argval--) *res++ = *p++;
		return;
	}
	f = frexp(f, &i);
	fl = f;
	fl = frexp(fl,&j);
	if (i+j > 127) {
		/* overflow situation. Unfortunately we don't have the
		   slightest idea where the floating point constant came
		   from, so, for the time being, just ignore
		*/
		*res++ = f < 0 ? 0377 : 0177;
		*res++ = 0377;
		*res++ = 0377;
		*res = 0377;
		return;
	}
	if (i+j < -127) {
		/* underflow situation. Unfortunately we don't have the
		   slightest idea where the floating point constant came
		   from, so, for the time being, just ignore
		*/
		*res++ = f < 0 ? 0200 : 0;
		*res++ = 0200;
		*res++ = 0;
		*res = 0;
		return;
	}
	fl = ldexp(fl, i+j);
	p = (char *) &fl;
	while (argval--) *res++ = *p++;
}
