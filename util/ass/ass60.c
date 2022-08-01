/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include        "ass00.h"
#include        "assex.h"
#include        "ip_spec.h"


#ifdef DUMP
static char *typestr[] =
{ "missing", "const", "procname", "glosym", "locsym", "glosym+off", "pseudo" };
static char *labstr[] =
{ "EMPTY", "no", "yes", "seen", "notpresent" };
static char formstr[] =
{ 'm', 's', '-', '1', '2', '4', '8' };
static char *r_data[] =
{ "null", "glob", "head", "loc", "adr" };

cons_t nicepr(int typ, addr_u *ap)
{
	register proc_t *pl;

	switch (typ)
	{
	case CONST:
		return (ap->ad_i);
	case LOCSYM:
		return (int_cast (intptr_t)ap->ad_lp);
	case GLOOFF:
		return (ap->ad_df.df_gp - mglobs);
	case GLOSYM:
		return (ap->ad_gp - mglobs);
	case PROCNAME:
		pl = ap->ad_pp;
		;
		if (pl->p_status & EXT)
			return ((pl - xprocs) + 1000);
		else
			return (pl - mprocs);
	default:
		if (typ >= VALLOW && typ <= VALHIGH)
			return VAL1(typ);
		break;
	}
	return (0);
}

char *pflags(int flg)
{
	static char res[9];
	register char *cp;

	cp = res;
	if (flg & OPESC)
		*cp++ = 'e';
	switch (flg & OPRANGE)
	{
	case OP_NEG:
		*cp++ = 'N';
		break;
	case OP_POS:
		*cp++ = 'P';
		break;
	}
	if (flg & OPWORD)
		*cp++ = 'w';
	if (flg & OPNZ)
		*cp++ = 'o';
	*cp++ = formstr[flg & OPTYPE];
	*cp++ = 0;
	return res;
}

