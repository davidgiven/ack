/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include        "ass00.h"
#include        "assex.h"
#include		"assci.h"
#include		"asscm.h"
#include		"assrl.h"
#include        <em_mes.h>
#include        <em_pseu.h>
#include        <em_ptyp.h>

/*
 * read compact code and fill in tables
 */

static int tabval;
static cons_t argval;

static int oksizes; /* MES EMX,.,. seen */

static enum m_type
{
	CON, ROM, HOLBSS
} memtype;
static int valtype; /* Transfer of type information between
 valsize, inpseudo and putval
 */


/* Forward declarations
 * Yes, there is a better way to reorder to avoid these forward
 * declarations, but its complex to do, so we use the lazy way
 */
static int getarg(int);
static cons_t getint(void);
static glob_t *getlab(int);
static char *getdig(char *, unsigned int);
static void make_string(unsigned int);
static void getstring(void);
static void inident(void);
static char *inproname(void);
static int needed(void);
static cons_t valsize(void);
static void setline(void);
static void inpseudo(int);
static void compact_line(void);
static cons_t maxval(int);
static void setsizes(void);
static void exchange(int, int);
static void doinsert(line_t *, int, int);
static void putval(void);
static void chkstart(void);
static void typealign(enum m_type);
static void sizealign(cons_t);
static void extconst(cons_t);
static void extbss(cons_t);
static void extloc(register locl_t *);
static void extglob(glob_t *, cons_t);
static void extpro(proc_t *);
static void extstring(void);
static void extxcon(int);
static long myatol(register char *);
static void extvcon(int);


static int table3(int i)
{

	switch (i)
	{
	case sp_ilb1:
		tabval = get8();
		break;
	case sp_dlb1:
		make_string(get8());
		i = sp_dnam;
		break;
	case sp_dlb2:
		tabval = get16();
		if (tabval < 0)
		{
			error("illegal data label .%d", tabval);
			tabval = 0;
		}
		make_string(tabval);
		i = sp_dnam;
		break;
	case sp_cst2:
		argval = get16();
		break;
	case sp_ilb2:
		tabval = get16();
		if (tabval < 0)
		{
			error("illegal instruction label %d", tabval);
			tabval = 0;
		}
		i = sp_ilb1;
		break;
	case sp_cst4:
		i = sp_cst2;
		argval = get32();
		break;
	case sp_dnam:
	case sp_pnam:
		inident();
		break;
	case sp_scon:
		getstring();
		break;
	case sp_doff:
		getarg(sym_ptyp);
		getarg(cst_ptyp);
		break;
	case sp_icon:
	case sp_ucon:
	case sp_fcon:
		getarg(cst_ptyp);
		consiz = argval;
		if (consiz < wordsize ? wordsize % consiz != 0 : consiz % wordsize != 0)
		{
			fatal("illegal object size");
		}
		getstring();
		break;
	}
	return (i);
}

int get16(void)
{
	register int l_byte, h_byte;

	l_byte = get8();
	h_byte = get8();
	if (h_byte >= 128)
		h_byte -= 256;
	return l_byte | (h_byte * 256);
}

int getu16(void)
{
	register int l_byte, h_byte;

	l_byte = get8();
	h_byte = get8();
	return l_byte | (h_byte * 256);
}

cons_t get32(void)
{
	register cons_t l;
	register int h_byte;

	l = get8();
	l |= (unsigned) get8() * 256;
	l |= get8() * 256L * 256L;
	h_byte = get8();
	if (h_byte >= 128)
		h_byte -= 256;
	return l | (h_byte * 256L * 256 * 256L);
}

static int table1(void)
{
	register int i;

	i = xget8();
	if (i < sp_fmnem + sp_nmnem && i >= sp_fmnem)
	{
		tabval = i - sp_fmnem;
		return (sp_fmnem);
	}
	if (i < sp_fpseu + sp_npseu && i >= sp_fpseu)
	{
		tabval = i;
		return (sp_fpseu);
	}
	if (i < sp_filb0 + sp_nilb0 && i >= sp_filb0)
	{
		tabval = i - sp_filb0;
		return (sp_ilb1);
	}
	return (table3(i));
}

