/*
 * isatty - check if a file descriptor is associated with a terminal
 */
/* $Header$ */

int gtty(int d, char *buf);

int isatty(int d)
{
	char buf[128]; 
	/* not a sgttyb struct; it might not be large enough;
	   I know for a fact that it isn't large enough on PC/IX,
	   where gtty is an ioctl(..., TCGETA, ...)
	*/

	return gtty(d, buf) >= 0;
}
