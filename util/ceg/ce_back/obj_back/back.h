/* This file must be included in all the files which use the backlibrary.
 */

extern char *extnd_name(), *extnd_dnam(), *extnd_dlb(), *extnd_ilb(),
	    *extnd_hol(), *extnd_ext(), *extnd_pro(), *extnd_start(),
	    *extnd_part(), *extnd_cont();
extern holno, procno;

#include "data.h"

/* These routines are called very often, thus we turned them into macros. */

#define text1(b)	{if (--_text_cnt < 0) mem_text(); *text++ = b;}
#define con1(b)		{if (--_data_cnt < 0) mem_data(); *data++ = b;}
#define rom1(b)		{if (--_data_cnt < 0) mem_data(); *data++ = b;}
#define bss( n)		( nbss += n)


/* Numbering of the segments and some global constants */

#define 	SEGTXT		0
#define 	SEGROM		1
#define 	SEGCON		2
#define 	SEGBSS		3
#define 	SEGHOL		-1	/* does not exist */

#define swtxt()		switchseg( SEGTXT)

#define switchseg(seg)	if ((seg) != cur_seg) swtchsg(seg); else

#define 	PC_REL  	1
#define 	ABSOLUTE 	!PC_REL


/* Initialize values. */

#define MAXTEXT		4096
#define MAXDATA		2048
#define	MAXRELO		512
#define	MAXNAME		512
#define	MAXSTRING	4096