static int table2(void)
{
	register int i;

	i = get8();
	if (i < sp_fcst0 + sp_ncst0 && i >= sp_fcst0)
	{
		argval = i - sp_zcst0;
		return (sp_cst2);
	}
	return (table3(i));
}

/* Read argument of instruction */
static int getarg(int typset)
{
	register int t, argtyp;

	argtyp = t = table2();
	t -= sp_fspec;
	t = 1 << t;
	if ((typset & t) == 0)
		error("bad argument type %d", argtyp);
	return (argtyp);
}

static cons_t getint(void)
{
	getarg(cst_ptyp);
	return (argval);
}

static glob_t *getlab(int status)
{
	getarg(sym_ptyp);
	return (glo2lookup(string, status));
}

static char *getdig(char *str, unsigned int number)
{
	register int remain;

	remain = number % 10;
	number /= 10;
	if (number)
		str = getdig(str, number);
	*str++ = '0' + remain;
	return str;
}

static void make_string(unsigned int n)
{
	string[0] = '.';
	*getdig(&string[1], n) = 0;
}

static void getstring(void)
{
	register char *p;
	register int n;

	getarg(cst_ptyp);
	if (argval < 0 || argval >= MAXSTRING - 1)
		fatal("string/identifier too long");
	strlngth = n = argval;
	p = string;
	while (--n >= 0)
		*p++ = get8();
	*p = 0;
}

static void inident(void)
{
	getstring();
}

static char *inproname(void)
{
	getarg(ptyp(sp_pnam));
	return (string);
}

static int needed(void)
{
	register glob_t *g;
	register proc_t *p;

	for (;;)
	{
		switch (table2())
		{
		case sp_dnam:
			if ( (g = xglolookup(string, SEARCHING)) != NULL)
			{
				if ((g->g_status & DEF) != 0)
					continue;
			}
			else
				continue;
			break;
		case sp_pnam:
			p = searchproc(string, xprocs, oursize->n_xproc);
			if (p->p_name)
			{
				if ((p->p_status & DEF) != 0)
					continue;
			}
			else
				continue;
			break;
		default:
			error("Unexpected byte after ms_ext");
		case sp_cend:
			return FALSE;
		}
		while (table2() != sp_cend)
			;
		return TRUE;
	}
}

static cons_t valsize(void)
{
	switch (valtype = table2())
	{ /* valtype is used by putval and inpseudo */
	case sp_cst2:
		return wordsize;
	case sp_ilb1:
	case sp_dnam:
	case sp_doff:
	case sp_pnam:
		return ptrsize;
	case sp_scon:
		return strlngth;
	case sp_fcon:
	case sp_icon:
	case sp_ucon:
		return consiz;
	case sp_cend:
		return 0;
	default:
		fatal("value expected");
		return 0;
		/* NOTREACHED */
	}
}

void newline(int type)
{
	register line_t *n_lnp;

	if (type > VALLOW)
		type = VALLOW;
	n_lnp = lnp_cast getarea((unsigned) linesize[type]);
	n_lnp->l_next = pstate.s_fline;
	pstate.s_fline = n_lnp;
	n_lnp->type1 = type;
	n_lnp->opoff = NO_OFF;
}

static void setline(void)
{

	/* Get line numbers correct */

	if (pstate.s_fline &&
	ctrunc(pstate.s_fline->instr_num) == sp_fpseu)
	{
		/* Already one present */
		pstate.s_fline->ad.ad_ln.ln_extra++;
	}
	else
	{
		newline(LINES);
		pstate.s_fline->instr_num = sp_fpseu;
		pstate.s_fline->ad.ad_ln.ln_extra = 0;
		pstate.s_fline->ad.ad_ln.ln_first = line_num;
	}

}

