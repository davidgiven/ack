/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	"../lpass1/l_class.h"
#include	"class.h"
#include	"inpdef.h"

#include	<ctype.h>

#define	INP_NPUSHBACK 2

#include	<inp_pkg.spec>
#include	<inp_pkg.body>

#include	"private.h"

PRIVATE int LineNr = 1;

/* Two dangerous macro's. They replace a single statement by
 * two statements
 */
#define	loadchar(ch) LoadChar(ch); if (ch=='\n') LineNr++
#define	pushback(ch) PushBack(); if (ch=='\n') LineNr--

PRIVATE int ReadString();
PRIVATE int ReadInt();
PRIVATE SkipChar();
PRIVATE int ReadArgs();

int
get_id(id)
	struct inpdef *id;
{
	/* A low-level function which just reads a definition */

	if (!ReadString(id->id_name, ':', NAMESIZE))
		return 0;
	if (!ReadInt(&id->id_statnr))
		return 0;
	SkipChar(':');

	loadchar(id->id_class);
	if (id->id_class == EOI)
		return 0;
	SkipChar(':');

	if (is_class(id, CL_FUNC|CL_DEF) || is_class(id, CL_FUNC|CL_USAGE)) {
		/* read the argument information */
		id->id_args = 1;
		if (!ReadInt(&id->id_nrargs))
			return 0;
		SkipChar(':');
		if (!ReadArgs(id->id_nrargs, id->id_argtps))
			return 0;
		if (id->id_class == FC) {
			/* function call */
			if (!ReadInt(&id->id_valused))
				return 0;
		}
		else {
			/* function definition */
			if (!ReadInt(&id->id_valreturned))
				return 0;
		}
		SkipChar(':');
	}
	else {
		id->id_args = 0;
	}

	if (!ReadString(id->id_type, ':', TYPESIZE))
		return 0;
	if (!ReadInt(&id->id_line))
		return 0;
	SkipChar(':');

	if (!ReadString(id->id_file, '\n', FNAMESIZE))
		return 0;
	{	extern char loptions[];
		if (loptions['X'])
			print_id("read", id);/*???*/
	}
	return (1);
}

PRIVATE int
ReadString(buf, delim, maxsize)
	char *buf;
{
	/*	Reads a string until 'delim' is encountered.
		Delim is discarded.
		If 'maxsize-1' is exceeded, "string too long" is written
		by panic().
		A '\0' is appended to the string.
		At EOI 0 is returned, else the length of the string (including
		the appended '\0') is returned.
	*/

	int ch = 0;
	int nread = 0;

	while (nread < maxsize - 1) {
		loadchar(ch);
		if (ch == EOI)
			return 0;
		if (ch == delim)
			break;
		buf[nread++] = (char)ch;
	}
	buf[nread++] = '\0';
	if (ch != delim) {
		panic("line %d: string too long: %s", LineNr, buf);
		/*NOTREACHED*/
	}
	return (nread);
}

PRIVATE int
ReadInt(ip)
	int *ip;
{
/* Reads a decimal integer until a character which is not
 * a digit is encountered.
 * Non-digits except minus-sign in front of the number are discarded.
 * Doesn't check on overflow.
 * Just a minus-sign is interpreted as 0. (To prevent a look-ahead.)
 * At EOI 0 is returned, else 1.
 */
	int ch;
	int negative = 0;
	int res = 0;

	do {
		loadchar(ch);
	} while (!isdigit(ch) && ch != '-');
	if (ch == EOI)
		return 0;
	if (ch == '-')
		negative = 1;
	else
		res = ch - '0';
	loadchar(ch);
	while (isdigit(ch)) {
		res = 10*res + ch - '0';
		loadchar(ch);
	}
	pushback(ch);
	*ip = negative ? -res : res;
	return 1;
}

PRIVATE SkipChar(ch)
{
	int c;

	loadchar(c);
	if (c != ch) {
		panic("line %d: bad format, '%c' expected; '%c' read",
				LineNr, ch, c);
		/*NOTREACHED*/
	}
}

PRIVATE int
ReadArgs(nrargs, buf)
	char *buf;
{
	/*	Reads a string into buf with format
			<type1>:<type2>: ... :<typeN>:
		Note: format must include the final colon.
	*/
	int i;
	int charcount = 1;
	int n;

	if (nrargs < 0) {
		/* variable # of args */
		nrargs = -nrargs - 1;
	}
	*buf = '\0';
	for (i = 0; i < nrargs; i++) {
		if (!ReadString(buf, ':', ARGTPSSIZE-charcount-1))
			return 0;
		n = strlen(buf) + 1;
		charcount += n;
		buf += n - 1;
		*buf++ = ':';
	}
	*buf = '\0';
	return 1;
}

