/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * main.c
 * Contains main program, and some error message routines
 */

# include "types.h"
# include "io.h"
# include "extern.h"
# include "sets.h"
# include "assert.h"

static string rcsid = "$Header$";

static string	rec_file;
static string	incl_file;

/* In this file the following routines are defined: */
extern int	main();
STATIC		readgrammar();
extern		error();
extern		fatal();
extern		comfatal();
extern		copyfile();
extern		install();
# ifndef NDEBUG
extern		badassertion();
# endif not NDEBUG

main(argc,argv) register string	argv[]; {
	register string arg;
	string libpath();
	int nflag = 0;

	/* Initialize */

	maxorder = order;
	assval = 0400;
	/* read options */
	
	while (argc >= 2 && (arg = argv[1], *arg == '-')) {
		while (*++arg) {
			switch(*arg) {
			  case 'v':
			  case 'V':
				verbose++;
				continue;
			  case 'n':
			  case 'N':
				nflag++;
				continue;
			  case 'f':
			  case 'F':
			  	fflag++;
				continue;
# ifndef NDEBUG
			  case 'a':
			  case 'A':
				debug++;
				continue;
# endif not NDEBUG
			  case 'r':
			  case 'R':
				if (rec_file) {
					fprintf(stderr,"duplicate -r flag\n");
					exit(1);
				}
			  	rec_file = ++arg;
				break;
			  case 'i':
			  case 'I':
				if (incl_file) {
					fprintf(stderr,"duplicate -i flag\n");
					exit(1);
				}
			  	incl_file = ++arg;
				break;
			  case 'x':
			  case 'X':
				ntneeded = 1;
				ntprint = 1;
				continue;
			  default:
				fprintf(stderr,"illegal option : %c\n",*arg);
				return 1;
			}
			break;
		}
		argv++;
		argc--;
	}
	/*
	 * Now check wether the sets should include nonterminals
	 */
	if (verbose == 2) ntneeded = 1;
	else if (! verbose) ntneeded = 0;
	/*
	 * Initialise
	 */
	 if (!rec_file) rec_file = libpath("rec");
	 if (!incl_file) incl_file = libpath("incl");
	if ((fact = fopen(f_temp,"w")) == NULL) {
		fputs("Cannot create temporary\n",stderr);
		return 1;
	}
	name_init();
	readgrammar(argc,argv);
	if (nflag) comfatal();
	setinit(ntneeded);
	maxnt = &nonterms[nnonterms];
	max_t_ent = &h_entry[nterminals];
	fclose(fact);
	/*
	 * Now, the grammar is read. Do some computations
	 */
	co_reach();		/* Check for undefined and unreachable */
	if (nerrors) comfatal();
	createsets();
	co_empty();		/* Which nonterminals produce empty? */
	co_first();		/* Computes first sets */
	co_follow();		/* Computes follow sets */
	co_symb();		/* Computes choice sets in alternations */
	conflchecks();		/* Checks for conflicts etc, and also
				 * takes care of LL.output etc
				 */
	if (nerrors) comfatal();
	co_contains();		/* Computes the contains sets */
	co_safes();		/* Computes safe terms and nonterminals.
				 * Safe means : always called with a terminal
				 * symbol that is guarantied to be eaten by
				 * the term
				 */
	if (argc-- == 1) {
		fputs("No code generation for input from standard input\n",stderr);
	} else	gencode(argc);
	UNLINK(f_temp);
	UNLINK(f_pars);
	return 0;
}