static void inpseudo(int instr_no)
{
	cons_t cst;
	register proc_t *prptr;
	cons_t objsize;
	cons_t par1, par2;
	register char *pars;

	/*
	 * get operands of pseudo (if needed) and process it.
	 */

	switch (ctrunc(instr_no))
	{
	case ps_bss:
		chkstart();
		typealign(HOLBSS);
		cst = getint(); /* number of bytes */
		extbss(cst);
		break;
	case ps_hol:
		chkstart();
		typealign(HOLBSS);
		holsize = getint();
		holbase = databytes;
		extbss(holsize);
		break;
	case ps_rom:
	case ps_con:
		chkstart();
		typealign( ctrunc(instr_no) == ps_rom ? ROM : CON);
		while ((objsize = valsize()) != 0)
		{
			if (valtype != sp_scon)
				sizealign(objsize);
			putval();
			databytes += objsize;
		}
		break;
	case ps_end:
		prptr = pstate.s_curpro;
		if (prptr == prp_cast 0)
			fatal("unexpected END");
		proctab[prptr->p_num].pr_off = textbytes;
		if (procflag)
		{
			printf("%6lu\t%6lo\t%5d\t%-12s\t%s", textbytes, textbytes,
					prptr->p_num, prptr->p_name, curfile);
			if (archmode)
				printf("(%.14s)", archhdr.ar_name);
			printf("\n");
		}
		par2 = proctab[prptr->p_num].pr_loc;
		if (getarg(cst_ptyp | ptyp(sp_cend)) == sp_cend)
		{
			if (par2 == -1)
			{
				fatal("size of local area unspecified");
			}
		}
		else
		{
			if (par2 != -1 && argval != par2)
			{
				fatal("inconsistent local area size");
			}
			proctab[prptr->p_num].pr_loc = argval;
		}
		setline();
		do_proc();
		break;
	case ps_mes:
		switch ( int_cast getint())
		{
		case ms_err:
			error("module with error");
			ertrap();
			/* NOTREACHED */
		case ms_emx:
			if (oksizes)
			{
				if (wordsize != getint())
				{
					fatal("Inconsistent word size");
				}
				if (ptrsize != getint())
				{
					fatal("Inconsistent pointer size");
				}
			}
			else
			{
				oksizes++;
				wordsize = getint();
				ptrsize = getint();
				if (wordsize != 2 && wordsize != 4)
				{
					fatal("Illegal word size");
				}
				if (ptrsize != 2 && ptrsize != 4)
				{
					fatal("Illegal pointer size");
				}
				setsizes();
			}
			++mod_sizes;
			break;
		case ms_src:
			break;
		case ms_flt:
			intflags |= 020;
			break; /*floats used*/
		case ms_ext:
			if (!needed())
			{
				eof_seen++;
			}
			if (line_num > 2)
			{
				werror("mes ms_ext must be first or second pseudo");
			}
			return;
		}
		while (table2() != sp_cend)
			;
		break;
	case ps_exc:
		par1 = getint();
		par2 = getint();
		if (par1 == 0 || par2 == 0)
			break;
		exchange((int) par2, (int) par1);
		break;
	case ps_exa:
		getlab(EXTERNING);
		break;
	case ps_ina:
		getlab(INTERNING);
		break;
	case ps_pro:
		chkstart();
		initproc();
		pars = inproname();
		if (getarg(cst_ptyp | ptyp(sp_cend)) == sp_cend)
		{
			par2 = -1;
		}
		else
		{
			par2 = argval;
		}
		prptr = prolookup(pars, PRO_DEF);
		proctab[prptr->p_num].pr_loc = par2;
		pstate.s_curpro = prptr;
		break;
	case ps_inp:
		prptr = prolookup(inproname(), PRO_INT);
		break;
	case ps_exp:
		prptr = prolookup(inproname(), PRO_EXT);
		break;
	default:
		fatal("unknown pseudo");
	}
	if (!mod_sizes)
		fatal("Missing size specification");
	if (databytes > maxadr)
		error("Maximum data area size exceeded");
}

/*
 * read one "line" of compact code.
 */
