/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdio.h>

unsigned memaddr = 0x100;

main() {
	char buf[256];
	register i,len;

	while((len=read(0,buf,sizeof(buf))) > 0) {
		putw(memaddr,stdout);
		putw(0,stdout);
		putw(len,stdout);
		memaddr += len;
		for(i=0;i<len;i++)
			putc(buf[i],stdout);
	}
}
