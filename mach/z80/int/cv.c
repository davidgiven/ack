/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
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
