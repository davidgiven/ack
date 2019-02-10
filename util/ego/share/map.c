/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* M A P . C */

#include <stdlib.h>
#include "types.h"
#include "map.h"

short plength = 0;
short olength = 0;
short llength = 0;
short blength = 0;
short lplength = 0;
line_p* lmap = NULL;
bblock_p* lbmap = NULL;
proc_p* pmap = NULL; /* dynamically allocated array that maps
                      * every proc_id to a proc_p.
                      */
obj_p* omap = NULL; /* maps obj_id to obj_p */
loop_p* lpmap = NULL; /* maps loop_id to loop_p */
bblock_p* bmap = NULL; /* maps block_id to bblock_p */

dblock_p fdblock = NULL; /* first dblock */
proc_p fproc = NULL; /* first proc   */
