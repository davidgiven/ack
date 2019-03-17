/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include		<stddef.h>
#include        "ass00.h"
#include        "assex.h"
#include		"assci.h"
#include		"asscm.h"
#include        "ip_spec.h"

short opt_line; /* max_line_no - # lines removed from end
 after perfoming exc's.
 Used to estimate the distance in # of
 instructions.
 */

/* Forward declarations. */
static int valid(register line_t *);
static char *findfit(int, cons_t);
static char *findnop(int);

/*
 ** Determine the exact instruction length & format where possible, and the
 ** the upper and lower limits otherwise. Enter limits in labeltable
 */
void pass_3(void)
{
	register line_t *lnp, *rev_lnp;
	line_t *tmp_lnp;
	locl_t *lbp;
	int min_l, max_l, min_bytes;
	short last_line;
	short hol_err_line;
	register int insno;

	pass = 3;
	opt_line = line_num;
	hol_err_line = 0;
	min_bytes = max_bytes = 0;
	rev_lnp = lnp_cast 0;
	for (lnp = pstate.s_fline; lnp; opt_line--, line_num--)
	{
		pstate.s_fline = lnp;
		insno = ctrunc(lnp->instr_num);
		switch (insno)
		{
		case sp_fpseu:
			last_line = line_num;
			line_num = lnp->ad.ad_ln.ln_first;
			opt_line -= lnp->ad.ad_ln.ln_extra;
			lnp->ad.ad_ln.ln_first = last_line;
			break;
		case sp_ilb1:
			lbp = lnp->ad.ad_lp;
			lbp->l_defined = SEEN;
			lbp->l_min = min_bytes;
			lbp->l_max = max_bytes;
			break;
		default:
			if (lnp->type1 == CONST && (em_flag[insno] & EM_PAR) == PAR_G)
			{
				if (holbase != 0)
				{
					if (lnp->ad.ad_i >= holsize)
					{
						hol_err_line = line_num;
					}
					lnp->ad.ad_i += holbase;
				}
			}
			else if (lnp->type1 >= VALLOW && (em_flag[insno] & EM_PAR) == PAR_G)
			{
				if (holbase != 0)
				{
					pstate.s_fline = lnp->l_next;
					newline(CONST);
					pstate.s_fline->instr_num = insno;
					pstate.s_fline->ad.ad_i =
					VAL1(lnp->type1) + holbase;
					freearea((area_t) lnp, (unsigned) linesize[VALLOW]);
					lnp = pstate.s_fline;
					if ( VAL1(lnp->type1) >= holsize)
					{
						hol_err_line = line_num;
					}
				}
			}
			if (!valid(lnp))
				fatal("Invalid operand");

			determine_props(lnp, &min_l, &max_l);
			min_bytes += min_l;
			max_bytes += max_l;
			break;
		}
		tmp_lnp = lnp->l_next;
		lnp->l_next = rev_lnp;
		rev_lnp = lnp;
		lnp = tmp_lnp;
	}
	pstate.s_fline = rev_lnp;
	if (hol_err_line)
	{
		line_num = hol_err_line;
		werror("address exceeds holsize");
	}
}

int oplength(int flag)
{
	int cnt;

	cnt = 1;
	if (flag & OPESC)
		cnt++;
	switch (flag & OPTYPE)
	{
	case OPNO:
	case OPMINI:
		break;
	case OP8:
	case OPSHORT:
		cnt++;
		break;
	case OP16U:
	case OP16:
		cnt += 2;
		break;
	case OP32:
		cnt += 5;
		break;
	case OP64:
		cnt += 9;
		break;
	}
	return cnt;
}

/*
 ** Determine the format that should be used for each instruction,
 ** depending on its offsets
 */

void determine_props(line_t *lnp, int *min_len, int *max_len)
{
	cons_t val;
	register int insno;
	register char *f_off, *l_off;
	char defined;

	insno = ctrunc(lnp->instr_num);
	val = parval(lnp, &defined);
	if (!defined)
	{
		switch (em_flag[insno] & EM_PAR)
		{
		case PAR_NO:
		case PAR_W:
			f_off = findnop(insno);
			break;
		case PAR_G:
			/* We want the maximum address that is a multiple
			 of the wordsize.
			 Assumption: there is no shortie for
			 intr max_word_multiple
			 where intr is a instruction allowing parameters
			 that are not a word multiple (PAR_G).
			 */
			f_off = findfit(insno, maxadr & (~(wordsize - 1)));
			break;
		case PAR_B:
			f_off = findfit(insno, (cons_t) 0);
			l_off = findfit(insno, val);
			if (f_off != l_off)
			{
				*min_len = oplength(*f_off);
				*max_len = oplength(*l_off);
				lnp->opoff = NO_OFF;
				return;
			}
			break;
		}
	}
	else
	{
		f_off = findfit(insno, val);
	}
	lnp->opoff = f_off;
	*min_len = *max_len = oplength(*f_off);
}

static char *findfit(int instr, cons_t val)
{
	register char *currc, *endc;
	int found, flags, number;
	char *opc;

	endc = opindex[instr + 1];
	for (currc = opindex[instr], found = 0; !found && currc < endc; currc++)
	{
		opc = currc;
		flags = ctrunc(*currc++);
		switch (flags & OPTYPE)
		{
		case OPNO:
			continue;
		case OPMINI:
		case OPSHORT:
			number = ctrunc(*++currc);
		}
		found = opfit(flags, number, val, em_flag[instr] & EM_PAR);
	}
	if (!found)
		fatal("Cannot find interpreter opcode");
	return opc;
}

