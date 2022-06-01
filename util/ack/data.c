/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "ack.h"
#include "list.h"
#include "trans.h"
#include "data.h"

char* stopsuffix = NULL; /* Suffix to stop at */
char* machine = NULL; /* The machine id */
char* callname = NULL; /* argv[0] */
char* rts = NULL; /* The runtime-system */
char* rtsuf = NULL; /* The runtime-system module suffix */
char* Optlist = NULL; /* Which optimizers */

list_head arguments = {}; /* List of arguments */
list_head flags = {}; /* List of flags */

list_head tr_list = {}; /* List of transformations */

list_head R_list = {}; /* List of -R flags */
list_head head_list = {}; /* List of suffices for headers */
list_head tail_list = {}; /* List of suffices for tails */

int k_flag = 0; /* Like -k of lint */
int t_flag = 0; /* Preserve intermediate files */
int v_flag = 0; /* Verbose */
int w_flag = 0; /* Don't print warnings */
int nill_flag = 0; /* Don't print file names */
int Optlevel = 0; /* Optimizing */

#ifdef DEBUG
int debug = 0; /* Debugging control */
#endif

int n_error = 0; /* Number of errors encountered */

char* progname = NULL; /* The program call name */

char* outfile = NULL; /* The result file e.g. a.out */

trf* linker = NULL; /* Pointer to the Loader/Linker */
trf* cpp_trafo = NULL; /* Pointer to C-preprocessor */

path in = {}; /* The current single input pathname */
path out = {}; /* The current output pathname */
path orig = {}; /* The original input path */
char* p_basename = NULL; /* The current basename */
const char* p_suffix = NULL; /* The current input suffix */
