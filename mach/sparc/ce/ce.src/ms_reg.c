/* catch register messages. BEWARE: code uses plain printf's (fprint's)
 * to generate code. This is not compatible with the usual procedure
 * used in the EM_table
 */

#define CODE_EXPANDER
#include <em.h>
#include <em_reg.h>
#include <em_mes.h>
#include <stb.h>
#include "mach.h"
#include "push_pop.h"
#include <stdio.h>

#define MAX_NR_REGS 12
#define MAX_NR_FLTS MAX_NR_FLT_REGS	/* depends on using float or doubles */

#define RM_OFFSET 0
#define RM_SIZE 1
#define RM_TYPE 2
#define RM_COUNT 3

typedef struct reg_info {
	int offset;
	int size;	/* 4 or 8 */
	int pri;
	reg_t reg, reg2;	/* reg2 used for doubles only */
} reg_info;

static reg_info reg_dat[MAX_NR_REGS], flt_dat[MAX_NR_FLTS];

static int current_reg_mes[RM_COUNT+4];

static int in_reg_mes = 0;	/* boolean */
static int reg_mes_nr;
static int db_mes = 0;
static int db_str = 0;
static int db_nul = 0;		/* boolean */

static int worst_reg_pri, worst_flt_pri; /* reset by C_prolog (to some large number) */
static int nr_reg_vars, nr_flt_vars;		/* dito  (both to 0) */

init_reg_man()
{
  worst_reg_pri = worst_flt_pri = (unsigned)-1/2;
  nr_reg_vars = nr_flt_vars = 0;
}

static reg_t my_alloc_reg(pri,loc)
int pri, *loc;
{
	reg_t S1;
	int i;

	if ((S1 = alloc_reg_var()) == NULL)
		if (current_reg_mes[RM_COUNT] > worst_reg_pri) {
			for (i = 0; i < nr_reg_vars; i++)
				if (reg_dat[i].pri <= worst_reg_pri) {
					*loc = i;
					S1 = reg_dat[i].reg;
					break;
				}
			worst_reg_pri = (unsigned)-1/2;
			for (i = 0; i < nr_reg_vars; i++)
				if (reg_dat[i].pri <= worst_reg_pri)
					worst_reg_pri = reg_dat[i].pri;
		} else
			return NULL;	/* SORRY, FULL HOUSE! */
	else
		*loc = nr_reg_vars++;

	return S1;
}

static reg_t my_alloc_double(pri,loc,r2)
int pri, *loc;
reg_t *r2;
/* implementation note: my_alloc_double only reclaims other doubles
 * when a better candidate is given. It never reclaims floats, even if
 * the current double is a mich better candidate.
 */
{
	reg_t S1;
	int i;

	if ((S1 = alloc_double_var(r2)) == NULL)
		if (current_reg_mes[RM_COUNT] > worst_flt_pri) {
			for (i = 0; i < nr_flt_vars; i++)
				if (flt_dat[i].pri <= worst_flt_pri &&
						flt_dat[i].size == EM_DSIZE) {
					*loc = i;
					S1 = flt_dat[i].reg;
					*r2 = flt_dat[i].reg2;
					break;
				}
			worst_flt_pri = (unsigned)-1/2;
			for (i = 0; i < nr_flt_vars; i++)
				if (flt_dat[i].pri < worst_flt_pri)
					worst_flt_pri = flt_dat[i].pri;
		} else
			return NULL;	/* SORRY, FULL HOUSE! */
	else
		*loc = nr_flt_vars++;

	return S1;
}

static reg_t my_alloc_float(pri,loc)
int pri, *loc;
/* just as for my_alloc_double, my_alloc_float never reclaims a double,
 * even though this me be useful and easy. Sorry.
 */
{
	reg_t S1;
	int i;

	if ((S1 = alloc_float_var()) == NULL)
		if (current_reg_mes[RM_COUNT] > worst_flt_pri) {
			for (i = 0; i < nr_flt_vars; i++)
				if (flt_dat[i].pri <= worst_flt_pri &&
						flt_dat[i].size == EM_WSIZE) {
					*loc = i;
					S1 = flt_dat[i].reg;
					break;
				}
			worst_flt_pri = (unsigned)-1/2;
			for (i = 0; i < nr_flt_vars; i++)
				if (flt_dat[i].pri <= worst_flt_pri)
					worst_flt_pri = flt_dat[i].pri;
		} else
			return NULL;	/* SORRY, FULL HOUSE! */
	else
		*loc = nr_flt_vars++;

	return S1;
}

