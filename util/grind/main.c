#include <stdio.h>
#include <varargs.h>

#include "tokenname.h"
#include "position.h"
#include "file.h"
#include "symbol.h"
#include "scope.h"

static char	*usage = "Usage: %s [-d] [<ack.out>] [<a.out>]";
static char	*progname;
char		*AckObj;
char		*AObj;
char		*dirs[] = { "", 0 };
FILE		*db_out;
FILE		*db_in;
t_lineno	currline;
int		debug;
extern struct tokenname tkidf[];
extern char	*strindex();

main(argc, argv)
  char	*argv[];
{
  char	*p;

  db_out = stdout;
  db_in = stdin;
  progname = argv[0];
  while (p = strindex(progname, '/')) {
	progname = p + 1;
  }
  if (argv[1][0] == '-') {
	switch(argv[1][1]) {
	case 'd':
		debug++;
		break;
	default:
		fatal(usage, progname);
	}
	argv++;
	argc--;
  }
  if (argc > 3) {
	fatal(usage, progname);
  }
  AckObj = argv[1] ? argv[1] : "a.out";
  if (argc == 3) AObj = argv[2];
  init_idf();
  init_types();
  init_scope();
  init_languages();
  if (DbxRead(AckObj) && AObj == 0) AObj = AckObj;
  else if (AObj == 0) AObj = "a.out";
  reserve(tkidf);
  if (currfile) CurrentScope = currfile->sy_file->f_scope;
  if (! init_run()) {
	fatal("something wrong with file descriptors");
  }
  prompt();
  Commands();
  fputc( '\n', db_out);
  exit(0);
}

prompt()
{
  if (isatty(fileno(db_in))) {
	fprintf(db_out, "%s -> ", progname);
	fflush(db_out);
  }
}

/*VARARGS1*/
fatal(va_alist)
  va_dcl
{
  va_list ap;
  char *fmt;

  va_start(ap);
  {
	fmt = va_arg(ap, char *);
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
  }
  va_end(ap);
  abort();
  exit(1);
}

extern int errorgiven;

/*VARARGS1*/
error(va_alist)
  va_dcl
{
  va_list ap;
  char *fmt;

  va_start(ap);
  {
	fmt = va_arg(ap, char *);
	fprintf(stderr, "%s: ", progname);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
  }
  va_end(ap);
  errorgiven = 1;
}

rd_fatal()
{
  fatal("read error in %s", AckObj);
}

No_Mem()
{
  fatal("out of memory");
}
