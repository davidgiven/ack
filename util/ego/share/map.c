/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* M A P . C */

#include "types.h"
#include "map.h"

short	plength;
short	olength;
short	llength;
short	blength;
short	lplength;
line_p	*lmap;
bblock_p *lbmap;
proc_p	*pmap	;	/* dynamically allocated array that maps
			 * every proc_id to a proc_p.
			 */
obj_p	*omap;		/* maps obj_id to obj_p */
loop_p	*lpmap;		/* maps loop_id to loop_p */
bblock_p *bmap;		/* maps block_id to bblock_p */

dblock_p	fdblock;	/* first dblock */
proc_p		fproc;		/* first proc   */
