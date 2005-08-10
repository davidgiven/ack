/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Download Z80 load module into the NASCOM
 *
 * Johan Stevenson, Vrije Universiteit, Amsterdam
 * Adapted (untested) to new ack.out format by
 * Ceriel Jacobs, Vrije Universiteit, Amsterdam
 */
#include	<stdio.h>
#include	<assert.h>
/*#include	<sgtty.h>*/
#include	<termios.h>
#include 	<unistd.h>
#include	<signal.h>
#include	<out.h>

int	check;
int	nascom = 1;
int	nl = '\037';
int	zero = 0;
int	disp = 0;

char	hex[] = "0123456789ABCDEF";

char *progname;

/*struct sgttyb	ttynormal;
struct sgttyb	ttyraw;*/
struct termios	ttynormal;
struct termios	ttyraw;
int		rawmode = 0;

struct outhead ohead;
struct outsect sect[MAXSECT];

stop(code) {
	if (rawmode)
		/*stty(1, &ttynormal);*/
		tcsetattr(1, TCSAFLUSH, &ttynormal);
	exit(code);
}

main(argc,argv) char **argv; {
	register unsigned nd;
	long pc;
	register char *s;
	int i;

	progname = argv[0];
	while (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
		case 'u':
			/* unix output */
			nascom = 0;
			nl = '\n';
			break;
		case 'e':
			/* fill EPROM. make minimal change */
			zero = 0xFF;
			break;
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
		stop(-1);
	}
	if (! rd_open(s)) {
		fprintf(stderr,"%s: can't open %s\n",progname,s);
		stop(-1);
	}
	if (nascom) {
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, stop);
		signal(SIGTERM, stop);
		/*if (gtty(1, &ttynormal) < 0) {*/
		if (tcgetattr(1, &ttynormal) < 0) {
			fprintf(stderr, "no tty\n");
			stop(-1);
		}
		rawmode++;
		ttyraw = ttynormal;
		/*
		ttyraw.sg_flags |= RAW;
		ttyraw.sg_ispeed = B1200;
		ttyraw.sg_ospeed = B1200;
		stty(1, &ttyraw);
		*/
		cfmakeraw(&ttyraw);
		cfsetispeed(&ttyraw, B1200);
		cfsetospeed(&ttyraw, B1200);
		sleep(5);
	}
	rd_ohead(&ohead);
	if (ohead.oh_flags & HF_LINK) {
		fprintf(stderr,"%s: %s contains unresolved references\n",progname,s);
		stop(-1);
	}
	rd_sect(sect, ohead.oh_nsect);
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
			while (sz >= 8) {
				data(8, (int) pc, buf);
				sz -= 8;
				buf += 8;
				pc += 8;
			}
			if (sz > 0) {
				data(sz, (int) pc, buf);
			}
			free(pbuf);
		}
	}
	putchar('.');
	putchar(nl);
	if (nascom)
		sleep(5);
	stop(0);
}

data(nd,pc,buf)
	register char *buf;
	int pc;
{
	register i;

	check = 0;
	pc += disp;
	byte(pc>>8);
	byte(pc);
	for (i = 0; i < nd; i++) {
		putchar(' ');
		byte(*buf++);
	}
	while (i < 8) {
		putchar(' ');
		byte(zero);
		i++;
	}
	putchar(' ');
	byte(check);
	putchar(nl);
}

byte(b) {

	check += b;
	putchar(hex[(b>>4) & 017]);
	putchar(hex[b & 017]);
}

rd_fatal()
{
	fprintf(stderr, "%s: Read error\n", progname);
	stop(-1);
}
