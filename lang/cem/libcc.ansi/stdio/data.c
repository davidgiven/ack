/*
 * data.c - this is the initialization for the standard streams
 */
/* $Header$ */

#include	<stdio.h>

struct _iobuf _stdin = {
	0, 0, _IOREAD, 0,
	(char *)NULL, (unsigned char *)NULL, (unsigned char *)NULL, 
};

struct _iobuf _stdout = {
	1, 0, _IOWRITE, 0,
	(char *)NULL, (unsigned char *)NULL, (unsigned char *)NULL, 
};

struct _iobuf _stderr = {
	2, 0, _IOWRITE | _IONBF, 0,
	(char *)NULL, (unsigned char *)NULL, (unsigned char *)NULL, 
};

struct  _iobuf  *_iotable[FOPEN_MAX] = {
	&_stdin,
	&_stdout,
	&_stderr,
	0
};
