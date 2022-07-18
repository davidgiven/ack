/*
 Gathering run-time statistics
 */

/* $Id$ */

#include	<stdio.h>

#include	"global.h"
#include	"linfil.h"
#include	"alloc.h"
#include	"whatever.h"

struct line_tally
{ /* one for each line */
	long lt_cnt; /* counts entrances */
	long lt_instr; /* counts instructions */
};

struct file_tally
{ /* one for each file */
	struct file_tally *next;
	ptr ft_fil; /* file name */
	long ft_limit; /* size of line array */
	struct line_tally *ft_line; /* pointer to line array */
};

PRIVATE struct file_tally *first_tally; /* start of chain */
PRIVATE struct file_tally *file; /* present file */

PRIVATE long lastLIN;

PRIVATE FILE *tally_fp;

/* Forward declarations. */
PRIVATE void tally_newFIL(ptr);
PRIVATE void enlarge(struct file_tally *, long);

void tally(void)
{
	if (!FIL)
		return;

	if (!file || FIL != file->ft_fil)
	{
		tally_newFIL(FIL);
		file->ft_fil = FIL;
		lastLIN = -1;
	}
	if (LIN != lastLIN)
	{
		if (LIN >= file->ft_limit)
		{
			enlarge(file, LIN);
		}
		file->ft_line[LIN].lt_cnt++;
		lastLIN = LIN;
	}
	file->ft_line[LIN].lt_instr++;
}

PRIVATE void tally_newFIL(ptr f)
{
	struct file_tally **hook = &first_tally;

	while (*hook)
	{
		if ((*hook)->ft_fil == f)
			break;
		hook = &(*hook)->next;
	}
	if (!*hook)
	{
		/* first time we see this file */
		/* construct a new entry */
		struct file_tally *nt = (struct file_tally *) Malloc(
				(size) sizeof(struct file_tally), "file_tally");

		nt->next = (struct file_tally *) 0;
		nt->ft_fil = f;
		nt->ft_limit = 1; /* provisional length */
		nt->ft_line = (struct line_tally *) Malloc(
				(size) sizeof(struct line_tally), "struct line_tally");
		nt->ft_line[0].lt_cnt = 0;
		nt->ft_line[0].lt_instr = 0;

		/* and hook it in */
		*hook = nt;
	}
	file = *hook;
}

PRIVATE void enlarge(struct file_tally *ft, long l)
{
	long limit = allocfrac(l < 100 ? 100 : l);

	if (limit <= ft->ft_limit)
		return;
	ft->ft_line = (struct line_tally *) Realloc((char *) ft->ft_line,
			(size) (limit * sizeof(struct line_tally)), "array line_tally");
	while (ft->ft_limit < limit)
	{
		ft->ft_line[ft->ft_limit].lt_cnt = 0;
		ft->ft_line[ft->ft_limit].lt_instr = 0;
		ft->ft_limit++;
	}
}



void out_tally(void)
{
	struct file_tally **hook = &first_tally;

	if (!*hook)
		return;

	tally_fp = fopen("int.tally", "wb");
	if (!tally_fp)
		return;

	while (*hook)
	{
		struct file_tally *ft = *hook;
		register long i;

		fprintf(tally_fp, "%s:\n", dt_fname(ft->ft_fil));
		for (i = 0; i < ft->ft_limit; i++)
		{
			struct line_tally *lt = &ft->ft_line[i];

			if (lt->lt_cnt)
			{
				/* we visited this line */
				fprintf(tally_fp, "\t%ld\t%ld\t%ld\n", i, lt->lt_cnt,
						lt->lt_instr);
			}
		}
		fprintf(tally_fp, "\n");
		hook = &(*hook)->next;
	}

	fclose(tally_fp);
	tally_fp = 0;
}

