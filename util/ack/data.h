/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
#define RCS_DATA "$Id$"
#endif

EXTERN  char            *stopsuffix;    /* Suffix to stop at */
EXTERN  char            *machine;       /* The machine id */
EXTERN	char		*callname;	/* argv[0] */
EXTERN  char            *rts;           /* The runtime-system */
EXTERN  char            *rtsuf;         /* The runtime-system module suffix */
EXTERN	char		*Optlist;	/* Which optimizers */

EXTERN  list_head       arguments;      /* List of arguments */
EXTERN  list_head       flags;          /* List of flags */

EXTERN  list_head       tr_list;        /* List of transformations */

EXTERN  list_head       R_list;         /* List of -R flags */
EXTERN  list_head       head_list;      /* List of suffices for headers */
EXTERN  list_head       tail_list;      /* List of suffices for tails */

EXTERN  int             k_flag;         /* Like -k of lint */
EXTERN  int             t_flag;         /* Preserve intermediate files */
EXTERN  int             v_flag;         /* Verbose */
EXTERN  int             w_flag;         /* Don't print warnings */
EXTERN  int             nill_flag;      /* Don't print file names */
EXTERN  int             Optlevel;       /* Optimizing */

#ifdef DEBUG
EXTERN  int             debug;          /* Debugging control */
#endif

EXTERN  int             n_error;        /* Number of errors encountered */

EXTERN  char            *progname;      /* The program call name */

EXTERN  char            *outfile;       /* The result file e.g. a.out */
EXTERN  char            template[20];   /* The template for temporary file
						names */

EXTERN  trf             *linker;	/* Pointer to the Loader/Linker */
EXTERN  trf             *cpp_trafo;     /* Pointer to C-preprocessor */

EXTERN  path            in;             /* The current single input pathname */
EXTERN  path            out;            /* The current output pathname */
EXTERN  path            orig;           /* The original input path */
EXTERN  char            *p_basename;    /* The current basename */
EXTERN  const char      *p_suffix;      /* The current input suffix */
