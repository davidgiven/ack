/*
 * isatty - check if a file descriptor is associated with a terminal
 */
/* $Header$ */

int _isatty(int d);

int isatty(int d)
{
	return _isatty(d);
}
