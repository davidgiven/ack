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
 * gencode.c
 * Defines the routine "gencode", which generates the parser
 * we wanted so badly.
 * This file is a mess, it should be cleaned up some time.
 */

# include "types.h"
# include "io.h"
# include "tunable.h"
# include "extern.h"
# include "sets.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid3 = "$Header$";
# endif NORCSID

/*
 * Some codestrings used more than once
 */

static string	c_arrend =	"0 };\n";
static string	c_close =	"}\n";
static string	c_LLptrmin =	"LLptr++;\n";
static string	c_break =	"break;\n";
static string	c_read =	"LLread();\n";

/* Some constants used for reading from the action file */
# define ENDDECL	0400
# define IDENT		0401

static int nlabel;		/* count for the generation of labels */
static int nvar;		/* count for generation of variables */
static int pushlist[100];
static int *ppushlist;
static int *lists,*maxlists,*plists;
p_mem ralloc(),alloc();

/* In this file the following routines are defined: */
extern		gencode();
STATIC		opentemp();
STATIC		geninclude();
STATIC		genrecovery();
STATIC string	genname();
STATIC		generate();
STATIC		prset();
STATIC		macro();
STATIC		controlline();
STATIC		getparams();
STATIC		gettok();
STATIC		rulecode();
STATIC int	dopush();
STATIC		pushcode();
STATIC		getaction();
STATIC		codeforterm();
STATIC		genifhead();
STATIC		gencases();
STATIC		genpush();

/* Macro to print a terminal */
# define PTERM(f,p)	fprintf(f,(p)->h_num<0400?"'%s'":"%s",(p)->h_name)

gencode(argc) {
	register p_file p = files;
	
	/* Generate include file Lpars.h */
	geninclude();

	/* Set up for code generation */
	if ((fact = fopen(f_temp,"r")) == NULL) {
		fatal(0,e_noopen,f_temp);
	}
	lists = (int *) alloc(50 * sizeof(int));
	plists = lists;
	maxlists = lists+49;

	/* For every source file .... */
	while (argc--) {
		/* Open temporary */
		f_input = p->f_name;
		opentemp(f_input);
		/* generate code ... */
		copyfile(0);
		generate(p);
		getaction(2);
		if (ferror(fpars) != 0) {
			fatal(0,"Write error on temporary");
		}
		fclose(fpars);
		/* And install */
		install(genname(p->f_name),p->f_name);
		p++;
	}
	genrecovery();
}

STATIC
opentemp(str) string str; {

	if ((fpars = fopen(f_pars,"w")) == NULL) {
		fatal(0,e_noopen,f_pars);
	}
	fprintf(fpars,LLgenid,str ? str : ".");
}

STATIC
geninclude() {
	register p_entry p;
	register FILE *f;

	opentemp((string) 0);
	f = fpars;
	for (p = h_entry; p < max_t_ent; p++) {
		if (p->h_num >= 0400) {
			fprintf(f,"# define %s %d\n", p->h_name,p->h_num);
		}
	}
	if (ferror(f) != 0) {
		fatal(0,"write error on temporary");
	}
	fclose(f);
	install(HFILE, (string) 0);
}

