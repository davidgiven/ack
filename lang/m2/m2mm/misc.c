/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* M I S C E L L A N E O U S    R O U T I N E S */

/* stripped version from the one in the Modula-2 compiler */

/* $Id$ */

#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"

struct idf *
gen_anon_idf()
{
	/*	A new idf is created out of nowhere, to serve as an
		anonymous name.
	*/
	static int name_cnt;
	char buff[100];
	char *sprint();

	sprint(buff, "#%d in %s, line %u",
			++name_cnt, FileName, LineNumber);
	return str2idf(buff, 1);
}
