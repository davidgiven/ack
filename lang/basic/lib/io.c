#include "bc_io.h"

/* dtrg --- this originally used sgtty.h to do clever tty manipulation.
 * Strictly this should be converted to use termios, but for simplicity
 * we're going to stick with plain stdio for now. */

/* BASIC has some nasty io characteristics */

#define MAXWIDTH 255

int _width = 75, _pos = 0, _zonewidth = 15;

void _out(char* str)
{
	int pos;

	if (_chann == -1)
		pos = _pos;
	else
		pos = _fdtable[_chann].pos;
	while (*str)
	{
		if (pos >= _width)
		{
			_outnl();
			pos = 0;
		}
		fputc(*str++, _chanwr);
		pos++;
	}
	if (_chann == -1)
		_pos = pos;
	else
		_fdtable[_chann].pos = pos;
}

void _outnl(void)
{
	fputc('\n', _chanwr);
	if (_chann == -1)
		_pos = 0;
	else
		_fdtable[_chann].pos = 0;
}
void _zone(void)
{
	/* go to next zone */
	int pos;
	if (_chann == -1)
		pos = _pos;
	else
		pos = _fdtable[_chann].pos;
	do
	{
		fputc(' ', _chanwr);
		pos++;
		if (pos == _width)
		{
			_outnl();
			pos = 0;
			break;
		}
	} while (pos % _zonewidth != 0);
	if (_chann == -1)
		_pos = pos;
	else
		_fdtable[_chann].pos = pos;
}
void _in(char* buf)
{
	register int holder;
	char* c;
	int pos;
	if (_chann == -1)
	{
		pos = _pos;
	}
	else
		pos = _fdtable[_chann].pos;
	c = buf;
	while ((holder = fgetc(_chanrd)) != EOF && holder != '\n')
	{
		*c = holder;
		if (_chann == -1)
			putchar(holder);
		c++;
		pos++;
	}
	*c = 0;
	if (_chann == -1)
	{
		_pos = pos;
	}
	else
		_fdtable[_chann].pos = pos;
}
void _tab(int x)
{
	if (x > _width)
		error(3);
	if (x < _pos)
		_outnl();
	_spc(x - _pos);
}
void _spc(int x)
{
	while (x-- > 0)
		_out(" ");
}
