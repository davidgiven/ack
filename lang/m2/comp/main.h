/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* S O M E   G L O B A L   V A R I A B L E S */

/* $Id$ */

extern char options[];	/* indicating which options were given */

extern int DefinitionModule;
			/* flag indicating that we are reading a definition
			   module
			*/

extern struct def *Defined;
			/* definition structure of module defined in this
			   compilation
			*/
extern char **DEFPATH;	/* search path for DEFINITION MODULE's */
extern int mDEF, nDEF;
extern int state;	/* either IMPLEMENTATION or PROGRAM */
