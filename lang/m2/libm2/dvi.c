/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	implementation of DIV and MOD
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
  Reason:	We cannot use DVI and RMI, because DVI rounds towards 0
        and Modula-2 requires truncation
*/

#include "libm2.h"
#include <em_abs.h>

int dvi(int j, int i)
{
	if (j == 0)
		TRP(EIDIVZ);
	if ((i < 0) != (j < 0))
	{
		if (i < 0)
			i = -i;
		else
			j = -j;
		return -((i + j - 1) / j);
	}
	else
		return i / j;
}

long dvil(long j, long i)
{
	if (j == 0)
		TRP(EIDIVZ);
	if ((i < 0) != (j < 0))
	{
		if (i < 0)
			i = -i;
		else
			j = -j;
		return -((i + j - 1) / j);
	}
	else
		return i / j;
}

int rmi(int j, int i)
{
	int m;

	if (j == 0)
		TRP(EIDIVZ);
	if (i == 0)
		return 0;

	m = i % j;
	if (m != 0 && (i < 0) != (j < 0))
		m += j;
	return m;
}

long rmil(long j, long i)
{
	long m;

	if (j == 0)
		TRP(EIDIVZ);
	if (i == 0)
		return 0L;

	m = i % j;
	if (m != 0 && (i < 0) != (j < 0))
		m += j;
	return m;
}
