#ifndef NORCSID
#define RCS_TRANS "$Header$"
#endif

/* This structure is the center of all actions */
/* It contains the description of all phases,
   the suffices they consume and produce and various properties */

typedef struct transform trf;

struct transform {
	char    *t_in ;         /* Suffices in '.o.k' */
	char    *t_out ;        /* Result '.suffix' or 'name' */
	char    *t_name ;       /* The name of this transformation */
	list_head t_mapf ;      /* Mapflags argument, uses varrep */
	char    *t_argd ;       /* Argument descriptor, uses varrep */
	char    *t_needed ;     /* Suffix indicating the libraries needed */
	char    *t_rts ;        /* Suffix indicating the major language used*/
	int     t_stdin:1 ;     /* The input is taken on stdin */
	int     t_stdout:1 ;    /* The output comes on stdout */
	int     t_combine:1 ;   /* Transform several files to one result */
	int     t_visited:1 ;   /* NO before setup, YES after */
	int     t_prep:2 ;      /* Needs preprocessor YES/NO/MAYBE */
	int     t_optim:1 ;     /* Is optimizer */
	int     t_isprep:1 ;    /* Is preprocessor */
	int     t_keep:1 ;      /* Keep the output file */
	char    *t_prog ;       /* Pathname for load file */
	list_head t_flags ;     /* List of flags */
	list_head t_args ;      /* List of arguments */
	int     t_scan:1 ;      /* Used while finding path's */
	int     t_do:1 ;        /* Is in path to execute */
} ;

#define t_cont(elem) ((trf *)l_content(elem))
