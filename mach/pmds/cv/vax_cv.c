/* The format of the a.out files produced by the assemblers
   is machine dependent.
   This program acts as a gateway between two machines and it's effect
   is independent of the machine it executes on.
   The a.out file is assumed to be made on a vax-11
   while the target machine is a Philips Microcomputer Development system

*/

#include <stdio.h>

main(argc,argv) char **argv ; {
	char i_addr[4];
	short count;
	char i_count[2];

	if (argc != 3) {
		fprintf(stderr,"Usage: %s vax-a.out VU-pmds-a.out\n",argv[0]);
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
	while (fread(&i_addr,sizeof i_addr,1,stdin)==1) {
		putchar(i_addr[3]) ; putchar(i_addr[2]) ;
		putchar(i_addr[1]) ; putchar(i_addr[0]) ;
		if (fread(&i_count,sizeof i_count,1,stdin)!=1)
			exit(fprintf(stderr,"foo\n"));
		putchar(i_count[1]) ; putchar(i_count[0]) ;
		count= ((i_count[1]&0377)<<8) | (i_count[0]&0377) ;
		while (count--) {
			putchar(getchar());
		}
	}
	return 0;
}