STATIC
genrecovery() {
	register FILE 	*f;
	register p_entry t;
	register int	i;
	register int	*q;
	int		index[256+NTERMINALS];
	register p_start st;

	opentemp((string) 0);
	f = fpars;
	copyfile(0);
	/* Now generate the routines that call the startsymbols */
	fputs("#define LLSTSIZ 1024\n",f);
	for (i = 1, st = start; st; st = st->ff_next) {
		i++;
		fputs(st->ff_name, f);
		fputs("() {\n\tint LLstack[LLSTSIZ];\n\tLLnewlevel(LLstack);\n\tLLread();\n", f);
		if (g_gettype(st->ff_nont->n_rule) == ALTERNATION) {
			fprintf(f, "\tLLxx.LLx_p--; *LLxx.LLx_p = %d;\n",
			findindex(&(st->ff_nont->n_contains)));
		}
		fprintf(f, "\tL%d_%s();\n",
			st->ff_nont-nonterms,
			(min_nt_ent+(st->ff_nont-nonterms))->h_name);
		if (getntout(st->ff_nont) == NOSCANDONE) {
			fputs("\tLLscan(EOFILE);\n",f);
		}
		else	fputs("\tif (LLsymb != EOFILE) LLerror(EOFILE);\n",f);
		fputs("\tLLoldlevel();\n}\n",f);
	}
	fprintf(f,"#define LL_MAX %d\n#define LL_LEXI %s\n", i, lexical);
	fputs("static short LLlists[] = {\n", f);
	/* Now generate lists */
	q = lists;
	while (q < plists) {
		fprintf(f,"%d,\n",*q++);
	}
	fputs(c_arrend, f);
	/* Now generate the sets */
	fputs("char LLsets[] = {\n",f);
	for (i = 0; i < maxptr-setptr; i++) prset(setptr[i]);
	fputs(c_arrend, f);
	for (q = index; q <= &index[255 + NTERMINALS];) *q++ = -1;
	for (t = h_entry; t < max_t_ent; t++) {
		index[t->h_num] = t - h_entry;
	}
	fputs("short LLindex[] = {\n",f);
	for (q = index; q <= &index[assval-1]; q++) {
		fprintf(f, "%d,\n", *q);
	}
	fputs(c_arrend, f);
	copyfile(1);
	if (ferror(f) != 0) {
		fatal(0,"write error on temporary");
	}
	fclose(f);
	install(RFILE, (string) 0);
}

STATIC
generate(f) p_file f; {
	/*
	 * Generates a parsing routine for every nonterminal
	 */
	register short *s;
	register p_nont	p;
	register FILE *fd;
	int i;
	p_first		ff;
	int mustpop;

	/* Generate first sets */
	for (ff = f->f_firsts; ff; ff = ff->ff_next) {
		macro(ff->ff_name,ff->ff_nont);
	}
	
	/* For every nonterminal generate a function */
	fd = fpars;
	for (s = f->f_start; s <= f->f_end; s++) {
		p = &nonterms[*s];
		/* Generate functions in the order in which the nonterminals
		 * were defined in the grammar. This is important, because
		 * of synchronisation with the action file
		 */
		while (p->n_count--) getaction(1);
		if (p->n_flags & PARAMS) controlline();
		fprintf(fd,"L%d_%s (",*s,(min_nt_ent + *s)->h_name);
		if (p->n_flags & PARAMS) getparams();
		else fputs(") {\n", fd);
		fputs("register struct LLxx *LLx = &LLxx;\n#ifdef lint\nLLx=LLx;\n#endif\n", fd);
		if (p->n_flags & LOCALS) getaction(1);
		i = getntsafe(p);
		mustpop = 0;
		if (g_gettype(p->n_rule) == ALTERNATION) {
			mustpop = 1;
			if (i == NOSCANDONE) {
				fputs(c_read, fd);
				i = SCANDONE;
			}
		}
		nlabel = 1;
		rulecode(p->n_rule,
			 i,
			 getntout(p) != NOSCANDONE,
			 mustpop);
		fputs(c_close, fd);
	}
}

STATIC
prset(p) p_set p; {
	register int k;
	register unsigned i;
	int j;

	j = NBYTES(nterminals);
	for (;;) {
		i = (unsigned) *p++;
		for (k = 0; k < sizeof(int); k++) {
			fprintf(fpars,"0%o,",(i & 0377));
			i >>= 8;
			if (--j == 0) {
				fputs("\n",fpars);
				return;
			}
		}
	}
	/* NOTREACHED */
}

STATIC
macro(s,n) string s; p_nont n; {
	register FILE *f;
	int i;

	f = fpars;
	i = findindex(&(n->n_first));
	fprintf(f,"#define %s(x) ", s);
	if (i < 0) {
		fprintf(f, "((x) == %d)\n", -i);
		return;
	}
	fprintf(f,"LLfirst((x), %d)\n", i);
}

