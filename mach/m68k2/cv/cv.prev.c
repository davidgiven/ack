#include <stdio.h>
#include "out.h"

#define ASSERT(x) switch (2) { case 0: case (x): ; }

/*
 * Header and section table of new format object file.
 */
struct outhead	outhead;
struct outsect	outsect[S_MAX];

char	*output_file;
int	output_file_created;

main(argc, argv)
	int	argc;
	char	*argv[];
{
	register int		nsect;
	register struct outsect	*sectp;
	register FILE		*input;
	register FILE		*output;

	ASSERT(sizeof(struct outhead) == SZ_HEAD);
	ASSERT(sizeof(struct outsect) == SZ_SECT);

	input = stdin; output = stdout;
	switch (argc) {
	case 1:	break;
	case 3:	if ((output = fopen(argv[2], "w")) == (FILE *)0)
			fatal("Can't write %s.\n", argv[2]);
		output_file = argv[2];
		output_file_created = 1;
		/* FALLTHROUGH */
	case 2:	if ((input = fopen(argv[1], "r")) == (FILE *)0)
			fatal("Can't read %s.\n", argv[1]);
		break;
	default:fatal("Usage: %s <as object> <dl object>.\n", argv[0]);
	}
	if (fread((char *)&outhead, SZ_HEAD, 1, input) != 1)
		fatal("Reading header failed.\n");
	if (BADMAGIC(outhead))
		fatal("Not an ack object file.\n");
	if (outhead.oh_nrelo > 0)
		fprintf(stderr, "Warning: relocation information present.\n");
	nsect = outhead.oh_nsect;
	if (fread((char *)outsect, SZ_SECT, nsect, input) != nsect)
		fatal("Reading section table failed.\n");
	sectp = outsect;
	while (nsect--) {
		register long	flen;
#ifdef DO_BSS
		register long	zero;
#endif DO_BSS
		long	base;
		short	cnt;
		char	buffer[BUFSIZ];

		base = sectp->os_base;
		flen = sectp->os_flen;
#ifdef DO_BSS
		zero = sectp->os_size - flen;
#endif DO_BSS

		while (flen) {
			cnt = flen > BUFSIZ ? BUFSIZ : flen;
			if (fread((char *)buffer, 1, cnt, input) != cnt)
				fatal("Reading code bytes failed.\n");
			if (fwrite((char *)&base, 4, 1, output) != 1)
				fatal("Writing start address failed.\n");
			if (fwrite((char *)&cnt, 2, 1, output) != 1)
				fatal("Writing byte count failed.\n");
			if (fwrite((char *)buffer, 1, cnt, output) != cnt)
				fatal("Writing byte count failed.\n");
			base += cnt;
			flen -= cnt;
		}
#ifdef DO_BSS
		while (zero) {
			cnt = zero > BUFSIZ ? BUFSIZ : zero;
			if (fwrite((char *)&base, 4, 1, output) != 1)
				fatal("Writing start address failed.\n");
			if (fwrite((char *)&cnt, 2, 1, output) != 1)
				fatal("Writing byte count failed.\n");
			if (fseek(output, (long)cnt, 1) < (long)0)
				fatal("Fseek failed.\n");
			base += cnt;
			zero -= cnt;
		}
#endif DO_BSS
		sectp++;
	}
	exit(0);
}

/* VARARGS1 */
fatal(s, a1, a2)
	char	*s;
{
	fprintf(stderr, s, a1, a2);
	if (output_file_created)
		unlink(output_file);
	exit(-1);
}
