/*
 * Download Z80 load module into the NASCOM
 *
 * Johan Stevenson, Vrije Universiteit, Amsterdam
 */
#include	<stdio.h>
#include	<assert.h>
#include	<sgtty.h>
#include	<signal.h>

int	check;
int	nascom = 1;
int	nl = '\037';
int	zero = 0;
int	disp = 0;

char	hex[] = "0123456789ABCDEF";

struct sgttyb	ttynormal;
struct sgttyb	ttyraw;
int		rawmode = 0;

stop(code) {
	if (rawmode)
		stty(1, &ttynormal);
	exit(code);
}

main(argc,argv) char **argv; {
	register unsigned nd,pc;
	register char *s;

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
		fprintf(stderr,"usage: %s [flags] [object file]\n",argv[0]);
		stop(-1);
	}
	if (freopen(s,"r",stdin) == NULL) {
		fprintf(stderr,"%s: can't open %s\n",argv[0],s);
		stop(-1);
	}
	if (nascom) {
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, stop);
		signal(SIGTERM, stop);
		if (gtty(1, &ttynormal) < 0) {
			fprintf(stderr, "no tty\n");
			stop(-1);
		}
		rawmode++;
		ttyraw = ttynormal;
		ttyraw.sg_flags |= RAW;
		ttyraw.sg_ispeed = B1200;
		ttyraw.sg_ospeed = B1200;
		stty(1, &ttyraw);
		sleep(5);
	}
	for (;;) {
		pc = get2c(stdin);
		if (feof(stdin))
			break;
		nd = get2c(stdin);
		nd = get2c(stdin);
		if (nd > 256) {
			fprintf(stderr,"bad format on %s\n",s);
			stop(1);
		}
		while (nd > 8) {
			data(8,pc);
			nd -= 8;
			pc += 8;
		}
		if (nd > 0)
			data(nd,pc);
		assert(feof(stdin) == 0);
	}
	putchar('.');
	putchar(nl);
	if (nascom)
		sleep(5);
	stop(0);
}

data(nd,pc) {
	register i;

	check = 0;
	pc += disp;
	byte(pc>>8);
	byte(pc);
	for (i = 0; i < nd; i++) {
		putchar(' ');
		byte(getc(stdin));
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

get2c(f) FILE *f; {
	register c;

	c = getc(f);
	return((getc(f) << 8) | c);
}
