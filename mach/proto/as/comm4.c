/* @(#)comm4.c	1.6 */
/*
 * Micro processor assembler framework written by
 *	Johan Stevenson, Vrije Universiteit, Amsterdam
 * modified by
 *	Johan Stevenson, Han Schaminee and Hans de Vries
 *		Philips S&I, T&M, PMDS, Eindhoven
 */

#include	"comm0.h"
#include	"comm1.h"
#include	"y.tab.h"

extern YYSTYPE	yylval;

/* ========== Machine independent C routines ========== */

stop() {
#if DEBUG < 2
	unlink(temppath);
#ifdef LISTING
	unlink(listpath);
#endif
#endif
	exit(nerrors != 0);
}

main(argc, argv)
char **argv;
{
	register char *p;
	register i;
	static char sigs[] = {
		SIGHUP, SIGINT, SIGQUIT, SIGTERM, 0
	};

	/* this test should be performed by the
	 * preprocessor, but it cannot
	 */
	if ((S_ETC|S_COM|S_VAR|S_DOT) != S_ETC)
		fatal("incorrect type bits");

	progname = *argv++; argc--;
	for (p = sigs; i = *p++; )
		if (signal(i, SIG_IGN) != SIG_IGN)
			signal(i, stop);
	for (i = 0; i < argc; i++) {
		p = argv[i];
		if (*p++ != '-')
			continue;
		switch (*p++) {
		case 'o':
			if (*p != NULL) {
				aoutpath = p;
				break;
			}
			argv[i] = 0;
			if (++i >= argc)
				fatal("-o needs filename");
			aoutpath = argv[i];
			break;
		case 'd':
#ifdef LISTING
			dflag = 0;
			while (*p >= '0' && *p <= '7')
				dflag = (dflag << 3) + *p++ - '0';
			if ((dflag & 0777) == 0)
				dflag |= 0700;
			dflag &= ~4;
#endif
			break;
		case 's':
			sflag = 0;
			while (*p >= '0' && *p <= '7')
				sflag = (sflag << 3) + *p++ - '0';
			break;
		case 'r':
#ifdef RELOCATION
#ifdef ASLD
			rflag = 1;
#endif ASLD
#endif RELOCATION
			break;
		case 'b':
#ifdef THREE_PASS
			bflag = 1;
#endif
			break;
#ifndef ASLD
		case 'u':
		case '\0':
			uflag = 1;
			break;
#endif
		default:
			continue;
		}
		argv[i] = 0;
	}
#ifdef RELOCATION
	if (rflag)
		sflag |= SYM_SCT;
#endif RELOCATION
	pass_1(argc, argv);
#ifdef THREE_PASS
	pass_23(PASS_2);
#endif
	pass_23(PASS_3);
	stop();
}

/* ---------- pass 1: arguments, modules, archives ---------- */

pass_1(argc, argv)
char **argv;
{
	register i;
	register char *p;
	register item_t *ip;
#ifdef ASLD
	char armagic[SZMAGIC];
#else
	register nfile = 0;
#endif

	tempfile = fftemp(temppath, "asTXXXXXX");
#ifdef LISTING
	listmode = dflag;
	if (listmode & 0440)
		listfile = fftemp(listpath, "asLXXXXXX");
#endif
	for (ip = keytab; ip->i_type; ip++)
		item_insert(ip, H_KEY+hash(ip->i_name));
	machstart(PASS_1);
	while (--argc >= 0) {
		p = *argv++;
		if (p == 0)
			continue;
#ifdef ASLD
		if (p[0] == '-' && p[1] == '\0') {
			input = stdin;
			parse("STDIN");
			continue;
		}
#else
		if (nfile != 0)
			fatal("second source file %s", p);
		nfile++;
#endif
		if ((input = fopen(p, "r")) == NULL)
			fatal("can't open %s", p);
#ifdef ASLD
		if (
			fread(armagic, SZMAGIC, 1, input) == 1
			&&
			strncmp(armagic, ARMAGIC, SZMAGIC) == 0
		) {
			archive();
			fclose(input);
			continue;
		}
		rewind(input);
#endif
		parse(p);
		fclose(input);
	}
	commfinish();
	machfinish(PASS_1);
#ifdef ASLD
	if (unresolved) {
		nerrors++;
		fflush(stdout);
		fprintf(stderr, "unresolved references:\n");
		for (i = 0; i < H_SIZE; i++) {
			ip = hashtab[H_GLOBAL+i];
			while (ip != 0) {
				if ((ip->i_type & (S_EXT|S_TYP)) == (S_EXT|S_UND))
					fprintf(stderr, "\t%s\n", ip->i_name);
				ip = ip->i_next;
			}
		}
	}
#else
	if (unresolved)
		outhead.oh_flags |= HF_LINK;
	if (nfile == 0)
		fatal("no source file");
#endif
}

