static char rcsid[] = "$Id$";
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
#include <a.out.h>

/*
 * NOTE: Beware that the a.out.h file included here should be the a.out.h
 *       file of the TARGET machine, not of the SOURCE machine.
 */

struct bhdr s_exec;

main(argc,argv) char **argv; {
	unsigned short losh,hish;
	long addr,maxaddr;
	short count;
	
	maxaddr=0;
	if (argc != 3) {
		fprintf(stderr,"Usage: %s VU-a.out Bleasdale-a.out\n",argv[0]);
		exit(-1);
	}
	if (freopen(argv[1],"r",stdin)==NULL) {
		perror(argv[1]);
		exit(-1);
	}
	if (freopen(argv[2],"w",stdout)==NULL) {
		perror(argv[2]);
		exit(-1);
	}
	while (fread(&hish,sizeof(short),1,stdin)==1) {
		if (fread(&losh,sizeof(short),1,stdin)!=1)
			exit(fprintf(stderr,"foo\n"));
		addr=losh+(((long)hish)*65536L);
		addr -= 0x20000;  /* entry point is 0x20000 on Bleasdale */
		if (fread(&count,sizeof(short),1,stdin)!=1)
			exit(fprintf(stderr,"bar\n"));
		fseek(stdout,addr+sizeof(s_exec),0);
		while (count--) {
			putchar(getchar());
			addr++;
		}
		if (addr>maxaddr)
			maxaddr = addr;
	}
	s_exec.fmagic = FMAGIC;
	s_exec.dsize = maxaddr;
	s_exec.entry = 0x20000;
	fseek(stdout,0L,0);
	fwrite(&s_exec,sizeof(s_exec),1,stdout);
	chmod(argv[2],0755);
	return 0;
}

		
