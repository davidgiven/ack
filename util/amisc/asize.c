/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdio.h>
#include 	"out.h"

/*
	asize -- determine object size

*/

main(argc, argv)
char **argv;
{
	struct outhead	buf;
	struct outsect	sbuf;
	unsigned short	nrsect;
	long		sum;
	int		gorp;

	if (--argc == 0) {
		argc = 1;
		argv[1] = "a.out";
	}
	gorp = argc;

	while(argc--) {
		if (! rd_open(*++argv)) {
			fprintf(stderr, "asize: cannot open %s\n", *argv);
			continue;
		}
		rd_ohead(&buf);
		if(BADMAGIC(buf)) {
			fprintf(stderr, "asize: %s-- bad format\n", *argv);
			rd_close();
			continue;
		}
		nrsect = buf.oh_nsect;
		if (nrsect == 0) {
			fprintf(stderr, "asize: %s-- no sections\n", *argv);
			rd_close();
			continue;
		}
		if (gorp > 1)
			printf("%s: ", *argv);

		sum = 0;
		while (nrsect-- > 0) {
			rd_sect(&sbuf, 1);
			printf("%ld", sbuf.os_size);
			sum += sbuf.os_size;
			if (nrsect > 0)
				putchar('+');
		}
		printf(" = %ld = 0x%lx\n", sum, sum);
		rd_close();
	}
	exit(0);
}

rd_fatal()
{
	fprintf(stderr, "read error\n");
	exit(2);
}