STATIC
controlline() {
	/* Copy a compiler control line */
	register int l;
	register FILE *f1,*f2;

	f1 = fact; f2 = fpars;
	l = getc(f1);
	assert(l == '\0');
	l = getc(f1); putc(l,f2);
	assert( l == '#' ) ;
	do {
		l = getc(f1);
		putc(l,f2);
	} while ( l != '\n' ) ;
}

STATIC
getparams() {
	/* getparams is called if a nonterminal has parameters. The names
	 * of the parameters have to be found, and they should be declared
 	 */
	long off;
	register int l;
	register FILE *f;
	long ftell();
	char first;

	first = ' ';
	f = fpars;
	/* save offset in file to be able to copy the declaration later */
	off = ftell(fact);
	/* First pass through declaration, find the parameter names */
	while ((l = gettok()) != ENDDECL) {
		if (l == ';' || l == ',') {
			/*
			 * The last identifier found before a ';' or a ','
			 * must be a parameter
			 */
			fprintf(f,"%c%s", first, ltext);
			first = ',';
		}
	}
	fputs(") ",f);
	/*
	 * Now copy the declarations 
	 */
	fseek(fact,off,0);
	getaction(0);
	fputs(" {\n",f);
}

STATIC
gettok() {
	/* Read from the action file. */
	register int ch;
	register string	c;
	register FILE *f;

	f = fact;
	ch = getc(f);
	switch(ch) {
		case '\n':
			ch = getc(f);
			if (ch != EOF) {
				ungetc(ch,f);
				if (ch != '\0') return '\n';
			}
			return ENDDECL;
		case '\0':
			ungetc(ch,f);
			/* Fall through */
		case EOF :
			return ENDDECL;
		default :
			if (isalpha(ch) || ch == '_') {
				c = ltext;
				while (isalnum(ch) || ch == '_') {
					*c++ = ch;
					if (c-ltext >= LTEXTSZ) --c;
					ch = getc(f);
				}
				if (ch != EOF) ungetc(ch,f);
				*c = '\0';
				return IDENT;
			}
			return ch;
	}
}

STATIC
rulecode(p,safety,mustscan,mustpop) register p_gram p; {
	/*
	 * Code for a production rule.
	 */

	register int	toplevel = 1;
	register FILE	*f;

	/*
	 * First generate code to push the contains sets of this rule
	 * on a stack
	 */
	ppushlist = pushlist;
	if (dopush(p,safety,1) > 0) pushcode();
	f = fpars;
	for (;;) {
		switch (g_gettype(p)) {
		  case EORULE :
			if (mustscan && safety == NOSCANDONE) {
				fputs(c_read,f);
			}
			return;
		  case TERMINAL : {
			register p_entry t;

			t = &h_entry[g_getcont(p)];
			if (toplevel == 0 && safety != NOSCANDONE) {
				fputs(c_LLptrmin,f);
			}
			if (safety == SAFE) {
				fputs("LL_SAFE(",f);
			}
			else if (safety <= SCANDONE) {
				fputs("LL_SCANDONE(",f);
			}
			else if (safety == NOSCANDONE) {
				if (toplevel != 0) {
					fputs("LL_T_NOSCANDONE(", f);
				}
				else	fputs("LL_N_NOSCANDONE(", f);
			}
			PTERM(f,t);
			fputs(");\n", f);
			if (safety == SAFE && toplevel > 0) {
				safety = NOSCANDONE;
				toplevel = -1;
				p++;
				continue;
			}
			safety = NOSCANDONE;
			break; }
		  case NONTERM : {
			p_entry t;
			register p_nont n;
			int params;

			n = &nonterms[g_getnont(p)];
			t= min_nt_ent+(n-nonterms);
			if (safety == NOSCANDONE &&
			    getntsafe(n) < NOSCANDONE) fputs(c_read, f);
			if (toplevel == 0 &&
				   g_gettype(n->n_rule) != ALTERNATION) {
				fputs(c_LLptrmin, f);
			}
			params = g_getnpar(p);
			if (params) controlline();
			fprintf(f,"L%d_%s(",n-nonterms, t->h_name);
			if (params) getaction(0);
			fputs(");\n",f);
			safety = getntout(n);
			break; }
		  case TERM :
			safety = codeforterm((p_term) pentry[g_getcont(p)],
					     	safety,
					     	toplevel);
			break;
		  case ACTION :
			getaction(1);
			p++;
			continue;
		  case ALTERNATION :
			alternation(p, safety, mustscan,mustpop, 0);
			return;
		}
		p++;
		toplevel = 0;
	}
}

