#include "../lpass1/l_class.h"
#include "inpdef.h"

#include <ctype.h>

#define	INP_NPUSHBACK 2

#include <inp_pkg.spec>
#include <inp_pkg.body>

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
	switch (id->id_class) {
	case EFDF:
	case SFDF:
	case LFDF:
	case FC:
		if (!ReadInt(&id->id_nrargs))
			return 0;
		SkipChar(':');
		if (!ReadArgs(id->id_nrargs, id->id_argtps))
			return 0;
		if (!ReadInt(&id->id_returns))
			return 0;
		SkipChar(':');
		break;
	}
	if (!ReadString(id->id_type, ':', TYPESIZE))
		return 0;
	if (!ReadInt(&id->id_line))
		return 0;
	SkipChar(':');
	if (!ReadString(id->id_file, '\n', FNAMESIZE))
		return 0;
	{	extern char options[];
		if (options['X'])
			print_id(id);/*???*/
	}
	return (1);
}

PRIVATE int
ReadString(buf, delim, maxsize)
	char *buf;
{
/* Reads a string until 'delim' is encountered.
 * Delim is discarded.
 * If 'maxsize-1' is exceeded, "string too long" is written by panic().
 * A '\0' is appended to the string.
 * At EOI 0 is returned, else the length of the string (including
 * the appended '\0') is returned.
 */
	int ch;
	int nread = 0;

	while (nread < maxsize - 1) {
		loadchar(ch);
		if (ch == EOI)
			return 0;
		if (ch == delim)
			break;
		buf[nread++] = (char)ch;
	}
	if (ch != delim) {
		panic("line %d: string too long: %s", LineNr, buf);
		/*NOTREACHED*/
	}
	buf[nread++] = '\0';
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
/* Reads a string into buf with format <type1>:<type2>: ... :<typeN>: */

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

