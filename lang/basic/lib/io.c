#include "bc_io.h"
#include <sgtty.h>

/* $Id$ */

struct sgttyb _ttydef;

/* BASIC has some nasty io characteristics */

#define MAXWIDTH 255

int	_width = 75, _pos=0, _zonewidth=15;

_out(str)
char *str;
{
	int pos;

	if( _chann== -1) pos= _pos;
	else pos= _fdtable[_chann].pos;
	while( *str) 
	{
		if( pos>= _width){ _outnl(); pos=0;}
		fputc(*str++, _chanwr);
		pos++;
	}
	if( _chann== -1) _pos=pos;
	else _fdtable[_chann].pos= pos;
}

_outnl()
{
	fputc('\n',_chanwr);
	if( _chann == -1)
		_pos=0;
	else
		_fdtable[_chann].pos=0;
}
_zone()
{
	/* go to next zone */
	int pos;
	if( _chann == -1)
		pos= _pos;
	else pos= _fdtable[_chann].pos;
	do{
		fputc(' ',_chanwr);
		pos++;
		if( pos==_width)
		{
			_outnl();
			pos=0;
			break;
		}
	} while( pos % _zonewidth != 0);
	if( _chann== -1) _pos=pos;
	else _fdtable[_chann].pos= pos;
}
_in(buf)
char *buf;
{
	register int holder ;
	char *c;
	int pos;
	if( _chann == -1)
	{
		pos= _pos;
		gtty(0,_ttydef);
		_ttydef.sg_flags &= ~ECHO;
		stty(0,_ttydef);
	}else pos= _fdtable[_chann].pos;
	c= buf;
	while( (holder = fgetc(_chanrd)) != EOF && holder != '\n'){
		*c= holder ;
		if( _chann == -1) putchar(holder);
		c++; pos++;
	}
	*c= 0;
	if( _chann== -1) 
	{
		_pos=pos;
		_ttydef.sg_flags |= ECHO;
		stty(0,_ttydef);
	} else _fdtable[_chann].pos= pos;
}
_tab(x)
int x;
{
	if( x> _width) error(3);
	if( x< _pos) _outnl();
	_spc(x-_pos);
}
_spc(x)
int x;
{
	while(x-->0) _out(" ");
}