alternation(p, safety, mustscan, mustpop, lb) register p_gram p; {
	register FILE *f = fpars;
	register p_link	l;
	int		hulp, hulp1,hulp2;
	int		var;
	int		haddefault = 0;
	int		unsafe = 1;
	p_set		set;
	p_set		setalloc();

	assert(safety < NOSCANDONE);
	l = (p_link) pentry[g_getcont(p)];
	hulp = nlabel++;
	hulp1 = nlabel++;
	hulp2 = nlabel++;
	var = nvar++;
	if (!lb) lb = hulp1;
	if (safety <= SAFESCANDONE) unsafe = 0;
	if (unsafe && hulp1 == lb) fprintf(f,"L_%d: ", hulp1);
	else if (mustpop) {
		mustpop = 0;
		fputs(c_LLptrmin, f);
	}
	if (unsafe) {
		fprintf(f,"{ int LL_%d = 0;\n", var);
	}
	fputs("switch(LLcsymb) {\n", f);
	while (g_gettype(p) != EORULE) {
		l = (p_link) pentry[g_getcont(p)];
		if (unsafe && (l->l_flag & DEF)) {
			haddefault = 1;
			fprintf(f,
"default: if (!LL_%d && LLskip()) {LL_%d = 1; goto L_%d;}\ngoto L_%d;\n",
			var, var, lb, hulp2);
		}
		if (l->l_flag & COND) {
			set = setalloc(tsetsize);
			setunion(set, l->l_others, tsetsize);
			setintersect(set, l->l_symbs, tsetsize);
			setminus(l->l_symbs, set, tsetsize);
			setminus(l->l_others, set, tsetsize);
			gencases(set);
			free((p_mem) set);
			controlline();
			fputs("if (!",f);
			getaction(0);
			fprintf(f,") goto L_%d;\n", hulp);
		}
		if (!unsafe && (l->l_flag & DEF)) {
			fputs("default:\n", f);
			haddefault = 1;
		}
		else	gencases(l->l_symbs);
		if (l->l_flag & DEF) {
			if (unsafe) {
				fprintf(f,"L_%d: ;\n", hulp2);
			}
			if (mustpop) fputs(c_LLptrmin, f);
			rulecode(l->l_rule, SAFESCANDONE, mustscan, 0);
		}
		else {
			if (mustpop) fputs(c_LLptrmin, f);
			rulecode(l->l_rule, SAFE, mustscan, 0);
		}
		fputs(c_break,f);
		if (l->l_flag & COND) {
			if (!haddefault) {
				fputs("default:\n", f);
			}
			else {
				gencases(l->l_others);
			    	safety = SAFE;
			}
			fprintf(f,"L_%d : ;\n",hulp);
			p++;
			if (!unsafe && g_gettype(p+1) == EORULE) {
				if (mustpop) fputs(c_LLptrmin, f);
				rulecode(((p_link)pentry[g_getcont(p)])->l_rule,
						safety,mustscan,0);
			}
			else alternation(p,safety,mustscan,mustpop,lb);
			break;
		}
		p++;
	}
	fputs(c_close, f);
	if (unsafe) fputs(c_close, f);
	return;
}

