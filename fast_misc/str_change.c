/*	Utility to change strings in binary files.
	Reads from standard input, writes on standard output.
	Only replaces one occurrence if the -s flag is given.
*/

#include <stdio.h>

main(argc, argv)
	char *argv[];
{
	register char *psrc;
	register int ch;
	int srclen;
	int sflag = 0;
	char *progname = argv[0];

	while (argc > 1 && argv[1][0] == '-') {
		switch(argv[1][1]) {
		case 's':
			sflag = 1;
			break;
		default:
			fprintf(stderr,
			"Usage: %s [-s] <originalstring> <replacementstring>\n",
			progname);
			exit(1);
		}
		argc--;
		argv++;
	}
	if (argc != 3) {
		fprintf(stderr,
			"Usage: %s [-s] <originalstring> <replacementstring>\n",
			progname);
		exit(1);
	}

	psrc = argv[1];
	srclen = strlen(psrc);
	if (srclen == 0) {
		fprintf(stderr,
			"%s: originalstring must have length > 0\n",
			progname);
		exit(1);
	}
	for (;;) {
		ch = getchar();
		if (ch == EOF) exit(0);
		if (ch != (*psrc & 0377)) {
			putchar(ch);
			continue;
		}
		do {
			psrc++;
			ch = getchar();
		} while (ch && ch == (*psrc & 0377));
		if (ch != EOF) ungetc(ch, stdin);
		if (*psrc == '\0') {
			/* we have a match */
			register int i;
			register char *prepl = argv[2];

			for (i = srclen; i; i--) {
				if (*prepl) {
					putchar(*prepl);
					prepl++;
				}
				else putchar('\0');
			}
			while (*prepl) {
				putchar(*prepl);
				prepl++;
				if (ch != EOF) ch = getchar();
			}
			if (sflag) {
				break;
			}
		}
		else {
			register char *p = argv[1];
			
			while (p < psrc) {
				putchar(*p);
				p++;
			}
			if (ch == EOF) exit(0);
		}
		psrc = argv[1];
	}
	if (ch == EOF) exit(0);
	while ((ch = getchar()) != EOF) {
		putchar(ch);
	}
	exit(0);
}
