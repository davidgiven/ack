/* $Id$ */

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

long _abl(long i)
{
	return( i>=0?i:-i);
}
double _abr(double f)
{	
	return( f>=0.0?f: -f);
}
