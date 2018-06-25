/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	double abs function
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/
#include "libm2.h"

#ifndef NOFLOAT
double absd(double i)
{
	return i >= 0 ? i : -i;
}
#endif
