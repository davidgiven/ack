/* $Header$ */

#include <stdio.h>
#include <varargs.h>
#include <signal.h>

#include "tokenname.h"
#include "position.h"
#include "file.h"
#include "symbol.h"
#include "scope.h"
#include "Lpars.h"
#include "type.h"

static char	*usage = "Usage: %s [<ack.out>] [<a.out>]";
char		*progname;
char		*AckObj;
char		*AObj;
char		*dirs[] = { "", 0 };
FILE		*db_out;
FILE		*db_in;
int		debug;
extern struct tokenname tkidf[];
extern char	*strindex();
extern void	signal_child();
extern void	init_del();
extern void	init_run();
extern int	eof_seen;
extern int	interrupted;

static struct tokenname shorts[] = {
	{LIST, "l"},
	{CONT, "c"},
	{STEP, "s"},
	{NEXT, "n"},
	{DELETE, "d"},
	{PRINT, "p"},
	{RESTORE, "r"},
	{TRACE, "t"},
	{WHERE, "w"},
	{ 0, 0}
};

main(argc, argv)
  char	*argv[];
{
  char	*p;

  db_out = stdout;
  db_in = stdin;
  progname = argv[0];
  init_del();
  while (p = strindex(progname, '/')) {
	progname = p + 1;
  }
  while (argv[1] && argv[1][0] == '-') {
	switch(argv[1][1]) {
	case 'v':
		debug++;
		break;
	case 'i':
		int_size = atoi(&argv[1][2]);
		break;
	case 's':
		short_size = atoi(&argv[1][2]);
		break;
	case 'l':
		long_size = atoi(&argv[1][2]);
		break;
	case 'f':
		float_size = atoi(&argv[1][2]);
		break;
	case 'd':
		double_size = atoi(&argv[1][2]);
		break;
	case 'p':
		pointer_size = atoi(&argv[1][2]);
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
  init_types();
  init_scope();
  init_languages();
  if (DbRead(AckObj) && AObj == 0) AObj = AckObj;
  else if (AObj == 0) AObj = "a.out";
  reserve(tkidf);
  reserve(shorts);
  init_run();
  prompt();
  Commands();
  signal_child(SIGKILL);
  if (eof_seen) putc('\n', db_out);
  exit(0);
}

prompt()
{
  if (isatty(fileno(db_in))) {
	fprintf(db_out, "-> ");
	fflush(db_out);
  }
}

/*VARARGS*/
fatal(va_alist)
  va_dcl
{
  va_list ap;
  char *fmt;

  va_start(ap);
  {
	fmt = va_arg(ap, char *);
	fprintf(db_out, "%s: ", progname);
	vfprintf(db_out, fmt, ap);
	fprintf(db_out, "\n");
  }
  va_end(ap);
  exit(1);
}

extern int errorgiven;

/*VARARGS*/
error(va_alist)
  va_dcl
{
  va_list ap;
  char *fmt;

  if (! interrupted) {
  	va_start(ap);
  	{
		fmt = va_arg(ap, char *);
		fprintf(db_out, "%s: ", progname);
		vfprintf(db_out, fmt, ap);
		fprintf(db_out, "\n");
  	}
  	va_end(ap);
  }
  errorgiven = 1;
}

/*VARARGS*/
warning(va_alist)
  va_dcl
{
  va_list ap;
  char *fmt;

  va_start(ap);
  {
	fmt = va_arg(ap, char *);
	fprintf(db_out, "%s: ", progname);
	vfprintf(db_out, fmt, ap);
	fprintf(db_out, "\n");
  }
  va_end(ap);
}

rd_fatal()
{
  fatal("read error in %s", AckObj);
}

No_Mem()
{
  fatal("out of memory");
}
