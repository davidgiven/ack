#include <stdio.h>

main(argc, argv)
	int argc;
	char *argv[];
{
	int nflag = 0;

	if(argc > 1 && ! strncmp(argv[1], "-n", 2)) {
		nflag++;
		argc--;
		argv++;
	}
	while (--argc > 0) {
		fputs(argv[1], stdout);
		argv++;
		if (argc > 1) putchar(' ');
	}
	if (!nflag) putchar('\n');
	exit(0);
}