STATIC int
dopush(p,safety,toplevel) register p_gram p; {
	/*
	 * The safety only matters if toplevel != 0
	 */
	register int count = 0;

	for (;;) {
		switch(g_gettype(p)) {
		  case EORULE :
		  case ALTERNATION :
			return count;
		  case TERM : {
			register p_term q;

			q = (p_term) pentry[g_getcont(p)];
			count += dopush(p+1,SCANDONE,0);
			*ppushlist++ = findindex(&(q->t_contains));
			return count+1; }
		  case TERMINAL :
			if (toplevel > 0 && safety == SAFE) {
				count += dopush(p+1,NOSCANDONE,-1);
			}
			else	count += dopush(p+1, NOSCANDONE, 0);
			if (toplevel != 0) {
				return count;
			}
			*ppushlist++ = -h_entry[g_getcont(p)].h_num;
			return count + 1;
		  case NONTERM : {
			register p_nont n;

			n = &nonterms[g_getnont(p)];
			count += dopush(p+1, SCANDONE, 0);
			if (toplevel == 0 ||
			     g_gettype(n->n_rule) == ALTERNATION) {
				*ppushlist++ = findindex(&n->n_contains);
				count++;
			}
			return count; }
		}
		p++;
	}
}

# define max(a,b) ((a) < (b) ? (b) : (a))
STATIC
pushcode() {
	register int i,j,k;
	register int *p = pushlist;

	if ((i = ppushlist - p) == 0) return;
	if (i <= 2) {
		genpush(p[0]);
		if (i == 2) genpush(p[1]);
		return;
	}
	fprintf(fpars,"\LLlpush(%d, %d);\n",plists-lists, i);
	if (maxlists - plists < i) {
		j = plists - lists;
		k = maxlists-lists+max(i+1,50);
		lists = (int *) ralloc( (p_mem)lists,
				(unsigned)(k+1)*sizeof(int));
		plists = lists+j;
		maxlists = lists+k;
	}
	while (i--) {
		*plists++ = *p++;
	}
}

STATIC
getaction(flag) {
	/* Read an action from the action file.
	 * flag = 1 if it is an action,
	 * 0 when reading parameters
	 */
	register int match,ch;
	register FILE *f;
	register int newline;
	int mark = 0;

	if (flag == 1) {
		controlline();
	}
	f = fpars;
	newline = 0;
	for (;;) {
		ch = gettok();
		switch(ch) {
		  case ENDDECL:
			if (flag != 2) break;
			ch = getc(fact);
			assert(ch == '\0');
			fputs("\n",f);
			if (mark) return;
			mark = 1;
			continue;
		  case '\n':
			newline = 1;
			break;
		  case '\'' :
		  case '"' :
			if (newline) {
				newline = 0;
			}
			match = ch;
			putc(ch,f);
			while (ch = getc(fact)) {
				if (ch == match) break;
				if (ch == '\\') {
					putc(ch,f);
					ch = getc(fact);
				}
				putc(ch,f);
			}
			break;
		  case IDENT :
			if (newline) {
				newline = 0;
			}
			fputs(ltext,f);
			continue;
		}
		mark = 0;
		if (ch == ENDDECL) break;
		if (newline && ch != '\n') {
			newline = 0;
		}
		putc(ch,f);
	}
	if (flag) fputs("\n",f);
}

STATIC
codeforterm(q,safety,toplevel) register p_term q; {
	/*
	 * Generate code for a term
	 */
	register FILE	*f;
	register int	i;
	register int	rep;
	int		persistent;

	f = fpars;
	i = r_getnum(&(q->t_reps));
	rep = r_getkind(&(q->t_reps));
	persistent = (q->t_flags & PERSISTENT);
	if (safety == NOSCANDONE && (rep != FIXED || i == 0)) {
		fputs(c_read, f);
		if (rep == FIXED && g_gettype(q->t_rule) != ALTERNATION) {
			fputs(c_LLptrmin, f);
		}
	}
	if (i) {
		/* N > 0, so generate fixed forloop */
		fprintf(f,"{\nregister LL_i = %d;\n",i);
		fputs("for (;;) {\nif (!LL_i--) {\nLLptr++;\n", f);
		fputs("break;\n}\n", f);
		if (rep == FIXED) {
			if (gettout(q) == NOSCANDONE && safety == NOSCANDONE) {
				fputs(c_read,f);
			}
		}
	}
	else if (rep != OPT && rep != FIXED) {
		/* '+' or '*', so generate infinite loop */
		fputs("for (;;) {\n",f);
	}
	if (rep == STAR || rep == OPT) {
		genifhead(q,rep);
	}
	rulecode(q->t_rule,t_safety(rep,i,persistent,safety),
		 rep != FIXED || gettout(q) != NOSCANDONE,
		 rep == FIXED && i == 0 && g_gettype(q->t_rule) == ALTERNATION);
	/* in the case of '+', the if is after the code for the rule */
	if (rep == PLUS) {
		if (!persistent) {
			fprintf(f, "*LLptr = %d;\n", findindex(&(q->t_first)));
		}
		genifhead(q,rep);
	}
	if (rep != OPT && rep != FIXED) fputs("continue;\n", f);
	if (rep != FIXED) {
		fputs(c_close, f); /* Close switch */
		if (rep != OPT) {
			fputs("break;\n", f);
		}
	}
	if (rep != OPT && (rep != FIXED || i > 0)) {
		fputs(c_close, f);	/* Close for */
		if (i > 0) {
			fputs(c_close, f);/* Close Register ... */
		}
	}
	return t_after(rep, i, gettout(q));
}

