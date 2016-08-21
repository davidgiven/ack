/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 *  C O M P A C T   A S S E M B L Y   L A N G U A G E   G E N E R A T I O N
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <em_pseu.h>
#include <em_mes.h>
#include "../share/types.h"
#include "ca.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/files.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/get.h"
#include "ca_put.h"

/* This phase transforms the Intermediate Code of the global optimizer
 * to 'standard' compact assembly language, which will be processed
 * by the code generator.
 */

short dlength;
dblock_p* dmap;

char** dnames, **pnames; /* Dynamically allocated arrays of strings.
			 * pnames[i] contains a pointer to the name
			 * of the procedure  with proc_id i.
			 */

STATIC line_p get_ca_lines(lf, p_out)
    FILE* lf;
proc_p* p_out;
{
	/* Read lines of EM text and link them.
	 * Register messages are outputted immediately after the PRO.
	 */

	line_p head, *pp, l;
	line_p headm, *mp;
	arg_p a;

	curinp = lf; /* EM input file */
	pp = &head;
	mp = &headm;
	headm = (line_p)0;
	while (TRUE)
	{
		l = read_line(p_out);
		if (feof(curinp))
			break;
		assert(l != (line_p)0);
		if (INSTR(l) == ps_end && INSTR(head) != ps_pro)
		{
			/* Delete end pseudo after data-unit */
			oldline(l);
			break;
		}
		if (INSTR(l) == ps_mes && l->l_a.la_arg->a_a.a_offset == ms_reg)
		{
			/* l is a register message */
			if (l->l_a.la_arg->a_next == (arg_p)0)
			{
				/* register message without arguments */
				oldline(l);
			}
			else
			{
				*mp = l;
				mp = &l->l_next;
			}
		}
		else
		{
			*pp = l;
			pp = &l->l_next;
		}
		if (INSTR(l) == ps_end)
		{
			break;
		}
	}
	*pp = (line_p)0;
	if (head != (line_p)0 && INSTR(head) == ps_pro)
	{
		/* append register message without arguments to list */
		l = newline(OPLIST);
		l->l_instr = ps_mes;
		a = ARG(l) = newarg(ARGOFF);
		a->a_a.a_offset = ms_reg;
		*mp = l;
		l->l_next = head->l_next;
		head->l_next = headm;
	}
	else
	{
		assert(headm == (line_p)0);
	}
	return head;
}

STATIC int makedmap(dbl)
    dblock_p dbl;
{
	/* construct the dmap table */

	dblock_p d;
	int cnt;

	/* determine the length of the table */

	cnt = 0;
	for (d = dbl; d != (dblock_p)0; d = d->d_next)
		cnt++;
	dmap = (dblock_p*)newmap(cnt);
	for (d = dbl; d != (dblock_p)0; d = d->d_next)
	{
		assert(d->d_id <= cnt);
		dmap[d->d_id] = d;
	}
	return cnt;
}

STATIC getdnames(dumpd)
    FILE* dumpd;
{
	/* Read the names of the datalabels from
	 * the dump file.
	 */

	char str[IDL + 1];
	int id;

	dnames = (char**)newmap(dlength);
	for (;;)
	{
		if (fscanf(dumpd, "%d	%s", &id, str) == EOF)
			return;
		assert(id <= dlength);
		dnames[id] = (char*)newcore(strlen(str) + 1);
		strcpy(dnames[id], str);
	}
}

STATIC getpnames(dumpp)
    FILE* dumpp;
{
	/* Read the names of the procedures from
	 * the dump file.
	 */

	char str[IDL + 1];
	int id;

	pnames = (char**)newmap(plength);
	for (;;)
	{
		if (fscanf(dumpp, "%d	%s", &id, str) == EOF)
			return;
		assert(id <= plength);
		pnames[id] = (char*)newcore(strlen(str) + 1);
		strcpy(pnames[id], str);
	}
}

STATIC new_name(s) char** s;
{
	static int nn = 0;
	char buf[20];
	int len = strlen(*s);

	oldcore(*s, len + 1);
	buf[0] = '_';
	buf[1] = 'I';
	buf[2] = 'I';
	sprintf(&buf[3], "%d", nn);
	nn++;
	*s = (char*)newcore(strlen(buf) + 1);
	strcpy(*s, buf);
}

STATIC uniq_names()
{
	/* The names of all internal procedures and data blocks
	 * are made different. As the optimizer combines several
	 * modules into one, there may be name conflicts between
	 * procedures or data blocks that were internal in
	 * different source modules.
	 */

	proc_p p;
	dblock_p d;

	for (p = fproc; p != (proc_p)0; p = p->p_next)
	{
		if (!(p->p_flags1 & PF_EXTERNAL))
		{
			new_name(&(pnames[p->p_id]));
		}
	}
	for (d = fdblock; d != (dblock_p)0; d = d->d_next)
	{
		if (!(d->d_flags1 & DF_EXTERNAL) && dnames[d->d_id])
		{
			new_name(&(dnames[d->d_id]));
		}
	}
}

main(argc, argv) int argc;
char* argv[];
{
	/* CA does not output proctable etc. files. Instead, its
	 * pname2 and dname2 arguments contain the names of the
	 * dump files created by IC.
	 */
	FILE* f, *f2; /* The EM input and output. */
	FILE* df, *pf; /* The dump files */
	line_p lnp;

	fproc = getptable(pname); /* proc table */
	fdblock = getdtable(dname); /* data block table */
	dlength = makedmap(fdblock); /* allocate dmap table */
	df = openfile(dname2, "r");
	getdnames(df);
	fclose(df);
	pf = openfile(pname2, "r");
	getpnames(pf);
	fclose(pf);
	uniq_names();
	f = openfile(lname, "r");
	f2 = stdout;
	cputmagic(f2); /* write magic number */
	while ((lnp = get_ca_lines(f, &curproc)) != (line_p)0)
	{
		cputlines(lnp, f2);
	}
	fclose(f);
	fclose(f2);
	exit(0);
}