free_all_reg_vars()
{
  int i;

  for (i = 0; i < nr_reg_vars; i++)
	free_reg(reg_dat[i].reg);
  for (i = 0; i < nr_flt_vars; i++)
	switch (flt_dat[i].size) {
	case EM_WSIZE: free_reg(flt_dat[i].reg); break;
	case EM_DSIZE: free_double_reg(flt_dat[i].reg); break;
	}
  check_cache();
}

alloc_all_reg_vars()
{
  int i;

  for (i = 0; i < nr_reg_vars; i++)
	forced_alloc_reg(reg_dat[i].reg);
  for (i = 0; i < nr_flt_vars; i++)
	switch (flt_dat[i].size) {
	case EM_WSIZE: forced_alloc_reg(flt_dat[i].reg); break;
	case EM_DSIZE:
		forced_alloc_reg(flt_dat[i].reg);
		forced_alloc_reg(flt_dat[i].reg2);
		break;
	}
  check_cache();
}

static params_to_regs()		/* copy required parameters to registers */
{
  int i, j;

  for (i = 0; i < nr_reg_vars; i++)
	if (reg_dat[i].offset > 0)
		fprint(codefile, "ld	[%%l1+%d], %s\n",
			reg_dat[i].offset, reg_dat[i].reg);

  for (i = 0; i < nr_flt_vars; i++)
	if (flt_dat[i].offset > 0)
	{
		fprint(codefile, "ld	[%%l1+%d], %s\n",
		  flt_dat[i].offset, flt_dat[i].reg);
		if (flt_dat[i].size == EM_DSIZE)
			fprint(codefile, "ld	[%%l1+%d], %s\n",
			  flt_dat[i].offset + 4, flt_dat[i].reg2);
	}
}

static cmp_flt_dat(e1, e2)
reg_info *e1, *e2;
{
  return (e1->offset - e2->offset);
}

static save_float_regs()
{
  int i;
  int offset;

  qsort(flt_dat, nr_flt_vars, sizeof(flt_dat[0]), cmp_flt_dat);
  for (i = 0, offset= 0; i < nr_flt_vars; i++, offset += 8)
	if ((i+1 < nr_flt_vars &&
		flt_dat[i].offset == flt_dat[i+1].offset-4 &&
		flt_dat[i].size == EM_FSIZE &&
		flt_dat[i+1].size == EM_FSIZE)
		|| (flt_dat[i].size == EM_DSIZE)) {
		fprint(codefile, "std	%s, [%%fp + %d]\n",
				flt_dat[i].reg, FLTSAV_OFFSET + offset);
		if (flt_dat[i].size != EM_DSIZE)
			++i;
	} else
		fprint(codefile, "st	%s, [%%fp + %d]\n",
				flt_dat[i].reg, FLTSAV_OFFSET + offset);
}

load_float_regs()
{
  int i;
  int offset;

  for (i = 0, offset= 0; i < nr_flt_vars; i++, offset += 8)
	if ((i+1 < nr_flt_vars &&
		flt_dat[i].offset == flt_dat[i+1].offset-4 &&
		flt_dat[i].size == EM_FSIZE &&
		flt_dat[i+1].size == EM_FSIZE)
		|| (flt_dat[i].size == EM_DSIZE)) {
		fprint(codefile, "ldd	[%%fp + %d], %s\n",
				FLTSAV_OFFSET + offset, flt_dat[i].reg);
		if (flt_dat[i].size != EM_DSIZE)
			++i;
	} else
		fprint(codefile, "ld	[%%fp + %d], %s\n",
				FLTSAV_OFFSET + offset, flt_dat[i].reg);

}

C_mes_begin( ms)
int ms;
{
	reg_mes_nr = 0;
	in_reg_mes = (ms == ms_reg);
	if (ms == ms_gto)
		fprint(codefile, "ta	3\n");
	db_mes = (ms == ms_stb || ms == ms_std) ? ms : 0;
}

static dump_reg_tabs();

