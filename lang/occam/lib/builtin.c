/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*	builtin.c - built in named processes */
#include "ocm_chan.h"
#ifndef nil
#define nil 0
#endif

extern int errno;

static void nullterm(s) register char *s;
/* Change Occam string to C string */
{
	register len= (*s & 0377);
	register char *p;

	while (--len>=0) {
		p=s++;
		*p = *s;
	}
	*s=0;
}

static void lenterm(s) register char *s;
/* Change C string to Occam string */
{
	register i=0;
	register c0, c1;

	c0=0;
	do {
		c1=s[i];
		s[i++]=c0;
		c0=c1;
	} while (c0!=0);
	*s= i-1;
}

void b_open(mode, name, index) register char *mode, *name; long *index;
/* PROC open(VAR index, VALUE name[], mode[])=	*/
{
	register FILE *fp;
	register i;

	nullterm(name);
	nullterm(mode);

	fp=fopen(name, mode);

	lenterm(name);
	lenterm(mode);

	if (fp==nil)
		*index= -errno;
	else {
		/* Find free file channel, there must be one free! */

		for (i=0; (file[i].f.flgs&C_F_INUSE)!=0; i++) ;

		file[i].f.flgs|=C_F_INUSE;
		unix_file[i]=fp;
		*index=i;
	}
}

void b_close(index) long index;
/* PROC close(VALUE index)=	*/
{
	fclose(unix_file[index]);
	file[index].f.flgs&= ~C_F_INUSE;
}

void b_exit(code) long code;
/* PROC exit(VALUE code)=	*/
{
	exit((int) code);
}
