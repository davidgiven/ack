#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PATHLENGTH	1024	/* max. length of path to file		*/


#define	ERROUT		STDERR	/* file pointer for writing messages	*/
#define	MAXERR_LINE	5	/* maximum number of error messages given
					on the same input line.		*/


#define	IDFSIZE	64	/* maximum significant length of an identifier	*/


#define	NUMSIZE	256	/* maximum length of a numeric constant		*/


#define	NPARAMS 32	/* maximum number of parameters of macros	*/
#define	STDC_NPARAMS 31	/* ANSI limit on number of parameters		*/


#define	IFDEPTH	256	/* maximum number of nested if-constructions	*/


#define	LAPBUF	128	/* initial size of macro replacement buffer	*/
#define	ARGBUF	128	/* initial size of macro parameter buffer(s)	*/


#define ISTRSIZE	16	/* minimum number of bytes allocated for
					storing a string		*/


#if 0
#define BOTCH_FREE	1	/* botch freed memory, as a check	*/
#endif


#if 0
#define DEBUG		1	/* perform various self-tests		*/
#endif
#define NDEBUG		1	/* disable assertions			*/


#define PARBUFSIZE	1024


#define ITEXTSIZE	16	/* 1st piece of memory for repl. text	*/


#if 0
#define INP_READ_IN_ONE	1	/* read input file in one.  */
				/* If defined, we cannot read from a pipe */
#endif


#define OBUFSIZE	8192	/* output buffer size */


#define DOBITS		1	/* use trick to reduce symboltable accesses */


#define LINE_PREFIX	"#"	/* prefix for generated line directives,
				   either "#" or "#line"
				*/

#endif

