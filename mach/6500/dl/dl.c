/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include	<stdio.h>
#include	<assert.h>
#include	<out.h>

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

struct outhead ohead;
struct outsect sect[MAXSECT];

main(argc,argv) char **argv; {
	int i,nd,pc,first;
	register char *s;


	progname = argv[0];
	if (argc > 3)
		fatal("usage: %s [object [tty]]\n",argv[0]);
	s = "a.out";
	if (argc >= 2)
		s = argv[1];
	if (! rd_open(s)) {
		fprintf(stderr,"%s: can't open %s\n",progname,s);
		exit(-1);
	}
	rd_ohead(&ohead);
	if (ohead.oh_flags & HF_LINK) {
		fprintf(stderr,"%s: %s contains unresolved references\n",progname,s);
		exit(-1);
	}
	rd_sect(sect, ohead.oh_nsect);
	ttyfd = 1;
	first = 1;
	for (i = 0; i < ohead.oh_nsect; i++) {
		rd_outsect(i);
		pc = sect[i].os_base;
		while (sect[i].os_size) {
			unsigned int sz = 8096, fl;
			extern char *calloc();
			register char *buf;
			char *pbuf;

			if (sz > sect[i].os_size) sz = sect[i].os_size;
			sect[i].os_size -= sz;
			pbuf = buf = calloc(sz, 1);
			if (fl = sect[i].os_flen) {
				if (fl > sz) fl = sz;
				sect[i].os_flen -= fl;

				rd_emit(buf, (long) fl);
			}
			while (sz >= MAXBYTE) {
				data(MAXBYTE, (int) pc, buf);
				sz -= MAXBYTE;
				buf += MAXBYTE;
				pc += MAXBYTE;
				first = 0;
			}
			if (sz > 0) {
				data(sz, (int) pc, buf);
				first = 0;
			}
			free(pbuf);
		}
	}
	if (first == 0)
		eof();
	if (echo)
		for (;;)
			reply();
}

data(nd,pc, buf)
	register char *buf;
{

	newline(nd,pc,DATTYPE);
	do
		byte(*buf++);
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

put(c)
	char c;
{

	write(ttyfd,&c,1);
}

reply() {
	register i;
	char c;

	if (echo == 0)
		return;
	i = read(ttyfd,&c,1);
	assert(i > 0);
	write(1,&c,1);
}

fatal(s,a) {

	fprintf(stderr,"%s: ",progname);
	fprintf(stderr,s,a);
	fprintf(stderr,"\n");
	exit(-1);
}

rd_fatal() { fatal("read error"); }
