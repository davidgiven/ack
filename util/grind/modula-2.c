/* $Header$ */

/* Language dependant support; this one is for Modula-2 */

#include <stdio.h>

#include "langdep.h"

extern FILE *db_out;

static int
	print_string();

static long
	array_elsize();

static struct langdep m2 = {
	"%ld",
	"%loB",
	"%lXH",
	"%lu",
	"%lXH",
	"%g",
	"%oC",

	"[",
	"]",
	"(",
	")",
	"{",
	"}",

	print_string,
	array_elsize
};

struct langdep *m2_dep = &m2;

static int
print_string(s)
  char	*s;
{
  register char	*str = s;
  int delim = '\'';

  while (*str) {
	if (*str++ == '\'') delim = '"';
  }
  fprintf(db_out, "%c%s%c", delim, s, delim);
}

extern long	int_size;

static long
array_elsize(size)
  long	size;
{
  if (! (int_size % size)) return size;
  if (! (size % int_size)) return size;
  return ((size + int_size - 1) / int_size) * int_size;
}
