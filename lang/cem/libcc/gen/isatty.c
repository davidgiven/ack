/* $Id$ */
isatty(f)
{
	char buf[128];
	/* not a sgttyb struct; it might not be large enough;
	   I know for a fact that it is'nt large enough on PC/IX,
	   where gtty is an ioctl(..., TCGETA, ...)
	*/

	if (gtty(f, buf) < 0) return 0;
	return 1;
}
