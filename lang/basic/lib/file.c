#include "bc_string.h"
#include <stdio.h>
#include "bc_io.h"

/* $Id$ */

Filedesc	_fdtable[16];
/* BASIC file descriptor table */
/* Channel assignment:
   -1		terminal IO
    0		data file
    1-15	user files
*/



int	_chann		= -1;
FILE	*_chanrd	= stdin;
FILE	*_chanwr	= stdout;

_setchan(index)
int index;
{
#ifdef DEBUG
	printf("setchannel %d\n",index);
#endif
	fflush(_chanwr);
	if( index == -1)
	{
		_chann= -1;
		_chanrd= stdin;
		_chanwr= stdout;
		return;
	}
	if( index<0 || index>15)
		error(27);
	_chann=index;
	_chanrd= _chanwr= _fdtable[index].fd;
}

_asschn()
{
#ifdef DEBUG
	printf("_asschn %d\n",_chann);
#endif
	if( _chann == -1) return;
#ifdef DEBUG
	printf(" file %d\n", _fdtable[_chann].fd);
#endif
	if( _chann<0 || _chann>15)
		error(27);
	if( _fdtable[_chann].fd== 0)
		error(39);
	if( feof( _fdtable[_chann].fd))
		error(2);
}

_clochn(nr)
int nr;
{
	if( nr<1 || nr >15 || _fdtable[nr].fd==0) error(3);
	fclose(_fdtable[nr].fd);
	_fdtable[nr].fd=0; _fdtable[nr].fname=0;
}

_opnchn(reclen,fname,mode)
String *mode,*fname;
int	reclen;
{
	/* channel has been set */
	FILE *f;
	int m;

#ifdef DEBUG
	printf("open %d %s %s \n",reclen,mode->strval,fname->strval);
#endif
	/* check for opened/closed file */
	if(_fdtable[_chann].fd)
		error(30);
	switch(*mode->strval)
	{
		case 'O':
		case 'o':
			if( (f=fopen(fname->strval,"w")) == NULL)
				error(28);
			m= OMODE;
			break;
		case 'I':
		case 'i':
			if( (f=fopen(fname->strval,"r")) == NULL)
				error(28);
			m= IMODE;
			break;
		case 'r':
		case 'R':
			if( (f=fopen(fname->strval,"a")) == NULL)
				error(28);
			m= RMODE;
			break;
		default:
			printf("file mode %s\n",mode->strval);
			error(29);
	}
	_chanwr= _chanrd= _fdtable[_chann].fd= f;
	_fdtable[_chann].fname= fname->strval;
	_fdtable[_chann].reclength= reclen;
	_fdtable[_chann].mode= m;
#ifdef DEBUG
	printf("file descr %d\n",f);
#endif
}

_ioeof(channel)
int channel;
{
	FILE *fd;
	char c;
	if( channel<0 || channel >15) error(3);
	fd= _fdtable[channel].fd;
	if( fd==0)
		error(3);
	c=fgetc(fd);
	if( feof(_fdtable[channel].fd) ) return(-1);
	ungetc(c,fd);
	return(0);
}

_close()
{
	/* close all open files */
	int i;
	for(i=1;i<16;i++)
	if( _fdtable[i].fd)
		_clochn(i);
}
