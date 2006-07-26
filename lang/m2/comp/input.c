/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* I N S T A N T I A T I O N   O F   I N P U T   P A C K A G E */

/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include	"f_info.h"
struct f_info	file_info;
#include	"input.h"
#include	<inp_pkg.body>


AtEoIF()
{
	/*	Make the unstacking of input streams noticable to the
	   	lexical analyzer
	*/
	if (WorkingDir[0] != '\0') free(WorkingDir);
	return 1;
}

AtEoIT()
{
	/*	Make the end of the text noticable
	*/
	return 1;
}
