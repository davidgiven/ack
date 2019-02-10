/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
#define RCS_ACK "$Id$"
#endif

#include <stdlib.h> /* size_t, free() */

/****************************************************************************/
/*                      User settable options                               */
/****************************************************************************/

#define FRONTENDS "fe" /* The front-end definitions */
#define TMPNAME "Ack_%x" /* Naming of temp. files */

/****************************************************************************/
/*         Internal mnemonics, should not be tinkered with                  */
/****************************************************************************/

/* The names of some string variables */

#define HOME "EM"
#define RTS "RTS"
#define HEAD "HEAD"
#define TAIL "TAIL"
#define SRC "SOURCE"
#define LIBVAR "LNAME"
#define SUFFIX "SUFFIX"

/* Intended for flags, possibly in bit fields */

#define YES 1
#define NO 0
#define MAYBE 2

#define SUFCHAR '.' /* Start of SUFFIX in file name */
#define SPACE ' '
#define TAB '\t'
#define EQUAL '='
#define S_VAR '{' /* Start of variable */
#define C_VAR '}' /* End of variable */
#define A_VAR '?' /* Variable alternative */
#define BSLASH '\\' /* Backslash */
#define STAR '*' /* STAR */
#define C_IN '<' /* Token specifying input */
#define C_OUT '>' /* Token specifying output */
#define S_EXPR '(' /* Start of expression */
#define C_EXPR ')' /* End of expression */
#define M_EXPR ':' /* Middle of two suffix lists */
#define T_EXPR '=' /* Start of tail */

#define NO_SCAN 0200 /* Bit set in character to defeat recogn. */

typedef struct
{
	char* p_path; /* points to the full pathname */
	int p_keeps : 1; /* The string should be thrown when unused */
	int p_keep : 1; /* The file should be thrown away after use */
} path;

#define p_cont(elem) ((path*)l_content(elem))

/* Own routines */

/* rmach.c */
extern void setlist(char*);

/* svars.c */
extern void setsvar(char*, char*);
extern void setpvar(char*, char* (*)(void));
extern char* getvar(const char*);

/* util.c */
extern char* ack_basename(const char*);
extern char* skipblank(char*);
extern char* firstblank(char*);
extern void fatal(const char*, ...);
extern void vprint(const char*, ...);
extern void fuerror(const char*, ...);
extern void werror(const char*, ...);
extern void quit(int);
extern char* keeps(const char*);
#define throws(str) free(str)
extern void* getcore(size_t);
extern void* changecore(void*, size_t);
#define freecore(area) free(area)

#define DEBUG 1 /* Allow debugging of Ack */

#ifndef DEBUG
#define debug 0 /* To surprise all these 'if ( debug ) 's */
#else
extern int debug;
#endif
