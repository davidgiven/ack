/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/***************************************************************/
/*                                                             */
/*   Definition for table that maps a name on an intable index */
/*                                                             */
/***************************************************************/

#ifndef NORCSID
#define RCS_DMACH "$Id$"
#endif


typedef struct {
	char *ma_name ;         /* The name of the machine */
	int   ma_index ;
} dmach ;

extern dmach massoc[] ;

extern char  intable[] ;