static void compact_line(void)
{
	register int instr_no;


	curglosym = 0;
	switch (table1())
	{
	default:
		fatal("unknown byte at start of \"line\""); /* NOTREACHED */
	case EOF:
		eof_seen++;
		while (pstate.s_prevstat != pst_cast 0)
		{
			error("missing end");
			do_proc();
		}
		return;
	case sp_fmnem:
		if (pstate.s_curpro == prp_cast 0)
		{
			error("instruction outside procedure");
		}
		instr_no = tabval;
		if ((em_flag[instr_no] & EM_PAR) == PAR_NO)
		{
			newline(MISSING);
			pstate.s_fline->instr_num = instr_no;
			return;
		}
		/*
		 * This instruction should have an opcode, so read it after
		 * this switch.
		 */
		break;
	case sp_dnam:
		chkstart();
		align(wordsize);
		curglosym = glo2lookup(string, DEFINING);
		curglosym->g_val.g_addr = databytes;
		lastglosym = curglosym;
		setline();
		line_num++;
		if (table1() != sp_fpseu)
			fatal("no pseudo after data label");
	case sp_fpseu:
		inpseudo(tabval);
		setline();
		return;
	case sp_ilb1:
		newline(LOCSYM);
		pstate.s_fline->ad.ad_lp = loclookup(tabval, DEFINING);
		pstate.s_fline->instr_num = sp_ilb1;
		return;
	}

	/*
	 * Now process argument
	 */

	switch (table2())
	{
	default:
		fatal("unknown byte at start of argument"); /*NOTREACHED*/
	case sp_cst2:
		if ((em_flag[instr_no] & EM_PAR) == PAR_B)
		{
			/* value indicates a label */
			newline(LOCSYM);
			pstate.s_fline->ad.ad_lp = loclookup((int) argval, OCCURRING);
		}
		else
		{
			if (argval >= VAL1(VALLOW) && argval <= VAL1(VALHIGH))
			{
				newline(VALLOW);
				pstate.s_fline->type1 = argval + VALMID;
			}
			else
			{
				newline(CONST);
				pstate.s_fline->ad.ad_i = argval;
				pstate.s_fline->type1 = CONST;
			}
		}
		break;
	case sp_ilb1:
		newline(LOCSYM);
		pstate.s_fline->ad.ad_lp = loclookup(tabval, OCCURRING);
		break;
	case sp_dnam:
		newline(GLOSYM);
		pstate.s_fline->ad.ad_gp = glo2lookup(string, OCCURRING);
		break;
	case sp_pnam:
		newline(PROCNAME);
		pstate.s_fline->ad.ad_pp = prolookup(string, PRO_OCC);
		break;
	case sp_cend:
		if ((em_flag[instr_no] & EM_PAR) != PAR_W)
		{
			fatal("missing operand");
		}
		newline(MISSING);
		break;
	case sp_doff:
		newline(GLOOFF);
		pstate.s_fline->ad.ad_df.df_i = argval;
		pstate.s_fline->ad.ad_df.df_gp = glo2lookup(string, OCCURRING);
		break;
	}
	pstate.s_fline->instr_num = instr_no;
	return;
}


void read_compact(void)
{
	init_module();
	pass = 1;
	eof_seen = 0;
	do
	{
		compact_line();
		line_num++;
	} while (!eof_seen);
	endproc(); /* Throw away unwanted garbage */
	if (mod_sizes)
		end_module();
	/* mod_sizes is only false for rejected library modules */
}






static cons_t maxval(int bits)
{
	/* find the maximum positive value,
	 * fitting in 'bits' bits AND
	 * fitting in a 'cons_t' .
	 */

	cons_t val;
	val = 1;
	while (bits--)
	{
		val <<= 1;
		if (val < 0)
			return ~val;
	}
	return val - 1;
}

static void setsizes(void)
{
	maxadr = maxval(8 * ptrsize);
	maxint = maxval(8 * wordsize - 1);
	maxunsig = maxval(8 * wordsize);
	maxdint = maxval(2 * 8 * wordsize - 1);
	maxdunsig = maxval(2 * 8 * wordsize);
}

