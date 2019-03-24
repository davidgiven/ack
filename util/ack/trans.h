/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
#define RCS_TRANS "$Id$"
#endif

/* This structure is the center of all actions */
/* It contains the description of all phases,
   the suffices they consume and produce and various properties */

typedef struct transform trf;

struct transform {
	char    *t_in ;         /* Suffices in '.o.k' */
	char    *t_out ;        /* Result '.suffix' */
	char	*t_outfile ;	/* Resulting output file */
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
	int     t_isprep:1 ;    /* Is preprocessor */
	int     t_keep:1 ;      /* Keep the output file */
	int     t_scan:1 ;      /* Used while finding path's */
	int	t_bscan:1 ;	/* Best scan so far, while finding path's */
	int	t_linker:1 ;	/* The linker usurps all unrecognized flags */
	int	t_do:1 ;	/* Is in a path to execute */
	int	t_blocked:1 ;	/* An input file could not be produced */
	short   t_optim ;       /* Is optimizer, + optimizer level */
	short	t_priority ;	/* Importance of including phase in scan */
	list_head t_inputs ;	/* The input 'path's of a combiner */
	char	*t_origname ;	/* The basename of the output file */
	trf	*t_next ;	/* The transformation to be executed next */
	char    *t_prog ;       /* Pathname for load file */
	list_head t_flags ;     /* List of flags */
	list_head t_args ;      /* List of arguments */
} ;

#define t_cont(elem) ((trf *)l_content(elem))

/* files.c */
int setfiles(trf *);
void disc_files(trf *);
void disc_inputs(trf *);
void rmfile(path *);
void rmtemps(void);
void add_input(path *, trf *);

/* run.c */
int runphase(trf *);

/* scan.c */
enum f_path { F_TRANSFORM, F_NOMATCH, F_NOPATH } ;
enum f_path getpath(trf **);

/* trans.c */
int transform(trf *);
void getmapflags(trf *);
void add_head(const char *);
void add_tail(const char *);
void transini(void);
void doassign(const char *, const char *, int);