#ifdef ASLD
archive()
{
	register long offset;
	register i;
	register long modsize;
	char modhead[SZMHEAD];

	archmode++;
	offset = SZMAGIC;
	for (;;) {
		if (unresolved == 0)
			break;
		fseek(input, offset, 0);
		if (fread(modhead, SZMHEAD, 1, input) != 1)
			break;
		if (
			strncmp(&modhead[OFF_BEG], STR_BEG, LEN_BEG)
			||
			strncmp(&modhead[OFF_END], STR_END, LEN_END)
		)
			fatal("bad archive");
		offset += SZMHEAD;
		modsize = atol(&modhead[OFF_SIZ]);
		archsize = modsize;
		if (needed()) {
			fseek(input, offset, 0);
			archsize = modsize;
			for (i = 0; i < LEN_NAM; i++)
				if (modhead[OFF_NAM+i] == ' ')
					break;
			modhead[OFF_NAM+i] = '\0';
			parse(remember(&modhead[OFF_NAM]));
		}
		offset += modsize;
	}
	archmode = 0;
}

needed()
{
	register c, first;
	register item_t *ip;
	register need;

#ifdef LISTING
	register save;

	save = listflag; listflag = 0;
#endif
	need = 0;
	peekc = -1;
	first = 1;
	for (;;) {
		c = nextchar();
		if (c == '\n') {
			first = 1;
			continue;
		}
		if (c == ' ' || c == '\t' || c == ',')
			continue;
		if (ISALPHA(c) == 0)
			break;
		if ((ip = item_search(readident(c))) == 0) {
			if (first)
				break;
			continue;
		}
		if (first) {
			if (ip != &keytab[KEYDEFINE])
				break;
			first = 0;
		}
		if ((ip->i_type & S_TYP) == S_UND) {
			need++;
			break;
		}
	}
#ifdef LISTING
	listflag = save;
#endif
	return(need);
}
#endif ASLD

parse(s)
char *s;
{
	register i;
	register item_t *ip;
	register char *p;

	for (p = s; *p; )
		if (*p++ == '/')
			s = p;
#ifdef ASLD
	yylval.y_strp = s;
	putval(MODULE);
#endif
	for (i = 0; i < FB_SIZE; i++)
		fb_ptr[FB_BACK+i] = 0;
	newmodule(s);
	peekc = -1;
	yyparse();
	/*
	 * Check for undefined symbols
	 */
#ifdef ASLD
	for (i = 0; i < H_SIZE; i++) {
		while (ip = hashtab[H_LOCAL+i]) {
			/*
			 * cleanup local queue
			 */
			hashtab[H_LOCAL+i] = ip->i_next;
			/*
			 * make undefined references extern
			 */
			if ((ip->i_type & (S_VAR|S_TYP)) == S_UND) 
				ip->i_type |= S_EXT;
			/*
			 * relink externals in global queue
			 */
			if (ip->i_type & S_EXT)
				item_insert(ip, H_GLOBAL+i);
		}
	}
#else
	for (i = 0; i < H_SIZE; i++) {
		for (ip = hashtab[H_LOCAL+i]; ip; ip = ip->i_next) {
			if (ip->i_type & S_EXT)
				continue;
			if (ip->i_type != S_UND)
				continue;
			if (uflag == 0)
				serror("undefined symbol %s", ip->i_name);
			ip->i_type |= S_EXT;
		}
	}
#endif
	/*
	 * Check for undefined numeric labels
	 */
	for (i = 0; i < FB_SIZE; i++) {
		if ((ip = fb_ptr[FB_FORW+i]) == 0)
			continue;
		serror("undefined label %d", i);
		fb_ptr[FB_FORW+i] = 0;
	}
}

pass_23(n)
{
	register i;
#ifdef ASLD
	register addr_t base = 0;
#endif
	register sect_t *sp;

	if (nerrors)
		stop();
	pass = n;
#ifdef LISTING
	listmode >>= 3;
	if (listmode & 4)
		ffreopen(listpath, listfile);
	listeoln = 1;
#endif
#ifdef THREE_PASS
	nbits = 0;
#endif
	for (i = 0; i < FB_SIZE; i++)
		fb_ptr[FB_FORW+i] = fb_ptr[FB_HEAD+i];
	outhead.oh_nemit = 0;
	for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
#ifdef ASLD
		if (sp->s_flag & BASED) {
			base = sp->s_base;
			if (base % sp->s_lign)
				fatal("base not aligned");
		} else {
			base += (sp->s_lign - 1);
			base -= (base % sp->s_lign);
			sp->s_base = base;
		}
		base += sp->s_size;
		base += sp->s_comm;
#endif
		outhead.oh_nemit += sp->s_size - sp->s_zero;
	}
	if (pass == PASS_3) 
		setupoutput();
	for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
		sp->s_size = 0;
		sp->s_zero = 0;
