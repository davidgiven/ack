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

#include	<sgtty.h>
#include	<stdio.h>
#include	<assert.h>

struct sgttyb	tty;

#define	DATTYPE		0
#define	EOFTYPE		1
#define	SEGTYPE		2
#define	PCTYPE		3

#define	MAXBYTE		0x18

int	check;
int	records;
int	echo;
int	bytecount;
int	ttyfd;

char	*progname;

char	hex[] = "0123456789ABCDEF";

main(argc,argv) char **argv; {
	register nd,pc,sg,osg,first;
	register char *s;

	progname = argv[0];
	if (argc > 3)
		fatal("usage: %s [object [tty]]\n",argv[0]);
	s = "a.out";
	if (argc >= 2)
		s = argv[1];
	if (freopen(s,"r",stdin) == NULL)
		fatal("can't open %s",s);
	ttyfd = 1;
	first = 1; osg = 0;
	for (;;) {
		pc = get2c(stdin);
		if (feof(stdin))
			break;
		sg = get2c(stdin);
		nd = get2c(stdin);
		if (first) {
			first = 0;
		}
		assert(sg == osg);
		while (nd > MAXBYTE) {
			data(MAXBYTE,pc);
			nd -= MAXBYTE;
			pc += MAXBYTE;
		}
		if (nd > 0)
			data(nd,pc);
		assert(feof(stdin) == 0);
	}
	if (first == 0)
		eof();
	if (echo)
		for (;;)
			reply();
}

data(nd,pc) {

	newline(nd,pc,DATTYPE);
	do
		byte(getc(stdin));
	while (--nd);
	endline();
}

eof() {

	newline(0,records,EOFTYPE);
	endline();
}

newline(n,pc,typ) {

	records++;
	put(';');
	byte(n);
	check = 0;
	bytecount = n+4;
	word(pc);
}

endline() {

	word(check);
	put('\r');
	put('\n');
	assert(bytecount == 0);
put(0);
put(0);
put(0);
put(0);
put(0);
put(0);
}

word(w) {

	byte(w>>8);
	byte(w);
}

byte(b) {

b &= 0377;
	check += b;
	--bytecount;
	put(hex[(b>>4) & 017]);
	put(hex[b & 017]);
}

put(c) {

	write(ttyfd,&c,1);
}

reply() {
	register i;
	int c;

	if (echo == 0)
		return;
	i = read(ttyfd,&c,1);
	assert(i > 0);
	write(1,&c,1);
}

get2c(f) FILE *f; {
	register c;

	c = getc(f);
	return((getc(f) << 8) | c);
}

fatal(s,a) {

	fprintf(stderr,"%s: ",progname);
	fprintf(stderr,s,a);
	fprintf(stderr,"\n");
	exit(-1);
}
