/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S O M E   G L O B A L   V A R I A B L E S */

/* $Header$ */

extern char options[];	/* indicating which options were given */

extern char **DEFPATH;	/* search path for DEFINITION MODULE's */
extern int mDEF, nDEF;
extern int state;	/* either IMPLEMENTATION or PROGRAM */
extern struct file_list *CurrentArg;
