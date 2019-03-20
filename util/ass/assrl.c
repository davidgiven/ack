/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include		<stdio.h>
#include        "ass00.h"
#include        "assex.h"
#include		"asscm.h"
#include		"assrl.h"



#define COPYFINAL       1
#define COPYTEMP        0

/*
 * collection of routines to deal with relocation business
 */


static void dataprocess(FILE *, FILE *);
static void textprocess(FILE *, FILE *);

relc_t * text_reloc(glob_t *glosym, FOFFSET off, int typ)
{

	/*
	 * prepare the relocation that has to be done at text-offset off
	 * according to global symbol glosym.
	 * NOTE: The pointer glosym will point into mglobs[], while at
	 *       the time copyout() is called all the symbols here
	 *       will have disappeared.
	 *       The procedure upd_reloc() will change this pointer
	 *       into the one in xglobs[] later.
	 */

	register relc_t *nxtextreloc;

	nxtextreloc = rlp_cast getarea(sizeof *nxtextreloc);
	if (!f_text)
	{
		f_text = nxtextreloc;
	}
	else
	{
		l_text->r_next = nxtextreloc;
	}
	nxtextreloc->r_next = rlp_cast 0;
	l_text = nxtextreloc;
	nxtextreloc->r_off = off;
	nxtextreloc->r_val.rel_gp = glosym;
	nxtextreloc->r_typ = typ; /* flags of instruction */
	return (nxtextreloc);
}

relc_t * data_reloc(char *arg ,FOFFSET off, int typ)
{

	/*
	 * Same as above.
	 */

	register relc_t *nxdatareloc;

	nxdatareloc = rlp_cast getarea(sizeof *nxdatareloc);
	if (!f_data)
	{
		f_data = nxdatareloc;
	}
	else
	{
		l_data->r_next = nxdatareloc;
	}
	nxdatareloc->r_next = rlp_cast 0;
	l_data = nxdatareloc;
	nxdatareloc->r_off = off;
	nxdatareloc->r_val.rel_lp = lbp_cast arg;
	nxdatareloc->r_typ = typ;
	return (nxdatareloc);
}

void copyout(void)
{
	register int i;
	int remtext;

	/*
	 * Make the e.out file that looks as follows:
	 *
	 *      __________________________
	 *      |      MAGIC             | \
	 *      |      FLAGS             |  \
	 *      |      UNRESOLVED        |   \
	 *      |      VERSION           |    | 8*(2-byte word) header
	 *      |      WORDSIZE          |    | for interpreter selection
	 *      |      PTRSIZE           |   /
	 *      |      <UNUSED>          |  /
	 *      |      <UNUSED>          | /
	 *      |      NTEXT             | \
	 *      |      NDATA             |  \
	 *      |      NPROC             |   \
	 *      |      ENTRY-POINT       |    | 8*(wordsize-word) header
	 *      |      NLINES            |    | for interpreter proper
	 *      |      <UNUSED>          |   /
	 *      |      <UNUSED>          |  /
	 *      |      <UNUSED>          | /
	 *      |________________________|
	 *      |                        |
	 *      |      TEXT              |        zero filled
	 *      |                        |        if not word multiple
	 *      |________________________|
	 *      |                        |
	 *      |      DATA              |
	 *      |                        |
	 *      |________________________|
	 *      |                        |
	 *      |      PROCTABLE         |
	 *      |                        |
	 *      |________________________|
	 *
	 *
	 */

	remtext = textbytes % wordsize;
	if (remtext != 0)
		remtext = wordsize - remtext;

	if ((ifile = fopen(eout, "w")) == 0)
		fatal("can't create e.out");

	rewind(tfile);
	rewind(dfile);

	xput16(as_magic, ifile);
	xput16(intflags, ifile);
	xput16(unresolved, ifile);
	xput16(VERSION, ifile);
	xput16(wordsize, ifile);
	xput16(ptrsize, ifile);
	xput16(0, ifile);
	xput16(0, ifile);
	xputa(textbytes + remtext, ifile);
	xputa((cons_t) datablocks, ifile);
	xputa((cons_t) procnum, ifile);
	xputa((cons_t) searchproc(MAIN, xprocs, oursize->n_xproc)->p_num, ifile);
	xputa((cons_t) sourcelines, ifile);
	xputa((cons_t) databytes, ifile);
	xputa((cons_t) 0, ifile);
	xputa((cons_t) 0, ifile);

	textprocess(tfile, ifile);
	while (remtext--)
		xputc(0, ifile);

	dataprocess(dfile, ifile);
	for (i = 0; i < procnum; i++)
	{
		xputarb(ptrsize, proctab[i].pr_loc, ifile);
		xputarb(ptrsize, proctab[i].pr_off, ifile);
	}
	if (fclose(ifile) == EOF)
		;
}

