/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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
#include	"object.h"

extern YYSTYPE	yylval;

static void pass_1(int, char **);
#ifdef ASLD
static void archive(void);
static int needed(void);
#endif
static void parse(char *);
static void pass_23(int);
static void setupoutput(void);
static void commfinish(void);

/* ========== Machine independent C routines ========== */

void stop(void) {
	exit(nerrors != 0);
}

static void stop_on_signal(int sig) {
	stop();
}

int
main(int argc, char **argv)
{
	char *p;
	int i;

	/* the next test should be performed by the
	 * preprocessor, but it cannot, so it is performed by the compiler.
	 */

	switch(0) {
	case 1:	break;
	case (S_ETC|S_COM|S_VAR|S_DOT) != S_ETC : break;
	}

	progname = *argv++; argc--;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, stop_on_signal);
	for (i = 0; i < argc; i++) {
		p = argv[i];
		if (*p++ != '-')
			continue;
		switch (*p++) {
		case 'o':
			if (*p != '\0') {
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
#endif /* ASLD */
#endif /* RELOCATION */
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
#endif /* RELOCATION */
	pass_1(argc, argv);
#ifdef THREE_PASS
	pass_23(PASS_2);
#endif
	pass_23(PASS_3);
	wr_close();
	stop();
}

/* ---------- pass 1: arguments, modules, archives ---------- */

static void
pass_1(int argc, char **argv)
{
	char *p;
	item_t *ip;
#ifdef ASLD
	char armagic[2];
#else
	int nfile = 0;
#endif

#ifdef THREE_PASS
	bitindex = -1;
	nbits = BITCHUNK;
#endif

	tempfile = tmpfile();
#ifdef LISTING
	listmode = dflag;
	if (listmode & 0440)
		listfile = tmpfile();
#endif
	for (ip = keytab; ip->i_type; ip++)
		item_insert(ip, H_KEY+hash(ip->i_name));
	machstart(PASS_1);
	while (--argc >= 0) {
		p = *argv++;
		if (p == 0)
			continue;
#ifndef ASLD
		if (nfile != 0)
			fatal("second source file %s", p);
		nfile++;
#else
		if (p[0] == '-' && p[1] == '\0') {
			input = stdin;
			parse("STDIN");
			continue;
		}
#endif
		if ((input = fopen(p, "r")) == NULL)
			fatal("can't open %s", p);
#ifdef ASLD
		if (
			fread(armagic, 2, 1, input) == 1
			&&
			((armagic[0]&0377) |
			 ((unsigned)(armagic[1]&0377)<<8)) == ARMAG
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
#ifndef ASLD
	if (nfile == 0) {
		input = stdin;
		parse("STDIN");
	}
#endif
	commfinish();
	machfinish(PASS_1);
#ifdef ASLD
	if (unresolved) {
		int i;

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
	/*
	if (nfile == 0)
		fatal("no source file");
	*/
#endif
}

#ifdef ASLD

static void
archive(void) {
	long offset;
	struct ar_hdr header;
	char ar_name[AR_NAME_MAX+1];
	char getsize[AR_TOTAL];

	archmode++;
	offset = 2;
	for (;;) {
		if (unresolved == 0)
			break;
		fseek(input,offset,0);
		if (fread(getsize,AR_TOTAL,1,input) != 1)
			break;
		offset += AR_TOTAL;
		strncpy(ar_name,getsize,sizeof(header.ar_name)) ;
		header.ar_size= (((((long) (getsize[AR_SIZE+1]&0377))<<8)+
				((long) (getsize[AR_SIZE  ]&0377))<<8)+
				((long) (getsize[AR_SIZE+3]&0377))<<8)+
				((long) (getsize[AR_SIZE+2]&0377)) ;
		archsize = header.ar_size;
		if (needed()) {
			fseek(input,offset,0);
			archsize = header.ar_size;
			ar_name[AR_NAME_MAX] = '\0';
			parse(remember(ar_name));
		}
		offset += header.ar_size;
		while (offset % 2)
			offset++;
	}
	archmode = 0;
}

static int
needed(void)
{
	int c, first;
	item_t *ip;
	int need;

#ifdef LISTING
	int save;

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
			if (ip == &keytab[KEYSECT]) {
				while ((c = nextchar()) != '\n')
					;
				continue;
			}

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
#endif /* ASLD */

static void
parse(char *s)
{
	int i;
	item_t *ip;
	char *p;

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
		while ((ip = hashtab[H_LOCAL+i])) {
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

static void
pass_23(int n)
{
	int i;
#ifdef ASLD
	ADDR_T base = 0;
#endif
	sect_t *sp;

	if (nerrors)
		stop();
	pass = n;
#ifdef LISTING
	listmode >>= 3;
	if (listmode & 4)
		rewind(listfile);
	listeoln = 1;
#endif
#ifdef THREE_PASS
	bitindex = -1;
	nbits = BITCHUNK;
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
#endif /* ASLD */
	rewind(tempfile);
	yyparse();
	commfinish();
	machfinish(n);
}

void
newmodule(char *s)
{
	static char nmbuf[STRINGMAX];

	switchsect(S_UND);
	if (s && s != modulename) {
		strncpy(nmbuf, s, STRINGMAX-1);
		modulename = nmbuf;
	}
	else modulename = s;
	lineno = 1;
#ifdef NEEDED
        /*
         * problem: it shows the name of the tempfile, not any name
         * the user is familiar with. Moreover, it is not reproducable.
         */
        if ((sflag & (SYM_EXT|SYM_LOC|SYM_LAB)) && PASS_SYMB)
                newsymb(s, S_MOD, 0, (valu_t)0);
#endif
#ifdef LISTING
	listtemp = 0;
	if (dflag & 01000)
		listtemp = listmode;
	listflag = listtemp;
#endif
}

static void
setupoutput(void)
{
	sect_t *sp;
	long off;
	struct outsect outsect;
	struct outsect *pos = &outsect;

	if (! wr_open(aoutpath)) {
		fatal("can't create %s", aoutpath);
	}
	wr_ohead(&outhead);
	/*
	 * section table generation
	 */
	off = SZ_HEAD;
	off += (long)outhead.oh_nsect * SZ_SECT;
	for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
		sp->s_foff = off;
		pos->os_base = SETBASE(sp);
		pos->os_size = sp->s_size + sp->s_comm;
		pos->os_foff = sp->s_foff;
		pos->os_flen = sp->s_size - sp->s_zero;
		pos->os_lign = sp->s_lign;
		off += pos->os_flen;
		wr_sect(pos, 1);
	}
#ifdef RELOCATION
	off += (long)outhead.oh_nrelo * SZ_RELO;
#endif
	if (sflag == 0)
		return;
	off += (long)outhead.oh_nname * SZ_NAME;
	outhead.oh_nchar = off;	/* see newsymb() */
}

static void
commfinish(void)
{
#ifndef ASLD
	int i;
#endif
	struct common_t *cp;
	item_t *ip;
	sect_t *sp;
	valu_t addr;

	switchsect(S_UND);
	/*
	 * assign .comm labels and produce .comm symbol table entries
	 */
	for (cp = commons; cp; cp = cp->c_next) {
		ip = cp->c_it;
#ifndef ASLD
		if (!( ip->i_type & S_EXT)) {
#endif
			sp = &sect[(ip->i_type & S_TYP) - S_MIN];
			if (pass == PASS_1) {
				addr = sp->s_size + sp->s_comm;
				sp->s_comm += ip->i_valu;
				ip->i_valu = addr;
#ifndef ASLD
				ip->i_type &= ~S_COM;
#endif
			}
#ifdef ASLD
#ifdef THREE_PASS
			if (pass == PASS_2) {
				ip->i_valu -= sp->s_gain;
			}
#endif
			if ((sflag & SYM_EXT) && PASS_SYMB)
				newsymb(
					ip->i_name,
					ip->i_type & (S_EXT|S_TYP),
					0,
					load(ip)
				);
#else /* not ASLD */
#ifdef THREE_PASS
			if (pass == PASS_2) {
				cp->c_size -= sp->s_gain;
			}
#endif /* THREE_PASS */
		}
		if (pass == PASS_1) cp->c_size = ip->i_valu;
		if (PASS_SYMB) {
			if (pass != PASS_3 && (ip->i_type & S_EXT)) {
				ip->i_valu = outhead.oh_nname;
			}
			newsymb(
				ip->i_name,
				ip->i_type,
				0,
				cp->c_size
			);
		}
#endif /* not ASLD */
	}
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
			newsymb(
				ip->i_name,
				S_EXT|S_UND,
				0,
				(valu_t)0
			);
		}
#endif /* not ASLD */
	/*
	 * produce symbol table entries for sections
	 */
	if (sflag & SYM_SCT)
		for (sp = sect; sp < &sect[outhead.oh_nsect]; sp++) {
			ip = sp->s_item;
			newsymb(
				ip->i_name,
				(ip->i_type | S_SCT),
				0,
				load(ip)
			);
		}
}
