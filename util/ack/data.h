#ifndef NORCSID
#define RCS_DATA "$Header$"
#endif

EXTERN  char            *stopsuffix;    /* Suffix to stop at */
EXTERN  char            *machine;       /* The machine id */
EXTERN  char            *rts;           /* The runtime-system id */

EXTERN  list_head       arguments;      /* List of arguments */
EXTERN  list_head       flags;          /* List of flags */

EXTERN  list_head       c_arguments;    /* List of linker arguments */

EXTERN  list_head       tr_list;        /* List of transformations */

EXTERN  list_head       R_list;         /* List of -R flags */
EXTERN  list_head       head_list;      /* List of suffices for headers */
EXTERN  list_head       tail_list;      /* List of suffices for tails */

EXTERN  int             k_flag;         /* Like -k of lint */
EXTERN  int             g_flag;         /* do_run() */
EXTERN  int             t_flag;         /* Preserve intermediate files */
EXTERN  int             v_flag;         /* Verbose */
EXTERN  int             w_flag;         /* Don't print warnings */
EXTERN  int             nill_flag;      /* Don't file names */
EXTERN  int             Optflag;        /* Optimizing */

#ifdef DEBUG
EXTERN  int             debug;          /* Debugging control */
#endif

EXTERN  int             n_error;        /* Number of errors encountered */

EXTERN  char            *progname;      /* The program call name */

EXTERN  char            *outfile;       /* The result file e.g. a.out */
EXTERN  char            *template;      /* The template for temporary file
						names */

EXTERN  trf             *combiner;      /* Pointer to the Loader/Linker */
EXTERN  trf             *cpp_trafo;     /* Pointer to C-preprocessor */

EXTERN  path            in;             /* The current input pathname */
EXTERN  path            out;            /* The current output pathname */
EXTERN  path            orig;           /* The original input path */
EXTERN  char            *p_basename;    /* The current basename */
EXTERN  char            *p_suffix;      /* The current input suffix */