void dump(int n)
{
	register glob_t *gb;
	register line_t *ln;
	register locl_t *lbp;
	register locl_t *lbhead;
	proc_t *pl;
	int i;
	int insno;
	extern char em_mnem[][4];

	if (d_flag == 0)
		return;
	if ((n == 0 && d_flag) || (n == 4 && d_flag >= 2)
			|| (n < 100 && d_flag >= 3))
	{
		printf("\nEM1-assembler      *****   pass %1d complete:\n", n);
		printf("current size %ld\n", prog_size);
		printf("  %9.9s%9.9s%14.14s%8.8s%8.8s\n", "instr_nr", "type1", "addr1",
				"length", "format");
		for (ln = pstate.s_fline; ln;
				ln = ln->l_next, n >= 3 || n == 0 ? i++ : i--)
		{
			insno = ctrunc(ln->instr_num);
			if (insno == sp_fpseu)
			{
				i = ln->ad.ad_ln.ln_first;
				continue;
			}
			printf("%4d  ", i);
			switch (insno)
			{
			default:
				printf(" %3.3s", em_mnem[insno]);
				break;
			case sp_ilb1:
				printf("l   ");
				break;
			case sp_fpseu:
				printf("p   ");
				break;
			}
			printf(" %9.9s%14ld",
					typestr[ln->type1 < VALLOW ? ln->type1 : CONST],
					nicepr(ln->type1, &ln->ad));
			if (ln->opoff != NO_OFF)
				printf("%5d     %.6s", oplength(*(ln->opoff)),
						pflags(*(ln->opoff)));
			printf("\n");
		}
		printf("\n    %8s%8s%8s%8s%8s\n", "labnum", "labid", "minval", "maxval",
				"defined");
		for (i = 0, lbhead = *pstate.s_locl; i < LOCLABSIZE; lbhead++, i++)
		{
			if (lbhead->l_defined != EMPTY)
				printf("%4d\n", i);
			for (lbp = lbhead; lbp != lbp_cast 0; lbp = lbp->l_chain)
			{
				if (lbp->l_defined != EMPTY)
					printf("    %8d%8d%8d%8d  %-s\n",
							lbp->l_hinum * LOCLABSIZE + i,
							int_cast (intptr_t)lbp, lbp->l_min, lbp->l_max,
							labstr[(unsigned char)lbp->l_defined]);
			}
		}
	}
	if (((n == 0 || n >= 100) && d_flag) || (n <= 1 && d_flag >= 2))
	{
		if (n == 0 || n == 100)
		{
			printf("File %s", curfile);
			if (archmode)
				printf("(%.14s)", archhdr.ar_name);
			printf(" :\n\n");
		}
		printf("Local data labels:\n");
		printf("\n\t%8.8s  %8.8s  %8.8s\n", "g_name", "g_status", "g_addr");
		for (gb = mglobs, i = 0; gb < &mglobs[oursize->n_mlab]; gb++, i++)
			if (gb->g_name[0] != 0)
			{
				printf("%5d\t%8.6s", i, gb->g_name);
				printf("  %8o  %8ld\n", gb->g_status, gb->g_val.g_addr);
			}
		printf("\n\nGlobal data labels\n");
		printf("\n\t%8.8s  %8.8s  %8.8s\n", "g_name", "g_status", "g_addr");
		for (gb = xglobs, i = 0; gb < &xglobs[oursize->n_glab]; gb++, i++)
			if (gb->g_name != 0)
			{
				printf("%5d\t%8.6s", i, gb->g_name);
				printf("  %8o  %8ld\n", gb->g_status, gb->g_val.g_addr);
			}
		printf("\n\nLocal procedures\n");
		printf("\n\t%8.8s%8s%8s\t%8s%8s\n", "name", "status", "num", "off",
				"locals");
		for (pl = mprocs; pl < &mprocs[oursize->n_mproc]; pl++)
			if (pl->p_name)
			{
				printf("%4d\t%-8s%8o%8d", pl - mprocs, pl->p_name, pl->p_status,
						pl->p_num);
				if (pl->p_status & DEF)
					printf("\t%8ld%8ld", proctab[pl->p_num].pr_off,
							proctab[pl->p_num].pr_loc);
				printf("\n");
			}
		printf("\nGlobal procedures\n");
		printf("\n\t%8s%8s%8s\t%8s%8s\n", "name", "status", "num", "off",
				"locals");
		for (pl = xprocs; pl < &xprocs[oursize->n_xproc]; pl++)
			if (pl->p_name)
			{
				printf("%4d\t%-8s%8o%8d", pl - xprocs, pl->p_name, pl->p_status,
						pl->p_num);
				if (pl->p_status & DEF)
					printf("\t%8ld%8ld", proctab[pl->p_num].pr_off,
							proctab[pl->p_num].pr_loc);
				printf("\n");
			}
		if (r_flag)
		{
			register relc_t *rl;
			printf("\nData relocation\n");
			printf("\n\t%10s %10s %10s\n", "offset", "type", "value");
			for (rl = f_data; rl; rl = rl->r_next)
			{
				printf("\t%10ld %10s ", rl->r_off, r_data[rl->r_typ]);
				switch (rl->r_typ)
				{
				case RELADR:
				case RELHEAD:
					printf("%10ld\n", rl->r_val.rel_i);
					break;
				case RELGLO:
					printf("%8.8s\n", rl->r_val.rel_gp->g_name);
					break;
				case RELLOC:
					printf("%10d\n", rl->r_val.rel_lp);
					break;
				case RELNULL:
					printf("\n");
					break;
				}
			}
			printf("\n\nText relocation\n");
			printf("\n\t%10s %10s %10s\n", "offset", "flags", "value");
			for (rl = f_text; rl; rl = rl->r_next)
			{
				printf("\t%10ld %10s ", rl->r_off,
						pflags(opchoice[rl->r_typ & ~RELMNS]));
				if (rl->r_typ & RELMNS)
					printf("%10ld\n", rl->r_val.rel_i);
				else
					printf("\n");
			}
		}

	}
}
#endif