#ifdef THREE_PASS
		sp->s_gain = 0;
#endif
	}
	machstart(n);
#ifndef ASLD
	newmodule(modulename);
#endif ASLD
	ffreopen(temppath, tempfile);
	yyparse();
	commfinish();
	machfinish(n);
}

newmodule(s)
char *s;
{
	switchsect(S_UND);
	modulename = s;
	lineno = 1;
	if ((sflag & (SYM_EXT|SYM_LOC|SYM_LAB)) && PASS_SYMB)
		newsymb(s, S_MOD, (short)0, (valu_t)0);
#ifdef LISTING
	listtemp = 0;
	if (dflag & 01000)
		listtemp = listmode;
	listflag = listtemp;
#endif
}

setupoutput()
{
	register sect_t *sp;
	register long off;
	struct outsect outsect;

#ifdef AOUTSEEK
#define AOUTseek(p,o)	{aoutseek[p]=o;}
	aoutfile = ffcreat(aoutpath);
#else
#define	AOUTseek(p,o)	{fseek(aoutfile[p],o,0);}
	aoutfile[PARTEMIT]=ffcreat(aoutpath);
#ifdef RELOCATION
	aoutfile[PARTRELO]=ffcreat(aoutpath);
#endif
	aoutfile[PARTNAME]=ffcreat(aoutpath);
	aoutfile[PARTCHAR]=ffcreat(aoutpath);
#endif
	/*
	 * header generation
	 */
	AOUTseek(PARTEMIT, 0);
	putofmt((char *)&outhead, SF_HEAD, PARTEMIT);
	/*
	 * section table generation
	 */
	off = SZ_HEAD;
	off += (long)outhead.oh_nsect * SZ_SECT;
	for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
		sp->s_foff = off;
		outsect.os_base = SETBASE(sp);
		outsect.os_size = sp->s_size + sp->s_comm;
		outsect.os_foff = sp->s_foff;
		outsect.os_flen = sp->s_size - sp->s_zero;
		outsect.os_lign = sp->s_lign;
		off += outsect.os_flen;
		putofmt((char *)&outsect, SF_SECT, PARTEMIT);
	}
#ifdef RELOCATION
	AOUTseek(PARTRELO, off);
	off += (long)outhead.oh_nrelo * SZ_RELO;
#endif
	if (sflag == 0)
		return;
	AOUTseek(PARTNAME, off);
	off += (long)outhead.oh_nname * SZ_NAME;
	AOUTseek(PARTCHAR, off);
	outhead.oh_nchar = off;	/* see newsymb() */
#undef AOUTseek
}

commfinish()
{
	register i;
	register item_t *ip;
	register sect_t *sp;
	register valu_t addr;

	switchsect(S_UND);
#ifdef ASLD
	/*
	 * assign .comm labels and produce .comm symbol table entries
	 */
	for (i = 0; i<H_SIZE; i++)
		for (ip = hashtab[H_GLOBAL+i]; ip; ip = ip->i_next) {
			if ((ip->i_type & S_COM) == 0)
				continue;
			sp = &sect[(ip->i_type & S_TYP) - S_MIN];
			if (pass == PASS_1) {
				addr = sp->s_size + sp->s_comm;
				sp->s_comm += ip->i_valu;
				ip->i_valu = addr;
			}
#ifdef THREE_PASS
			if (pass == PASS_2)
				ip->i_valu -= sp->s_gain;
#endif
			if ((sflag & SYM_EXT) && PASS_SYMB)
				newsymb(
					ip->i_name,
					ip->i_type & (S_EXT|S_TYP),
					(short)0,
					load(ip)
				);
		}
#endif
	if (PASS_SYMB == 0)
		return;
#ifndef ASLD
	/*
	 * produce symbol table entries for undefined's
	 */
	for (i = 0; i<H_SIZE; i++)
		for (ip = hashtab[H_LOCAL+i]; ip; ip = ip->i_next) {
			if (ip->i_type != (S_EXT|S_UND))
				continue;
			if (pass != PASS_3)
				/*
				 * save symbol table index
				 * for possible relocation
				 */
				ip->i_valu = outhead.oh_nname;
			if (sflag & SYM_SCT)
				newsymb(
					ip->i_name,
					S_EXT|S_UND,
					(short)0,
					(valu_t)0
				);
		}
#endif ASLD
	/*
	 * produce symbol table entries for sections
	 */
	if (sflag & SYM_SCT)
		for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
			ip = sp->s_item;
			newsymb(
				ip->i_name,
				(short)(ip->i_type | S_SCT),
				(short)0,
				load(ip)
			);
		}
}
