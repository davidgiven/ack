/*
	Generates contents of opcode switch from ip_spec.t
	Call is:
		mkswitch prefix ip_spec.t cases [ functions ]

	Note:
	The u flag has been implemented by just copying and adjusting the
	code for 2, which is almost identical to that for 4.  When this has
	stabilized, combine.
*/

/* $Id$ */

#include <stdio.h>

extern FILE *popen();

char *progname;

FILE	*ifp;			/* Input File Pointer */
FILE	*ofp;			/* Output File Pointer */
FILE	*ffp = 0;		/* Function File Pointer */
char	*Prefix;		/* Prefix for function name */

main(argc, argv)
	int argc;
	char **argv;
{
	char	mnem[8];		/* Mnemonic */
	char	flgs[8];		/* Flags */
	char	command[32];

	progname = argv[0];

	if (argc < 4 || argc >5) {
		fatal("usage is: %s prefix ip_spec.t cases [ functions ]\n",
			argv[0]);
	}

	Prefix = argv[1];

	if ((ifp = fopen(argv[2], "r")) == 0) {
		fatal("cannot open '%s' for reading\n", argv[2]);
	}

	if ((ofp = fopen(argv[3], "w")) == 0) {
		fatal("cannot open '%s' for writing\n", argv[3]);
	}

	if (argc == 5) {	/* Need to store functions */
		sprintf(command, "sort | uniq > %s", argv[4]);
		if ((ffp = popen(command, "w")) == 0) {
			fatal("cannot popen '%s'\n", command);
		}
	}

	/* Start reading the input file */
	while (fscanf(ifp, "%s %s", mnem, flgs) >= 0) {
		int i;
		char *p;
		char *base;
		int cnt, first;

		/* check flags */
		for (p = flgs; *p; p++) {
			if (!in("-ms2u4eNPwo", *p)) {
				fatal("bad flags ip_spec: %s\n", flgs);
			}
		}

		if (	in(flgs, '-')
		+	in(flgs, 'm')
		+	in(flgs, 's')
		+	in(flgs, '2')
		+	in(flgs, 'u')
		+	in(flgs, '4')
		!=	1
		) {
			fatal("duplicate or missing opcode flag ip_spec: %s\n",
					flgs);
		}

		if (	in(flgs, 'N')
		+	in(flgs, 'P')
		>	1
		) {
			fatal("duplicate restriction flags ip_spec: %s\n", flgs);
		}

		/* capitalize mnemonic */
		for (p = mnem; *p; p++) {
			*p += ('A' - 'a');
		}

		/* scan rest of line */
		if (	in(flgs, '-')
		||	in(flgs, '2')
		||	in(flgs, 'u')
		||	in(flgs, '4')
		) {
			fscanf(ifp, " %d \n", &first);
		}
		else {
			fscanf(ifp, " %d %d \n", &cnt, &first);
		}

		/* determine base */
		if (in(flgs, 'e'))	/* Escaped (secondary) opcode */
			base = "SEC_BASE";
		else
		if (in(flgs, '4'))	/* Escaped (tertiary) opcode */
			base = "TERT_BASE";
		else
			base = "PRIM_BASE";

		/* analyse the opcode */
		if (in(flgs, '-')) {	/* No arguments */
			NoArgs(base, first, mnem);
		} else if (in(flgs, 'm')) {	/* Mini */
			for (i = 0; i<cnt; i++) {
				Mini(i, flgs, base, first, mnem);
			}
		} else if (in(flgs, 's')) {	/* Shortie */
			for (i = 0; i<cnt; i++) {
				Shortie(i, flgs, base, first, mnem);
			}
		} else if (in(flgs, '2')) {	/* Two byte signed */
			TwoSgn(flgs, base, first, mnem);
		} else if (in(flgs, 'u')) {	/* Two byte unsigned */
			TwoUns(flgs, base, first, mnem);
		} else if (in(flgs, '4')) {	/* Four byte signed */
			FourSgn(flgs, base, first, mnem);
		} else {
			fatal("no opcode flag in ip_spec %s\n", flgs);
		}
	}
	exit(0);
}

NoArgs(base, first, mnem)
	char *base;
	int first;
	char *mnem;
{
	fprintf(ofp, "\t\tcase %s+%d:\t%s%sz(); break;\n",
		base, first, Prefix, mnem);
	if (ffp) {
		fprintf(ffp, "%s%sz() {", Prefix, mnem);
		fprintf(ffp, "LOG((\"@ %s%sz()\"));}\n", Prefix, mnem);
	}
}