static void exchange(int p1, int p2)
{
	int size, line;
	register line_t *t_lnp, *a_lnp, *b_lnp;

	/* Since the lines are linked backwards it is easy
	 * to count the number of lines backwards.
	 * Each instr counts for 1, each pseudo for ln_extra + 1.
	 * The line numbers in error messages etc. are INCORRECT
	 * If exc's are used.
	 */

	line = line_num;
	size = 0;
	newline(LINES);
	a_lnp = pstate.s_fline;
	a_lnp->instr_num = sp_fpseu;
	a_lnp->ad.ad_ln.ln_first = line;
	a_lnp->ad.ad_ln.ln_extra = -1;
	for (; a_lnp; a_lnp = a_lnp->l_next)
	{
		line--;
		switch (ctrunc(a_lnp->instr_num))
		{
		case sp_fpseu:
			line = a_lnp->ad.ad_ln.ln_first;
			size += a_lnp->ad.ad_ln.ln_extra;
			break;
		case sp_ilb1:
			a_lnp->ad.ad_lp->l_min -= p2;
			break;
		}
		size++;
		if (size >= p1)
			break;
	}
	if ((size -= p1) > 0)
	{
		if ( ctrunc(a_lnp->instr_num) != sp_fpseu)
		{
			fatal("EXC inconsistency");
		}
		doinsert(a_lnp, line, size - 1);
		a_lnp->ad.ad_ln.ln_extra -= size;
		size = 0;
	}
	else
	{
		if (a_lnp)
			doinsert(a_lnp, line, -1);
	}
	b_lnp = a_lnp;
	while (b_lnp)
	{
		b_lnp = b_lnp->l_next;
		line--;
		switch (ctrunc(b_lnp->instr_num))
		{
		case sp_fpseu:
			size += b_lnp->ad.ad_ln.ln_extra;
			line = b_lnp->ad.ad_ln.ln_first;
			break;
		case sp_ilb1:
			b_lnp->ad.ad_lp->l_min += p1;
			break;
		}
		size++;
		if (size >= p2)
			break;
	}
	if (!b_lnp)
	{ /* if a_lnp==0, so is b_lnp */
		fatal("Cannot perform exchange");
	}
	if ((size -= p2) > 0)
	{
		if ( ctrunc(b_lnp->instr_num) != sp_fpseu)
		{
			fatal("EXC inconsistency");
		}
		doinsert(b_lnp, line, size - 1);
		b_lnp->ad.ad_ln.ln_extra -= size;
	}
	else
	{
		doinsert(b_lnp, line, -1);
	}
	t_lnp = b_lnp->l_next;
	b_lnp->l_next = pstate.s_fline;
	pstate.s_fline = a_lnp->l_next;
	a_lnp->l_next = t_lnp;
}

static void doinsert(line_t *lnp, int first, int extra)
{
	/* Beware : s_fline will be clobbered and restored */
	register line_t *t_lnp;

	t_lnp = pstate.s_fline;
	pstate.s_fline = lnp->l_next;
	newline(LINES);
	pstate.s_fline->instr_num = sp_fpseu;
	pstate.s_fline->ad.ad_ln.ln_first = first;
	pstate.s_fline->ad.ad_ln.ln_extra = extra;
	lnp->l_next = pstate.s_fline;
	pstate.s_fline = t_lnp; /* restore */
}

static void putval(void)
{
	switch (valtype)
	{
	case sp_cst2:
		extconst(argval);
		return;
	case sp_ilb1:
		extloc(loclookup(tabval, OCCURRING));
		return;
	case sp_dnam:
		extglob(glo2lookup(string, OCCURRING), (cons_t) 0);
		return;
	case sp_doff:
		extglob(glo2lookup(string, OCCURRING), argval);
		return;
	case sp_pnam:
		extpro(prolookup(string, PRO_OCC));
		return;
	case sp_scon:
		extstring();
		return;
	case sp_fcon:
		extxcon(DATA_FCON);
		return;
	case sp_icon:
		extvcon(DATA_ICON);
		return;
	case sp_ucon:
		extvcon(DATA_UCON);
		return;
	default:
		fatal("putval notreached");
		/* NOTREACHED */
	}
}

static void chkstart(void)
{
	static int absout = 0;

	if (absout)
		return;
	if (!oksizes)
		fatal("missing size specification");
	set_mode(DATA_CONST);
	extconst((cons_t) 0);
	databytes = wordsize;
	set_mode(DATA_REP);
	if (wordsize < ABSSIZE)
	{
		register int factor = ABSSIZE / wordsize - 1;
		extadr((cons_t) factor);
		databytes += factor * wordsize;
	}
	absout++;
	memtype = HOLBSS;
}

