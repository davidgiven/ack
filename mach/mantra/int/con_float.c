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
	fl = f;
	p = (char *) &fl;
	while (argval--) *res++ = *p++;
}
