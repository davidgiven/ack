__IsNan(double d)
{
#if defined(__vax) || defined(__pdp)
#else
	float f = d;

	if ((*((long *) &f) & 0x7f800000) == 0x7f800000 &&
	    (*((long *) &f) & 0x007fffff) != 0) return 1;
#endif
	return 0;
}