C_mes_end()
{
	int pos;
	reg_t S1, S2;

	if (db_mes) {
		db_nul = 0;
		if (db_mes == ms_std && db_str == 2) fprint(codefile,",1f\n1:\n");
		else fprint(codefile, "\n");
		db_str = 0;
		db_mes = 0;
	}
	if (!in_reg_mes)	/* end of some other mes */
		return;
	if (reg_mes_nr == 0) {	/* end of reg_mes's */
		save_float_regs();
		params_to_regs();
		if (debug)
			dump_reg_tabs(codefile);
		return;
	}
	if (current_reg_mes[RM_COUNT] == 0)	/* never used, so ignore */
		return;
	if (current_reg_mes[RM_OFFSET] >= 0)
		current_reg_mes[RM_OFFSET] += EM_BSIZE;
	if (debug)
		fprint(codefile, "\t\t! Got reg_mes: %d %d %d %d\n",
			current_reg_mes[0], current_reg_mes[1],
		current_reg_mes[2], current_reg_mes[3]);
	if (current_reg_mes[RM_TYPE] == reg_float) {
		switch(current_reg_mes[RM_SIZE]) {
		case EM_WSIZE :
			if ((S1 = my_alloc_float(current_reg_mes[RM_COUNT], &pos))
									== NULL)
				return;
			break;
		case EM_DSIZE:
			if ((S1 = my_alloc_double(current_reg_mes[RM_COUNT], &pos, &S2))
									== NULL)
				return;
			flt_dat[pos].reg2 = S2;
		default: break;
		}
		flt_dat[pos].offset = current_reg_mes[RM_OFFSET];
		flt_dat[pos].size = current_reg_mes[RM_SIZE];
		flt_dat[pos].pri = current_reg_mes[RM_COUNT];
		flt_dat[pos].reg = S1;
	} else {
		if (current_reg_mes[RM_SIZE] != EM_WSIZE)
			return;		/* IGNORE THESE */
		if ((S1 = my_alloc_reg(current_reg_mes[RM_COUNT], &pos)) == NULL)
			return;		/* SORRY, FULL HOUSE! */

		reg_dat[pos].offset = current_reg_mes[RM_OFFSET];
		reg_dat[pos].size = current_reg_mes[RM_SIZE];
		reg_dat[pos].pri = current_reg_mes[RM_COUNT];
		reg_dat[pos].reg = S1;
	}
}

C_cst( l)
arith l;
{
	if (db_mes) {
		if (! db_str) {
			switchseg( SEGTXT);
			if (l == N_SLINE) {
				flush_cache();
				fprintf(codefile, "call ___uX_LiB\nnop\n");
			}
			if (db_mes == ms_std) {
				fprint(codefile, ".stabd 0x%lx,0", (long) l);
			}
			else	fprint(codefile, ".stabn 0x%lx,0", (long) l);
			db_str = 1;
			db_nul = 1;
		}
		else	fprint(codefile, ",0x%lx", (long) l);
		if (! db_nul) {
			fprint(codefile, ",0");
			db_nul = 1;
		}
	}
	if (in_reg_mes)
		current_reg_mes[reg_mes_nr++] = l;
}

C_scon(s, l)
register char *s;
register arith l;
{
	if (db_mes) {
		fprint(codefile, ".stabs \"");
		while (--l) {
			register int c = *s++;

			if (isprint(c) && c != '"' && c != '\\')
				fprint(codefile, "%c", c);
			else
				fprint(codefile, "\\%03o", c);
		}
		fprint(codefile, "\"");
		db_str = 2;
	}
}

C_dlb(l, off)
label l;
arith off;
{
	if (db_mes) {
		fprint(codefile,",");
		fprint(codefile, DLB_FMT, (long) l);
		if (off) fprint(codefile,"+%ld", (long) off);
	}
}

C_dnam(l, off)
char *l;
arith off;
{
	if (db_mes) {
		fprint(codefile,",");
		fprint(codefile, DNAM_FMT, l);
		if (off) fprint(codefile,"+%ld", (long) off);
	}
}

extern int B_procno;

C_ilb(l)
label l;
{
	if (db_mes) {
		fprint(codefile,",");
		fprint(codefile, ILB_FMT, B_procno, (long)l);
	}
}

C_pnam(s)
char *s;
{
	if (db_mes) {
		fprint(codefile,",");
		fprint(codefile, NAME_FMT, s);
	}
}

static
dump_reg_tabs(stream)
FILE *stream;
{
  int i;

  fprint(stream, "!offset\tsize\tname (%d regvars)\n", nr_reg_vars);
  for (i = 0; i < nr_reg_vars; i++)
	fprint(stream, "! %d\t%d\t%s\n", reg_dat[i].offset, reg_dat[i].size,
			reg_dat[i].reg);

  fprint(stream, "!offset\tsize\tname (%d fltvars)\n", nr_flt_vars);
  for (i = 0; i < nr_flt_vars; i++)
	fprint(stream, "! %d\t%d\t%s\n", flt_dat[i].offset, flt_dat[i].size,
			flt_dat[i].reg);
}

reg_t find_local(offset, reg2)	/* WARNING: no size checking here! */
int offset;
reg_t *reg2;
{
  int i;

  if (reg2)
	*reg2 = NULL;
  for (i = 0; i < nr_reg_vars; i++)
	if (reg_dat[i].offset == offset)
		return reg_dat[i].reg;

  for (i = 0; i < nr_flt_vars; i++)
	if (flt_dat[i].offset == offset) {
		if (flt_dat[i].size == EM_DSIZE)
			if (reg2)
				*reg2 = flt_dat[i].reg2;
		return flt_dat[i].reg;
	}
  return NULL;
}
