/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	"../lpass1/l_class.h"
#include	"class.h"
#include	"inpdef.h"

#include	<ctype.h>

#define	INP_NPUSHBACK 1

#include	<inp_pkg.spec>
#include	<inp_pkg.body>

#include	"private.h"

int LineNr = 1;

/* Two dangerous macro's. They replace a single statement by
 * two statements
 */
#define	loadchar(ch) LoadChar(ch); if (ch=='\n') LineNr++
#define	pushback(ch) PushBack(); if (ch=='\n') LineNr--

/* all the ReadX() functions return 0 upon EOI */
PRIVATE int ReadString();
PRIVATE int ReadInt();
PRIVATE int ReadArgs();
PRIVATE int ReadArg();

PRIVATE SkipChar();

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
	return 1;
}

PRIVATE int
ReadString(buf, delim, maxsize)
	char *buf;
{
	/*	Reads a string until 'delim' is encountered; delim is
		discarded.
		If 'maxsize-1' is exceeded or the string contains a newline
		panic() is called (unless delim == newline).
		A '\0' is appended to the string.
	*/

	int ch = 0;
	int nread = 0;

	while (nread < maxsize - 1) {
		loadchar(ch);
		if (ch == EOI)
			return 0;
		if (ch == delim)
			break;
		if (ch == '\n') {
			panic("incomplete line in intermediate file");
			/*NOTREACHED*/
		}
		buf[nread++] = (char)ch;
	}
	buf[nread++] = '\0';
	if (ch != delim) {
		panic("line too long in intermediate file");
		/*NOTREACHED*/
	}
	return 1;
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
	*ip = (negative ? -res : res);
	return 1;
}

PRIVATE int
ReadArgs(nrargs, buf)
	char *buf;
{
	/*	Reads a string into buf with format
			<type1>:<type2>: ... :<typeN>:\0
		Note: format must include the final colon.
	*/
	int i;
	int charcount = 1;

	if (nrargs < 0) {
		/* variable # of args */
		nrargs = -nrargs - 1;
	}
	*buf = '\0';
	for (i = 0; i < nrargs; i++) {
		int n;

		if (!ReadArg(buf, ARGTPSSIZE-charcount-1))
			return 0;
		n = strlen(buf) + 1;
		charcount += n;
		buf += n - 1;
		*buf++ = ':';
	}
	*buf = '\0';
	return 1;
}

PRIVATE int
ReadArg(buf, size)
	char *buf;
	int size;
{
	int ch;

	loadchar(ch);
	switch (ch) {
	case '"':	/* formal format or actual string */
		*buf++ = ch;
		if (!ReadString(buf, ch, size-1))
			return 0;
		buf += strlen(buf);
		*buf++ = ch;
		*buf++ = '\0';
		SkipChar(':');
		return 1;
	default:	/* normal type */
		pushback(ch);
		return ReadString(buf, ':', size);
	case EOI:
		return 0;
	}
}

PRIVATE SkipChar(ch)
{
	int c;

	loadchar(c);
	if (c == ch)
		return;
	panic("bad format in intermediate file, '%c' expected; '%c' read",
		ch, c
	);
	/*NOTREACHED*/
}

