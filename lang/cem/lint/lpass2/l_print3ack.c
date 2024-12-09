/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

/*	A C K   P R I N T   M O D U L E   D E F I N T I O N S   */

#ifdef	lint

#include	<system.h>

/* LINTLIBRARY */

/* FORMAT0 $
	%[bdox] = int		%l[bdox] = long
	%c = int		%s = char *
	%u = unsigned int
$ */
printf(format) char *format; { ; }

/* FORMAT1 */
fprintf(filep, format) FILE* filep; char *format; { ; }
/* FORMAT1 */
sprintf(s, format) char *s; char *format; { ; }
/* FORMAT1 */
vfprintf(filep, format) FILE* filep; char *format; { ; }

#endif	/* lint */