static void typealign(enum m_type new)
{
	if (memtype == new)
		return;
	align(wordsize);
	memtype = new;
}

static void sizealign(cons_t size)
{
	align(size > wordsize ? wordsize : (int) size);
}

void align(int size)
{
	while (databytes % size)
	{
		set_mode(DATA_BYTES);
		ext8(0);
		databytes++;
	}
}

static void extconst(cons_t n)
{
	set_mode(DATA_CONST);
	extword(n);
}

static void extbss(cons_t n)
{
	cons_t objsize, amount;
	cons_t sv_argval;
	int sv_tabval;

	if (n <= 0)
	{
		if (n < 0)
			werror("negative bss/hol size");
		if (table2() == sp_cend || table2() == sp_cend)
		{
			werror("Unexpected end-of-line");
		}
		return;
	}
	set_mode(DATA_NUL); /* flush descriptor */
	objsize = valsize();
	if (objsize == 0)
	{
		werror("Unexpected end-of-line");
		return;
	}
	if (n % objsize != 0)
		error("BSS/HOL incompatible sizes");
	sv_tabval = tabval;
	sv_argval = argval;
	getarg(sp_cst2);
	if (argval < 0 || argval > 1)
		error("illegal last argument");
	databytes += n;
	if (argval == 1)
	{
		tabval = sv_tabval;
		argval = sv_argval;
		putval();
		amount = n / objsize;
		if (amount > 1)
		{
			set_mode(DATA_REP);
			extadr(amount - 1);
		}
	}
	else
	{
		n = (n + wordsize - 1) / wordsize;
		while (n > MAXBYTE)
		{
			set_mode(DATA_BSS);
			ext8(MAXBYTE);
			n -= MAXBYTE;
		}
		set_mode(DATA_BSS);
		ext8((int) n);
	}
}

static void extloc(register locl_t *lbp)
{

	/*
	 * assemble a pointer constant from a local label.
	 * For example  con *1
	 */
	set_mode(DATA_IPTR);
	data_reloc( chp_cast lbp, dataoff, RELLOC);
	extadr((cons_t) 0);
}

static void extglob(glob_t *agbp, cons_t off)
{
	register glob_t *gbp;

	/*
	 * generate a word of data that is defined by a global symbol.
	 * Various relocation has to be prepared here in some cases
	 */
	gbp = agbp;
	set_mode(DATA_DPTR);
	if (gbp->g_status & DEF)
	{
		extadr(gbp->g_val.g_addr + off);
	}
	else
	{
		data_reloc( chp_cast gbp, dataoff, RELGLO);
		extadr(off);
	}
}

static void extpro(proc_t *aprp)
{
	/*
	 * generate a address that is defined by a procedure descriptor.
	 */
	consiz = ptrsize;
	set_mode(DATA_UCON);
	extarb((int) ptrsize, (long) (aprp->p_num));
}

static void extstring(void )
{
	register char *s;
	register int n;

	/*
	 * generate data for a string.
	 */
	for (n = strlngth, s = string; n--;)
	{
		set_mode(DATA_BYTES);
		ext8(*s++);
	}
	return;
}

static void extxcon(int header)
{
	register char *s;
	register int n;

	/*
	 * generate data for a floating constant initialized by a string.
	 */

	set_mode(header);
	s = string;
	for (n = strlngth; n--;)
	{
		if (*s == 0)
			error("Zero byte in initializer");
		ext8(*s++);
	}
	ext8(0);
	return;
}

/* Added atol() that ignores overflow. --Ceriel */
static long myatol(register char *s)
{
	register long total = 0;
	register unsigned digit;
	int minus = 0;

	while (*s == ' ' || *s == '\t')
		s++;
	if (*s == '+')
		s++;
	else if (*s == '-')
	{
		s++;
		minus = 1;
	}
	while ((digit = *s++ - '0') < 10)
	{
		total *= 10;
		total += digit;
	}
	return (minus ? -total : total);
}

static void extvcon(int header)
{
	/*
	 * generate data for a constant initialized by a string.
	 */

	set_mode(header);
	if (consiz > 4)
	{
		error("Size of initializer exceeds loader capability");
	}
	extarb((int) consiz, myatol(string));
	return;
}