static char* findnop(int instr)
{
	register char *currc, *endc;

	endc = opindex[instr + 1];
	for (currc = opindex[instr]; currc < endc; currc++)
	{
		switch ( ctrunc(*currc) & OPTYPE)
		{
		case OPNO:
			return currc;
		case OPSHORT:
		case OPMINI:
			currc++;
		}
		currc++;
	}
	fatal("Cannot find interpreter opcode");
	/* NOTREACHED */
	return NULL;
}

int opfit(int flag, int number, cons_t val, int i_flag)
{
	/* Number is invalid if flag does not contain MINI or SHORT */
	switch (flag & OPRANGE)
	{
	case OP_POS:
		if (val < 0)
			return 0;
		break;
	case OP_NEG:
		if (val >= 0)
			return 0;
		break;
	}
	if (flag & OPWORD)
	{
		if (val % wordsize)
			return 0;
		val /= wordsize;
	}
	if (flag & OPNZ)
	{
		if (val == 0)
			return 0;
		val--;
	}
	switch (flag & OPTYPE)
	{
	case OPMINI:
		if (val < 0)
			val = -1 - val;
		return val >= 0 && val < number;
	case OPSHORT:
		if (val < 0)
			val = -1 - val;
		return val >= 0 && val < number * 256;
	case OP16U:
		return val >= 0 && val <= 65535L && (i_flag != PAR_G || val <= maxadr);
	case OP16:
		return val >= -32768 && val <= 32767;
	case OP32:
		return TRUE;
	default:
		fatal("illegal OPTYPE value");
		return -1;
		/* NOTREACHED */
	}
}

/*
 ** return estimation of value of parameter
 */
cons_t parval(line_t *lnp, char *defined)
{
	register int type;
	register locl_t *lbp;
	register glob_t *gbp;
	cons_t offs;

	*defined = TRUE;
	type = lnp->type1;
	switch (type)
	{
	default:
		if (type >= VALLOW && type <= VALHIGH)
			return VAL1(type);
		error("bad type during parval");
		break;
	case CONST:
		return (lnp->ad.ad_i);
	case GLOSYM:
	case GLOOFF:
		if (type != GLOOFF)
		{
			gbp = lnp->ad.ad_gp;
			offs = 0;
		}
		else
		{
			gbp = lnp->ad.ad_df.df_gp;
			offs = lnp->ad.ad_df.df_i;
		}
		if (gbp->g_status & DEF)
			return (gbp->g_val.g_addr + offs);
		else
		{
			*defined = FALSE;
			return offs;
		}
	case LOCSYM:
		lbp = lnp->ad.ad_lp;
		switch (pass)
		{
		default:
			error("bad pass in parval");
		case 3:
			*defined = FALSE;
			switch (lbp->l_defined)
			{
			default:
				fatal("Illegal local label");
			case NO:
				error("Undefined local label");
				lbp->l_defined = NOTPRESENT;
			case NOTPRESENT:
				return max_bytes;
			case SEEN:
				return max_bytes - lbp->l_min;
			case YES:
				/* l_min contains line_num
				 adjusted for exc's.
				 */
				return (lbp->l_min - opt_line - 1) * maxinsl;
			}
		case 4:
			if (lbp->l_defined == YES)
				return (lbp->l_min - prog_size - maxinsl);
			return max_bytes - lbp->l_max - prog_size;
		case 5:
			if (lbp->l_defined == YES)
				return lbp->l_min;
			*defined = FALSE;
			break;
		}
		break;
	case MISSING:
		*defined = FALSE;
		break;
	case PROCNAME:
		return (lnp->ad.ad_pp->p_num);
	}
	return (0);
}

static int valid(register line_t *lnp)
{
	cons_t val;
	int type;

	type = lnp->type1;
	if (type >= VALLOW && type <= VALHIGH)
	{
		val = VAL1(type);
		type = CONST;
	}
	else if (type == CONST)
		val = lnp->ad.ad_i;
	switch (em_flag[ctrunc(lnp->instr_num)] & EM_PAR)
	{
	case PAR_NO:
		return type == MISSING;
	case PAR_C:
		if (type != CONST)
			return FALSE;
		if (val > maxint && val <= maxunsig)
		{
			lnp->ad.ad_i = val - maxunsig - 1;
		}
		return TRUE;
	case PAR_D:
		if (type != CONST)
			return FALSE;
		if (val > maxdint && val <= maxdunsig)
		{
			lnp->ad.ad_i = val - maxdunsig - 1;
		}
		return TRUE;
	case PAR_L:
	case PAR_F:
		return type == CONST;
	case PAR_N:
		return type == CONST && val >= 0;
	case PAR_G:
		return type == CONST || type == GLOSYM || type == GLOOFF;
	case PAR_W:
		if (type == MISSING)
			return TRUE;
	case PAR_S:
		return type == CONST && val > 0 && val % wordsize == 0;
	case PAR_Z:
		return type == CONST && val >= 0 && val % wordsize == 0;
	case PAR_O:
		return type == CONST && val >= 0
				&& (val >= wordsize ? val % wordsize : wordsize % val) == 0;
	case PAR_P:
		return type == PROCNAME;
	case PAR_B:
		return type == LOCSYM;
	case PAR_R:
		return type == CONST && val >= 0 && val <= 3;
	default:
		fatal("Unknown parameter type");
		return -1;
		/* NOTREACHED */
	}
}