STATIC
readgrammar(argc,argv) char *argv[]; {
	/*
	 * Do just what the name suggests : read the grammar
	 */
	register p_file	p;
	p_mem		alloc();

	linecount = 0;
	f_input = "no filename";
	/*
	 * Build the file structure
	 */
	files = p = (p_file) alloc((unsigned) (argc+1) * sizeof(t_file));
	if (argc-- == 1) {
		finput = stdin;
		p->f_name = f_input = "standard input";
		p->f_firsts = 0;
		p->f_start = maxorder;
		pfile = p;
		LLparse();
		p->f_end = maxorder - 1;		
		p++;
	} else {
		while (argc--) {
			if ((finput = fopen(f_input=argv[1],"r")) == NULL) {
				fatal(0,e_noopen,f_input);
			}
			linecount = 0;
			p->f_name = f_input;
			p->f_start = maxorder;
			p->f_firsts = 0;
			pfile = p;
			LLparse();
			p->f_end = maxorder-1;
			p++;
			argv++;
			fclose(finput);
		}
	}
	p->f_start = maxorder+1;
	p->f_end = maxorder;
	if (! lexical) lexical = "yylex";
	/*
	 * There must be a start symbol!
	 */
	if (start == 0) {
		fatal(linecount,"Missing %%start");
	}
	if (nerrors) comfatal();
}

/* VARARGS1 */
error(lineno,s,t,u) string	s,t,u; {	
	/*
	 * Just an error message
	 */
	register FILE *f;

	f = stderr;
	++nerrors;
	if (lineno) fprintf(f,"\"%s\", line %d : ",f_input,lineno);
	else fprintf(f,"\"%s\" : ",f_input);
	fprintf(f,s,t,u);
	putc('\n',f);
}

/* VARARGS1 */
fatal(lineno,s,t,u) string	s,t,u; {
	/*
	 * Fatal error
	 */
	error(lineno,s,t,u);
	comfatal();
}

comfatal() {
	/*
	 * Some common code for exit on errors
	 */
	if (fact != NULL) {
		fclose(fact);
		UNLINK(f_temp);
	}
	if (fpars != NULL) fclose(fpars);
	UNLINK(f_pars);
	exit(1);
}

copyfile(n) {
	/*
	 * Copies a file indicated by the parameter to filedescriptor fpars.
	 * If n != 0, the error recovery routines are copied,
	 * otherwise a standard header is.
	 */
	register	c;
	register FILE	*f;

	if ((f = fopen(n?rec_file:incl_file,"r")) == NULL) {
		fatal(0,"Cannot open libraryfile, call an expert");
	}
	while ((c = getc(f)) != EOF) putc(c,fpars);
}

install(target, source) string target, source; {
	/*
	 * Copy the temporary file generated from source to target
	 * if allowed (which means that the target must be generated
	 * by LLgen from the source, or that the target is not present
	 */
	register	c;
	register FILE	*f1;
	register FILE	*f2;
	register string	s1;
	register int	i;
	char		buf[100];

	/*
	 * First open temporary, generated for source
	 */
	if ((f1 = fopen(f_pars,"r")) == NULL) {
		fatal(0,e_noopen,f_pars);
	}
	i = 0;
	/*
	 * Now open target for reading
	 */
	if ((f2 = fopen(target,"r")) == NULL) {
		i = 1;
		fclose(f1);
	}
	else {
		/*
		 * Create string recognised by LLgen. The target must
		 * start with that!
		 */
		(int) sprintf(buf,LLgenid,source ? source : ".");
		s1 = buf;
		while (*s1 != '\0' && *s1++ == getc(f2)) { /* nothing */ }
		/*
		 * Ai,ai, it did not
		 */
		if (*s1 != '\0') {
			fatal(0,"%s : not a file generated by LLgen",target);
		}
		rewind(f2);
		/*
		 * Now compare the target with the temporary
		 */
		while ((c = getc(f1)) != EOF && c == getc(f2)) { /* nothing */}
		if (c != EOF || getc(f2) != EOF) i = 1;
		fclose(f1);
		fclose(f2);
	}
	/*
	 * Here, if i != 0 the target must be recreated
	 */
	if (i) RENAME(f_pars,target);
}

#ifndef NDEBUG
badassertion(asstr,file,line) char *asstr, *file; {

	fprintf(stderr,"Assertion \"%s\" failed %s(%d)\n",asstr,file,line);
	if (fact != NULL) fclose(fact);
	if (fpars != NULL) fclose(fpars);
	abort();
}
#endif