Mini(i, flgs, base, first, mnem)
	int i;
	char *flgs;
	char *base;
	int first;
	char *mnem;
{
	char arg[16];
	int newi = in(flgs, 'N') ? (-i-1) : in(flgs, 'o') ? (i+1) : i;

	switch (newi) {
	case -1:
		sprintf(arg, "%s",
			in(flgs, 'w') ? "-wsize" : "-1L");
		break;
	case 0:
		sprintf(arg, "0L");
		break;
	case 1:
		sprintf(arg, "%s",
			in(flgs, 'w') ? "wsize" : "1L");
		break;
	default:
		sprintf(arg, "%dL%s",
			newi, in(flgs, 'w') ? "*wsize" : "");
		break;
	}
	fprintf(ofp, "\t\tcase %s+%d:\t%s%sm(%s); break;\n",
		base, first+i, Prefix, mnem, arg);
	if (ffp) {
		fprintf(ffp, "%s%sm(arg) long arg; {",
				Prefix, mnem);
		fprintf(ffp, "LOG((\"@ %s%sm(%%d)\", arg));}\n",
				Prefix, mnem);
	}
}

Shortie(i, flgs, base, first, mnem)
	int i;
	char *flgs;
	char *base;
	int first;
	char *mnem;
{
	char arg[16];
	int newi = in(flgs, 'N') ? (-i-1) : in(flgs, 'o') ? (i+1) : i;

	sprintf(arg, "%dL, %s", newi, (in(flgs, 'w') ? "wsize" : "1L"));
	fprintf(ofp, "\t\tcase %s+%d:\t%s%ss(%s); break;\n",
		base, first+i, Prefix, mnem, arg);
	if (ffp) {
		fprintf(ffp, "%s%ss(hob, wfac) long hob; size wfac; {",
				Prefix, mnem);
		fprintf(ffp, "LOG((\"@ %s%ss(%%d)\", hob, wfac));",
				Prefix, mnem);
		fprintf(ffp, " newPC(PC+1);}\n");
	}
}

TwoSgn(flgs, base, first, mnem)
	char *flgs;
	char *base;
	int first;
	char *mnem;
{
	char *xy = in(flgs, 'P') ? "p2" : in(flgs, 'N') ? "n2" : "l2";

	fprintf(ofp, "\t\tcase %s+%d:\t%s%s%s(%s); break;\n",
			base, first, Prefix, mnem, xy,
			in(flgs, 'w') ? "wsize" : "1L");
	if (ffp) {
		fprintf(ffp, "%s%s%s(arg) long arg; {", Prefix, mnem, xy);
		fprintf(ffp, "LOG((\"@ %s%s%s(%%d)\", arg));",
				Prefix, mnem, xy);
		fprintf(ffp, " newPC(PC+2);}\n");
	}
}

TwoUns(flgs, base, first, mnem)
	char *flgs;
	char *base;
	int first;
	char *mnem;
{
	char *xy = "u";

	fprintf(ofp, "\t\tcase %s+%d:\t%s%s%s(%s); break;\n",
			base, first, Prefix, mnem, xy,
			in(flgs, 'w') ? "wsize" : "1L");
	if (ffp) {
		fprintf(ffp, "%s%s%s(arg) long arg; {", Prefix, mnem, xy);
		fprintf(ffp, "LOG((\"@ %s%s%s(%%d)\", arg));",
				Prefix, mnem, xy);
		fprintf(ffp, " newPC(PC+2);}\n");
	}
}

FourSgn(flgs, base, first, mnem)
	char *flgs;
	char *base;
	int first;
	char *mnem;
{
	char *xy = in(flgs, 'P') ? "p4" : in(flgs, 'N') ? "n4" : "l4";

	fprintf(ofp, "\t\tcase %s+%d:\t%s%s%s(%s); break;\n",
			base, first, Prefix, mnem, xy,
			in(flgs, 'w') ? "wsize" : "1L");
	if (ffp) {
		fprintf(ffp, "%s%s%s(arg) long arg; {", Prefix, mnem, xy);
		fprintf(ffp, "LOG((\"@ %s%s%s(%%d)\", arg));",
				Prefix, mnem, xy);
		fprintf(ffp, " newPC(PC+4);}\n");
	}
}

int
in(flgs, c)
	char *flgs;
	char c;
{
	while (*flgs)
		if (c == *flgs++)
			return 1;
	return 0;
}

fatal(fmt, str)
	char *fmt;
	char *str;
{
	fprintf(stderr, "%s, (fatal error): ", progname);
	fprintf(stderr, fmt, str);
	fprintf(stderr, "\n");
	exit(1);
}
