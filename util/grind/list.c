/* $Header$ */

#include <stdio.h>
#include <alloc.h>

#include "position.h"
#include "idf.h"
#include "file.h"
#include "symbol.h"
#include "misc.h"

static	line_positions();
extern char	*dirs[];
extern FILE	*fopen();
extern FILE	*db_out;
extern t_lineno	currline;
extern int	interrupted;

static void
mk_filnm(dir, file, newname)
  char	*dir;
  char	*file;
  char	**newname;
{
  register char	*dst = Malloc((unsigned) (strlen(dir) + strlen(file) + 2));

  *newname = dst;
  if (*dir) {
	while (*dst++ = *dir++) /* nothing */;
	*(dst - 1) = '/';
  }
  while (*dst++ = *file++) /* nothing */;
}

static FILE *
open_file(fn, mode, ffn)
  char	*fn;
  char	*mode;
  char	**ffn;
{
  FILE	*f;
  char	**p;

  if (fn[0] == '/') {
	*ffn = fn;
	return fopen(fn, mode);
  }
  p = dirs;
  while (*p) {	
	mk_filnm(*p++, fn, ffn);
	if ((f = fopen(*ffn, mode)) != NULL) {
		return f;
	}
	free(*ffn);
  }
  return NULL;
}

lines(file, l1, l2)
  register p_file file;
  int		l1, l2;
{
  static p_file last_file;
  static FILE *last_f;
  register FILE	*f;
  register int	n;

  if (last_file != file) {
	if (last_f) fclose(last_f);
	last_f = 0;
  	if (!(f = open_file(file->f_sym->sy_idf->id_text, 
			    "r",
			    &file->f_fullname))) {
		error("could not open %s", file->f_sym->sy_idf->id_text);
		return;
	}
	last_file = file;
	last_f = f;
	if (! file->f_linepos) {
		line_positions(file, f);
	}
  }
  else f = last_f;

  if (l1 < 1) l1 = 1;
  if (l1 > file->f_nlines) l1 = file->f_nlines;
  if (l1+l2-1 > file->f_nlines) l2 = file->f_nlines - l1 + 1;

  fseek(f, *(file->f_linepos+(l1-1)), 0);
  for (n = l1; n < l1 + l2; n++) {
	register int	c;

	if (interrupted) return;
	fprintf(db_out, "%c%5d\t", currfile && file == currfile->sy_file && n == currline ? '>' : ' ', n);
	do {
		c = getc(f);
		if (c != EOF) putc(c, db_out);
	} while (c != '\n' && c != EOF);
	if (c == EOF) break;
  }
  clearerr(f);
}

static
line_positions(file, f)
  p_file	file;
  register FILE	*f;
{
  int		nl;
  unsigned int	n_alloc = 256;
  register long	cnt = 0;
  register int	c;

  file->f_linepos = (long *) Malloc(n_alloc * sizeof(long));
  file->f_linepos[0] = 0;
  nl = 1;
  while ((c = getc(f)) != EOF) {
	cnt++;
	if (c == '\n') {
		if (nl == n_alloc) {
			n_alloc <<= 1;
			file->f_linepos =
				(long *) Realloc((char *)(file->f_linepos),
						 n_alloc * sizeof(long));
		}
		file->f_linepos[nl++] = cnt;
	}
  }
  if (cnt == file->f_linepos[nl-1]) nl--;
  file->f_linepos = (long *) Realloc((char *)(file->f_linepos),
					(unsigned)nl * sizeof(long));
  file->f_nlines = nl;
  clearerr(f);
}
