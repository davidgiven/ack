/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

extern int LINTLIB;			/* file is lint library */
extern int s_NOTREACHED;		/* statement not reached */
extern int f_VARARGSn;			/* function with variable # of args */
extern int f_ARGSUSED;			/* function does not use all args */
extern int f_FORMATn;			/* argument f_FORMATn is f_FORMAT */
extern char *f_FORMAT;
extern int f_FORMATvar;			/* but the formal argument may be
					   absent because of varargs.h */