static void dataprocess(FILE *f1, FILE *outf)
{
	relc_t datareloc;
	FOFFSET i;
	register int ieof;

	rewind(rdfile);
	ieof = getblk(rdfile, (char *) (&datareloc.r_off),
			sizeof datareloc - sizeof datareloc.r_next);
	for (i = 0; i < dataoff && !ieof; i++)
	{
		if (i == datareloc.r_off)
		{
			switch (datareloc.r_typ)
			{
			case RELADR:
				xputa(xgeta(f1) + datareloc.r_val.rel_i, outf);
				i += ptrsize - 1;
				break;
			case RELGLO:
				if (datareloc.r_val.rel_gp->g_status & DEF)
				{
					xputa(xgeta(f1) + datareloc.r_val.rel_gp->g_val.g_addr, outf);
					i += ptrsize - 1;
					break;
				}
				if (unresolved == 0)
					fatal("Definition botch");
			case RELHEAD:
				xputc((int) (xgetc(f1) + datareloc.r_val.rel_i), outf);
				break;
			default:
				fatal("Bad r_typ in dataprocess");
			}
			ieof = getblk(rdfile, (char *) (&datareloc.r_off),
					sizeof datareloc - sizeof datareloc.r_next);
		}
		else
			xputc(xgetc(f1), outf);
	}
	for (; i < dataoff; i++)
		xputc(xgetc(f1), outf);
	if (!ieof && !getblk(rdfile, (char *) &datareloc, 1))
		fatal("data relocation botch");
}

static void textprocess(FILE *f1, FILE *outf)
{
	relc_t textreloc;
	cons_t n;
	FOFFSET i;
	FILE *otfile;
	int insl;
	register int ieof;
	char *op_curr;
	register FOFFSET keep;

	rewind(rtfile);
	keep = textoff;
	textoff = 0;
	otfile = tfile;
	tfile = outf;
	/* This redirects the output of genop */
	ieof = getblk(rtfile, (char *) (&textreloc.r_off),
			sizeof textreloc - sizeof textreloc.r_next);
	for (i = 0; i < keep && !ieof; i++)
	{
		if (i == textreloc.r_off)
		{
			if (textreloc.r_typ & RELMNS)
			{
				n = textreloc.r_val.rel_i;
			}
			else
			{
				if (textreloc.r_val.rel_gp->g_status & DEF)
				{
					n = textreloc.r_val.rel_gp->g_val.g_addr;
				}
				else
				{
					if (unresolved == 0)
						fatal("Definition botch");
					xputc(xgetc(f1), outf);
					ieof = getblk(rtfile, (char *) (&textreloc.r_off),
							sizeof textreloc - sizeof textreloc.r_next);
					continue;
				}
			}
			op_curr = &opchoice[textreloc.r_typ & ~RELMNS];
			insl = oplength(*op_curr);
			genop(op_curr, n + xgetarb(insl, f1), PAR_G);
			i += insl - 1;
			ieof = getblk(rtfile, (char *) (&textreloc.r_off),
					sizeof textreloc - sizeof textreloc.r_next);
		}
		else
		{
			xputc(xgetc(f1), outf);
		}
	}
	for (; i < keep; i++)
		xputc(xgetc(f1), outf);
	if (!ieof && !getblk(rtfile, (char *) &textreloc, 1))
		fatal("text relocation botch");
	textoff = keep;
	tfile = otfile;
}

void upd_reloc(void)
{
	register relc_t *p;
	register glob_t *gbp;

	/*
	 * Change reloc-tables such that for every pointer into mglobs
	 * either the corresponding pointer into xglobs or its value
	 * is substituted.
	 *
	 * Use is made of the known order of mglobs and xglobs
	 * see also getcore()
	 */

	while ( (p = f_text) != NULL)
	{
		gbp = p->r_val.rel_gp;
		if (gbp->g_status & DEF)
		{
			p->r_typ |= RELMNS;
			p->r_val.rel_i = gbp->g_val.g_addr;
		}
		else
			p->r_val.rel_gp = gbp->g_val.g_gp;
		putblk(rtfile, (char *) (&(p->r_off)), sizeof *p - sizeof p);
		f_text = p->r_next;
		freearea((area_t) p, sizeof *p);
	}

	while ( (p = f_data) != NULL)
	{
		if (p->r_typ == RELGLO)
		{
			gbp = p->r_val.rel_gp;
			if (gbp->g_status & DEF)
			{
				p->r_typ = RELADR;
				p->r_val.rel_i = gbp->g_val.g_addr;
			}
			else
				p->r_val.rel_gp = gbp->g_val.g_gp;
		}
		putblk(rdfile, (char *) (&(p->r_off)), sizeof *p - sizeof p);
		f_data = p->r_next;
		freearea((area_t) p, sizeof *p);
	}
	l_data = rlp_cast 0;
}
