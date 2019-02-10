/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef DATA_H
#define DATA_H

extern char* stopsuffix; /* Suffix to stop at */
extern char* machine; /* The machine id */
extern char* callname; /* argv[0] */
extern char* rts; /* The runtime-system */
extern char* rtsuf; /* The runtime-system module suffix */
extern char* Optlist; /* Which optimizers */

extern list_head arguments; /* List of arguments */
extern list_head flags; /* List of flags */

extern list_head tr_list; /* List of transformations */

extern list_head R_list; /* List of -R flags */
extern list_head head_list; /* List of suffices for headers */
extern list_head tail_list; /* List of suffices for tails */

extern int k_flag; /* Like -k of lint */
extern int t_flag; /* Preserve intermediate files */
extern int v_flag; /* Verbose */
extern int w_flag; /* Don't print warnings */
extern int nill_flag; /* Don't print file names */
extern int Optlevel; /* Optimizing */

#ifdef DEBUG
extern int debug; /* Debugging control */
#endif

extern int n_error; /* Number of errors encountered */

extern char* progname; /* The program call name */

extern char* outfile; /* The result file e.g. a.out */
extern char template[20]; /* The template for temporary file
          names */

extern trf* linker; /* Pointer to the Loader/Linker */
extern trf* cpp_trafo; /* Pointer to C-preprocessor */

extern path in; /* The current single input pathname */
extern path out; /* The current output pathname */
extern path orig; /* The original input path */
extern char* p_basename; /* The current basename */
extern const char* p_suffix; /* The current input suffix */

#endif
