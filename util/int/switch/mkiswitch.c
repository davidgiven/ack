/*
	Generates contents of opcode switch from ip_spec.t,
	and parameter descriptions of em_flag.h.

	Call is:
		mkiswitch prefix ip_spec.t cases 

*/

/* $Header$ */

#include <stdio.h>

extern char *sprintf();
extern FILE *popen();

#include <ip_spec.h>
#include <em_spec.h>
#include <em_flag.h>

extern char em_mnem[][4] ;
extern char em_flag[] ;

char *progname;

FILE	*ifp;			/* Input File Pointer */
FILE	*ofp;			/* Output File Pointer */
char	*Prefix;		/* Prefix for function name */

main(argc, argv)
	int argc;
	char **argv;
{
	char	mnem[8];		/* Mnemonic */
	char	flgs[8];		/* Flags */
	char	argstr[256];		/* To build argument */

	progname = argv[0];

	if (argc != 4 ) {
		fatal("usage is: %s prefix ip_spec.t cases\n",
			argv[0]);
	}

	Prefix = argv[1];

	if ((ifp = fopen(argv[2], "r")) == 0) {
		fatal("cannot open '%s' for reading\n", argv[2]);
	}

	if ((ofp = fopen(argv[3], "w")) == 0) {
		fatal("cannot open '%s' for writing\n", argv[3]);
	}

	/* Start reading the input file */
	while (fscanf(ifp, "%s %s", mnem, flgs) >= 0) {
		int i;
		char *p;
		char *base;
		int cnt, first;
		int mnemcode;

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

		mnemcode = getmnem(mnem);


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
		if (in(flgs, '-')) {
			if ((em_flag[mnemcode] & EM_PAR) == PAR_W) {
				/* Implicit argument on stack */
				ImplicitArg(argstr);
				OutCase(mnem, base, first, 0, argstr);
			} else {
				/* No arguments */
				NoArgs(argstr);
				OutCase(mnem, base, first, 0, argstr);
			}
		} else if (in(flgs, 'm')) {	/* Mini */
			for (i = 0; i<cnt; i++) {
				Mini(argstr, i, flgs);
				OutCase(mnem, base, first, i, argstr);
			}
		} else if (in(flgs, 's')) {	/* Shortie */
			for (i = 0; i<cnt; i++) {
				Shortie(argstr, i, flgs);
				OutCase(mnem, base, first, i, argstr);
			}
		} else if (in(flgs, '2')) {	/* Two byte signed */
			TwoSgn(argstr, flgs);
			OutCase(mnem, base, first, 0, argstr);
		} else if (in(flgs, 'u')) {	/* Two byte unsigned */
			TwoUns(argstr, flgs);
			OutCase(mnem, base, first, 0, argstr);
		} else if (in(flgs, '4')) {	/* Four byte signed */
			FourSgn(argstr, flgs);
			OutCase(mnem, base, first, 0, argstr);
		} else {
			fatal("no opcode flag in ip_spec %s\n", flgs);
		}
	}
	exit(0);
}


OutCase(mnem, base, first, i, argstr)
	char *mnem;
	char *base;
	int first;
	int i;
	char *argstr;
{
	/* Output a case in the switch statement */
	fprintf(ofp, "\t\tcase %s+%d:\t%s%s(%s); break;\n",
		base, first+i, Prefix, mnem, argstr);
}



		
ImplicitArg(argstr)
	char *argstr;
{
	sprintf(argstr, "uwpop()");
}

NoArgs(argstr)
	char *argstr;
{
	sprintf(argstr, "");
}

Mini(argstr, i, flgs)
	char *argstr;
	int i;
	char *flgs;
{
	int newi = in(flgs, 'N') ? (-i-1) : in(flgs, 'o') ? (i+1) : i;

	switch (newi) {
	case -1:
		sprintf(argstr, "%s",
			in(flgs, 'w') ? "-wsize" : "-1L");
		break;
	case 0:
		sprintf(argstr, "0L");
		break;
	case 1:
		sprintf(argstr, "%s",
			in(flgs, 'w') ? "wsize" : "1L");
		break;
	default:
		sprintf(argstr, "%dL%s",
			newi, in(flgs, 'w') ? "*wsize" : "");
		break;
	}
}

Shortie(argstr, i, flgs)
	char *argstr;
	int i;
	char *flgs;
{
	int newi = in(flgs, 'N') ? (-i-1) : in(flgs, 'o') ? (i+1) : i;

	sprintf(argstr, "S_arg(%d)*%s",
		newi, in(flgs, 'w') ? "wsize" : "1L");
}

TwoSgn(argstr, flgs)
	char *argstr;
	char *flgs;
{

	sprintf(argstr, "%s*%s", in(flgs, 'P') ? "P_arg_2()" : in(flgs, 'N') ? "N_arg_2()" : "L_arg_2()", 
			in(flgs, 'w') ? "wsize" : "1L");
}

TwoUns(argstr, flgs)
	char *argstr;
	char *flgs;
{

	sprintf(argstr,	"%s*%s", "U_arg()", 
			in(flgs, 'w') ? "wsize" : "((unsigned long) 1)");
}

FourSgn(argstr, flgs)
	char *argstr;
	char *flgs;
{

	sprintf(argstr,	"%s*%s", in(flgs, 'P') ? "P_arg_4()" : in(flgs, 'N') ? "N_arg_4()" : "L_arg_4()",
			in(flgs, 'w') ? "wsize" : "1L");
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
int getmnem(str) char *str ; {
	char (*ptr)[4] ;

	for ( ptr = em_mnem ; *ptr<= &em_mnem[sp_lmnem][0] ; ptr++ ) {
		if ( strcmp(*ptr,str)==0 ) return (ptr-em_mnem) ;
	}
	fatal("Illegal mnemonic") ;
	return 0 ;
}

