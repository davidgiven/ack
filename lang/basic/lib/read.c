#include "bc_io.h"
#include <ctype.h>
#include <stdlib.h>
#include "lib.h"

/*
 * DATA statements are in the program: the compiler emits their text as
 * _datatext, one line per DATA statement ("1,2,\"abc\"\n"), and _seektab,
 * pairs of (BASIC line number, line index) ending with 0. READ (channel 0)
 * reads it through _datptr with the same rules as reading a file: skip
 * white space, read the item, skip to after the next ',' or newline.
 */
extern char _datatext[];
extern int _seektab[];
static char* _datptr;

static void datskip(void)
{
	char* p = _datptr;

	while (*p && *p != ',' && *p != '\n')
		p++;
	if (*p)
		p++;
	_datptr = p;
}

/* The start of the next item; "Out of data" if there is none. */
static char* datitem(void)
{
	char* p = _datptr ? _datptr : _datatext;

	while (isspace((unsigned char)*p))
		p++;
	if (!*p)
		error(2);
	return p;
}

static void _datint(int* addr)
{
	char* p = datitem();
	char* end;
	long l = strtol(p, &end, 10);

	if (end == p)
		error(40);
	_datptr = end;
	datskip();
	*addr = (int)l;
}

static void _datflt(double* addr)
{
	char* p = datitem();
	char* end;
	double d = strtod(p, &end);

	if (end == p)
		error(40);
	_datptr = end;
	datskip();
	*addr = d;
}

static void _datstr(String** s)
{
	char buffer[MAXSTRING];
	char* c = buffer;
	char* p = datitem();

	if (*p == '"')
	{
		/* quoted: up to the closing quote */
		p++;
		while (*p && *p != '"' && c < buffer + MAXSTRING - 1)
			*c++ = *p++;
	}
	else if (isalpha((unsigned char)*p))
	{
		/* unquoted: up to a ',' or white space */
		while (*p && *p != ',' && !isspace((unsigned char)*p) && c < buffer + MAXSTRING - 1)
			*c++ = *p++;
	}
	else
		error(40);
	*c = 0;
	_datptr = p;
	datskip();
	_decstr(*s);
	*s = (String*)_newstr(buffer);
}

void _readln(void)
{
	int c;
	while ((c = fgetc(_chanrd)) != EOF && c != '\n')
		;
}

void readskip(void)
{
	int c;
#ifdef DEBUG
	printf("readskip\n");
#endif
	while ((c = fgetc(_chanrd)) != EOF && c != ',' && c != '\n')
		;
}
void _readint(int* addr)
{
	int i;
	char buf[1024];

#ifdef DEBUG
	printf("read int from %d\n", _chann);
#endif
	if (_chann == 0)
	{
		_datint(addr);
		return;
	}
	_asschn();
	if (fscanf(_chanrd, "%d", &i) != 1)
	{
		if (ferror(_chanrd))
			error(29);
		if (feof(_chanrd))
			error(2);
		if (_chann == -1)
		{
			_asschn(); /* may be closed by now */
			fgets(buf, 1024, _chanrd);
			printf("?Redo ");
			_readint(addr);
			return;
		}
		error(40);
	}
	else
	{
		readskip();
		*addr = i;
	}
}
void _readflt(double* addr)
{
	double f;
	char buf[1024];

#ifdef DEBUG
	printf("read flt from %d\n", _chann);
#endif
	if (_chann == 0)
	{
		_datflt(addr);
		return;
	}
	_asschn();
	if (fscanf(_chanrd, "%lf", &f) != 1)
	{
		if (ferror(_chanrd))
			error(29);
		if (feof(_chanrd))
			error(2);
		if (_chann == -1)
		{
			fgets(buf, 1024, _chanrd);
			printf("?Redo ");
			_readflt(addr);
			return;
		}
		error(40);
	}
	else
	{
		readskip();
		*addr = f;
	}
}
void _readstr(String** s)
{
	char buffer[1024];
	int kar;
	char* c;

#ifdef DEBUG
	printf("read str from %d\n", _chann);
#endif
	if (_chann == 0)
	{
		_datstr(s);
		return;
	}
	_asschn();
	c = buffer;
	kar = fgetc(_chanrd);
	while (isspace(kar) && kar != EOF)
		kar = fgetc(_chanrd);
	*c = kar;
	if (kar == '"')
	{
/* read quoted string */
#ifdef DEBUG
		printf("qouted string\n");
#endif
		while ((kar = fgetc(_chanrd)) != EOF && kar != '"')
			*c++ = kar;
		ungetc(kar, _chanrd);
		*c = 0;
	}
	else if (isalpha(*c))
	{
		/* read normal string */
		c++;
#ifdef DEBUG
		printf("non-qouted string\n");
#endif
		while ((kar = fgetc(_chanrd)) != ',' && kar != EOF && !isspace(kar) && kar != '\n')
			*c++ = kar;
		ungetc(kar, _chanrd);
		*c = 0;
	}
	else
	{
		if (ferror(_chanrd))
			error(29);
		if (feof(_chanrd))
			error(2);
		if (_chann == -1)
		{
			fgets(buffer, 1024, _chanrd);
			printf("?Redo ");
			_rdline(s);
			return;
		}
		error(40);
	}
#ifdef DEBUG
	printf("string read: %s\n", buffer);
#endif
	readskip();
	/* save value read */
	_decstr(*s);
	*s = (String*)_newstr(buffer);
}

void _restore(int line)
{
	int nr;
	char* p = _datatext;

#ifdef DEBUG
	printf("seek to %d", line);
#endif
	if (line)
	{
		/* the number of DATA lines before line */
		for (nr = 0; _seektab[nr] && _seektab[nr] < line; nr += 2)
			;
		nr /= 2;
		while (nr-- > 0)
		{
			while (*p && *p != '\n')
				p++;
			if (*p)
				p++;
		}
	}
	_datptr = p;
}
void _rdline(String** s)
{
	char buffer[1024];
	if (fgets(buffer, 1024, _chanrd) == 0)
	{
		/* At the end of the terminal input, or with stdin closed, asking
		 * again cannot help (it recursed until the stack ran out): the
		 * same errors as INPUT. */
		if (_chann == -1)
			error(ferror(_chanrd) ? 29 : 2);
		error(40);
	}
	_decstr(*s);
	*s = (String*)_newstr(buffer);
}
