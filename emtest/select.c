/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include	<stdio.h>
#include	<assert.h>
#include	<signal.h>

#define	LINSIZ	100

int	sigs[] = {
	SIGHUP,
	SIGINT,
	SIGQUIT,
	SIGTERM,
	0
};

char	*prog;
char	line[LINSIZ];
int	nlocals	= 0;
int	nhol	= 0;
int	nerrors	= 0;
int	oknum	= 2;
int	fflag	= 1;
int	low	= 0;
int	high	= 999;

FILE	*file1;
FILE	*file2;
FILE	*file3;
char	name1[] = "/usr/tmp/f1XXXXXX";
char	name2[] = "/usr/tmp/f2XXXXXX";
char	name3[] = "/usr/tmp/f3XXXXXX";

char *to3dig();

stop() {
	unlink(name1);
	unlink(name2);
	unlink(name3);
	exit(nerrors);
}

main(argc,argv) char **argv; {
	register *p;
	register char *s;

	prog = *argv++; --argc;
	mktemp(name1);
	mktemp(name2);
	mktemp(name3);
	for (p = sigs; *p; p++)
		if (signal(*p, stop) == SIG_IGN)
			signal(*p, SIG_IGN);
	while (argc > 0 && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case 'f':
			fflag ^= 1;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			high = atoi(&argv[0][1]);
			break;
		default:
			usage();
			break;
		}
		argc--;
		argv++;
	}
	if (argc > 0 && argv[0][0] >= '0' && argv[0][0] <= '9') {
		s = argv[0];
		do
			low = low*10 + *s++ - '0';
		while (*s >= '0' && *s <= '9');
		if (*s == 0)
			high = low;
		else if (*s++ == '-') {
			high = atoi(s);
			if (high == 0)
				high = 999;
		} else
			fatal("bad range %s", argv[0]);
		argc--;
		argv++;
	}
	if (argc > 1)
		usage();
	if (argc == 1 && freopen(argv[0], "r", stdin) == NULL)
		fatal("cannot open %s", argv[0]);
	if ((file1 = fopen(name1, "w")) == NULL)
		fatal("cannot create %s", name1);
	if ((file2 = fopen(name2, "w")) == NULL)
		fatal("cannot create %s", name2);
	if ((file3 = fopen(name3, "w")) == NULL)
		fatal("cannot create %s", name3);
	if (getline())
		while (select())
			;
	fclose(file1);
	fclose(file2);
	fclose(file3);
	combine();
	stop();
}

select() {
	register FILE *f;
	int i;

	if (sscanf(line, "TEST %d", &i) != 1)
		fatal("bad test identification(%s)", line);
	if (i < low || i > high) {
		while (getline())
			if (line[0] == 'T')
				return(1);
		return(0);
	}
	fprintf(file2, "; %s\n", line);
	if (fflag) {
		char *s = to3dig(i);
		fprintf(file1, ".%s\n", s);
		fprintf(file1, " con \"tst%s\"\n", s);
		fprintf(file2, " fil .%s\n", s);
	}
	f = file1;
	while (getline()) {
		switch (line[0]) {
		case 'T':
			return(1);
		case 'M':
			if (sscanf(line, "MAIN%d", &i) != 1 || i%4 != 0)
				break;
			if (i > nlocals)
				nlocals = i;
			f = file2;
			continue;
		case 'P':
			if (strcmp(line, "PROC") != 0)
				break;
			f = file3;
			continue;
		case 'H':
			if (f != file1 ||
			    sscanf(line, "HOL%d", &i) != 1 ||
			    i%4 != 0)
				break;
			if (i > nhol)
				nhol = i;
			continue;
		case 'O':
			if (strcmp(line, "OK") != 0)
				break;
			fprintf(f, " lin %d\n nop\n", oknum++);
			continue;
		case 'E':
			if (f != file3 || strcmp(line, "ERRLAB") != 0)
				break;
			fprintf(f, "1\n lin 1\n nop\n loc 1\n loc 1\n mon\n");
			continue;
		default:
			putline(f);
			continue;
		}
		fatal("bad line (%s)", line);
	}
	return(0);
}

combine() {

	printf("#define WS EM_WSIZE\n");
	printf("#define PS EM_PSIZE\n");
	printf("#include \"test.h\"\n");
	printf(" mes 2,WS,PS\n");
	printf(" mes 1\n");
	printf(" mes 4,300\n");
	if (nhol)
		printf(" hol %d,0,0\n", nhol);
	copy(name1);
	printf(" exp $_m_a_i_n\n");
	printf(" pro $_m_a_i_n,%d\n", nlocals);
	printf(" loc 123\n");
	printf(" loc -98\n");
	copy(name2);
	printf(" loc -98\n");
	printf(" bne *1\n");
	printf(" loc 123\n");
	printf(" bne *1\n");
	printf(" lin 0\n");
	printf(" nop\n");
	printf(" loc 0\n");
	printf(" ret WS\n");
	printf("1\n");
	printf(" lin 1\n");
	printf(" nop\n");
	printf(" loc 1\n");
	printf(" ret WS\n");
	printf(" end\n");
	copy(name3);
}

copy(s) char *s; {
	if (freopen(s, "r", stdin) == NULL)
		fatal("cannot reopen %s", s);
	while (getline())
		putline(stdout);
}

getline() {
	register len;

	if (fgets(line, LINSIZ, stdin) == NULL)
		return(0);
	len = strlen(line);
	if (line[len-1] != '\n')
		fatal("line too long(%s)", line);
	line[len-1] = 0;
	return(1);
}

putline(f) FILE *f; {
	fprintf(f, "%s\n", line);
}

fatal(s, a1, a2, a3, a4) char *s; {
	fprintf(stderr, "%s: ", prog);
	fprintf(stderr, s, a1, a2, a3, a4);
	fprintf(stderr, " (fatal)\n");
	nerrors++;
	stop();
}

usage() {
	fprintf(stderr, "usage: %s -f [[low]-[high]] [testcollection]\n", prog);
	nerrors++;
	stop();
}

char *
to3dig(i)
	register int i;
{
	static char buf[4];
	register char *s = buf;

	*s++ = (i % 1000) / 100 + '0';
	*s++ = (i % 100) / 10 + '0';
	*s++ = (i % 10) + '0';
	*s = '\0';
	return buf;
}
