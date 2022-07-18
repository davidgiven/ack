/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
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

#define	MAXBYTE		32

int	check;
int	echo;
int	istty;
int	bytecount;
int	ttyfd;

char	*progname;

char	hex[] = "0123456789ABCDEF";

main(argc,argv) char **argv; {
	register nd,pc,sg,osg,first;
	register char *s;
	int uid;

	progname = argv[0];
	if (argc > 3)
		fatal("usage: %s [object [tty]]\n",argv[0]);
	s = "a.out";
	if (argc >= 2)
		s = argv[1];
	if (freopen(s,"rb",stdin) == NULL)
		fatal("can't open %s",s);
	s = "/dev/tty05";
	if (argc >= 3)
		s = argv[2];
	if ((ttyfd = open(s,2)) < 0)
		if ((ttyfd = creat(s,0666)) < 0)
			fatal("can't open %s",s);
	if (gtty(ttyfd,&tty) == 0) {
		echo++;
		istty++;
		tty.sg_ispeed = tty.sg_ospeed = B2400;
		tty.sg_flags = RAW;
		stty(ttyfd,&tty);
	} else {
		freopen(s,"wb",stdout);
	}
	first = 1; osg = 0;
	/* uid = getuid(); */
	/* lock(1); */
	for (;;) {
		pc = get2c(stdin);
		if (feof(stdin))
			break;
		sg = get2c(stdin);
		nd = get2c(stdin);
		if (first) {
			put('L'); reply();
			put('S'); reply();
			first = 0;
		}
		if (sg != osg) {
			segment(sg);
			osg = sg;
		}
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
/* 	lock(0); */
/* 	setuid(uid); */
/* 	if (echo) */
/* 		for (;;) */
/* 			reply(); */
}

segment(sg) {

	newline(2,0,SEGTYPE);
	word(sg);
	endline();
}

startad(pc) {

	newline(4,0,PCTYPE);
	word(0);
	word(pc);
	endline();
}

data(nd,pc) {

	newline(nd,pc,DATTYPE);
	do
		byte(getc(stdin));
	while (--nd);
	endline();
}

eof() {

	newline(0,0,EOFTYPE);
	byte(0xFF);
	put('\n');
}

newline(n,pc,typ) {

	check = 0;
	bytecount = n+5;
	put('\n');	/* added instruction */
	put(':');
	byte(n);
	word(pc);
	byte(typ);
}

endline() {

	byte(-check);
	assert(bytecount == 0);
	assert(check == 0);
}

word(w) {

	byte(w>>8);
	byte(w);
}

byte(b) {

	check += b;
	--bytecount;
	put(hex[(b>>4) & 017]);
	put(hex[b & 017]);
}

put(c) {

	if (istty)
		write(ttyfd,&c,1);
	else
		putchar(c);
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
