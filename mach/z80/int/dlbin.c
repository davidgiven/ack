/*
 * Download Z80 load module into the RC702
 *
 * Adapted (untested) to new ack.out format by
 * Ceriel Jacobs, Vrije Universiteit, Amsterdam
 */
#include	<stdio.h>
#include	<assert.h>
#include	<out.h>

int	disp = 0;

char	hex[] = "0123456789ABCDEF";

#define	DATTYPE		0
#define	EOFTYPE		1
#define	SEGTYPE		2
#define	PCTYPE		3

#define	MAXBYTE		32

char *progname;

struct outhead ohead;
struct outsect sect[MAXSECT];

long pc;

main(argc,argv) char **argv; {
	register unsigned nd;
	register char *s;
	int first = 1;
	int i;

	progname = argv[0];
	while (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
		case 'd':
			/* displacement at load time */
			disp = atoi(&argv[1][2]);
			break;
		}
		argc--;
		argv++;
	}
	s = "a.out";
	if (argc == 2)
		s = argv[1];
	else if (argc != 1) {
		fprintf(stderr,"usage: %s [flags] [object file]\n",progname);
		exit(-1);
	}
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
	for (i = 0; i < ohead.oh_nsect; i++) {
		rd_outsect(i);
		pc = sect[i].os_base;
		if (first) {
			first = 0;
			putchar('L');
			putchar('S');
		}
		segment(i);
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
			}
			if (sz > 0) {
				data(sz, (int) pc, buf);
			}
			free(pbuf);
		}
	}
	if (first == 0) eof();
	exit(0);
}

segment(sg) {

	newline(2,0,SEGTYPE);
	word(sg);
	endline();
}

data(nd,pc,buf)
	register char *buf;
	int pc;
{
	register i;

	newline(nd, pc+disp, DATTYPE);
	for (i = 0; i < nd; i++) {
		byte(*buf++);
	}
	endline();
}

int check, bytecount;

newline(n,pc,typ) {

	check = 0;
	bytecount = n+5;
	putchar('\n');	/* added instruction */
	putchar(':');
	byte(n);
	word(pc);
	byte(typ);
}

endline() {

	byte(-check);
	assert(bytecount == 0);
}

word(w) {

	byte(w>>8);
	byte(w);
}

byte(b) {

	check += b;
	bytecount--;
	putchar(hex[(b>>4) & 017]);
	putchar(hex[b & 017]);
}

rd_fatal()
{
	fprintf(stderr, "%s: Read error\n", progname);
	exit(-1);
}

eof() {

	newline(0,0,EOFTYPE);
	byte(0xFF);
	putchar('\n');
}
