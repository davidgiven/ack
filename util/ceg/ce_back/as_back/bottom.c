#include "mach.h"
#include "const.h"
#include "back.h"

/* This file contains low-level routines for generating assembly code. */

int procno = 0, holno = 0, cur_seg = -1, saved = FALSE;
char name[256], labeltje[256];

File *codefile;


align_word()

/* The EM-definition demands that segments are aligned at a word boundary
 */

{
	switch ( cur_seg) {
	  case SEGTXT : return;
	  default     : fprint( codefile, ALIGN_FMT);
	}
}


save_label( l)
char *l;
{
	sprint( labeltje, "%s", l);
	saved = TRUE;
}


dump_label()
{
	if ( saved)  {
		align_word();
		symbol_definition( labeltje);
	}
	saved = FALSE;
}


char *extnd_pro( procno)
int procno;
{
	sprint( name, "pro%d", procno);
	return( name);
}


char *extnd_start( procno)
int procno;
{
	sprint( name, "start%d", procno);
	return( name);
}


char *extnd_name( s)
char *s;
{
	sprint( name, NAME_FMT, s);
        return( name);
}


char *extnd_dnam( s)
char *s;
{
	sprint( name, DNAM_FMT, s);
        return( name);
}


char *extnd_dlb( g)
arith g;
{
	sprint( name, DLB_FMT, (arith)g);
        return( name);
}


char *extnd_ilb( l)
arith l;
{
	sprint( name, ILB_FMT, procno, (arith) l);
        return( name);
}


char *extnd_hol( hol)
int hol;
{
	sprint( name, HOL_FMT, hol);
        return( name);
}
