/* $Id$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */
#include "pc.h"

/* function argc:integer; extern; */
/* function argv(i:integer):string; extern; */
/* procedure argshift; extern; */
/* function environ(i:integer):string; extern; */

int argc(void)
{
	return (_pargc);
}

char* argv(int i)
{
	if (i >= _pargc)
		return (0);
	return (_pargv[i]);
}

void argshift(void)
{

	if (_pargc > 1)
	{
		--_pargc;
		_pargv++;
	}
}