STATIC
genifhead(q,rep) register p_term q; {
	/*
	 * Generate if statement for term q
	 */
	register FILE	*f;
	p_set		p1;
	p_set		setalloc();
	int		hulp, hulp1;

	f = fpars;
	hulp = nlabel++;
	hulp1 = nlabel++;
	fprintf(f, "L_%d : switch(LLcsymb) {\n", hulp);
	if (q->t_flags & RESOLVER) {
		p1 = setalloc(tsetsize);
		setunion(p1,q->t_first,tsetsize);
		setintersect(p1,q->t_follow,tsetsize);
		/*
		 * p1 now points to a set containing the conflicting
		 * symbols
		 */
		setminus(q->t_first, p1, tsetsize);
		setminus(q->t_follow, p1, tsetsize);
		gencases(p1);
		free((p_mem) p1);
		controlline();
		fputs("if (", f);
		getaction(0);
		fprintf(f, ") goto L_%d;\n", hulp1);
	}
	gencases(q->t_follow);
	fputs("LLptr++; break;\n", f);
	fprintf(f, "default: if (!LLnext()) goto L_%d;\n", hulp);
	gencases(q->t_first);
	if (q->t_flags & RESOLVER) {
		fprintf(f, "L_%d : ;\n", hulp1);
	}
	if (rep == OPT) {
		fputs(c_LLptrmin,f);
	}
}

STATIC
gencases(setp) register p_set setp; {
	/*
	 * setp points to a bitset indicating which cases must
	 * be generated.
	 * YECH, the PCC compiler does not accept many cases without statements
	 * inbetween, so after every case label an empty statement is
	 * generated.
	 * The C-grammar used by PCC is really stupid on this point :
	 * it contains the rule
	 * 	statement : label statement
	 * which is right-recursive, and as is well known, LALR parsers don't
	 * handle these things very good.
	 * The grammar should have been written :
	 * 	labeledstatement : labels statement ;
	 *	labels : labels label | ;
	 */
	register p_entry p;
	register i;

	p = h_entry;
	for (i=0; i < nterminals; i++) {
		if (IN(setp,i)) {
			fprintf(fpars,
				p->h_num<0400 ? "case /* '%s' */ %d : ;\n"
					      : "case /*  %s  */ %d : ;\n",
				p->h_name, i);
		}
		p++;
	}
}

STATIC
genpush(d) {

	fprintf(fpars, "LLptr--;\n*LLptr = %d;\n",d);
}

static char namebuf[20];

STATIC string
genname(s) string s; {
	/*
	 * Generate a target file name from the
	 * source file name s.
	 */
	register string c,d;

	c = namebuf;
	while (*s) {
		if (*s == '/') {
			while (*s == '/') s++;
			if (*s) c = namebuf;
			else break;
		}
		*c++ = *s++;
	}
	for (d = c; --d > namebuf;) if (*d == '.') break;
	if (d == namebuf) d = c;
	if (d >= &namebuf[12]) {
		fatal(0,"%s : filename too long",namebuf);
	}
	*d++ = '.';
	*d++ = 'c';
	*d = '\0';
	return namebuf;
}
