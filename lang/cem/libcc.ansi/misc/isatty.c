/*
 * isatty - check if a file descriptor is associated with a terminal
 */
/* $Id$ */

int _isatty(int d);

int isatty(int d)
{
	return _isatty(d);
}
