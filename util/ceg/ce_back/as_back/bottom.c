#include "header.h"

/* This file contains low-level routines for generating assembly code. */

int cur_seg = -1, saved = 0;
char name[256], labeltje[256];

FILE* codefile;


align_word()

/* The EM-definition demands that segments are aligned at a word boundary
 */

{
	switch ( cur_seg) {
	  case SEGTXT : return;
	  default     : fprintf( codefile, ALIGN_FMT);
	}
}


save_label( l)
char *l;
{
	sprintf( labeltje, "%s", l);
	saved = 1;
}


dump_label()
{
	if ( saved)  {
		align_word();
		symbol_definition( labeltje);
	}
	saved = 0;
}


char *extnd_pro( prcno)
int prcno;
{
	sprintf( name, "pro%d", prcno);
	return( name);
}


char *extnd_start( prcno)
int prcno;
{
	sprintf( name, "start%d", prcno);
	return( name);
}


char *extnd_name( s)
char *s;
{
	sprintf( name, NAME_FMT, s);
        return( name);
}


char *extnd_dnam( s)
char *s;
{
	sprintf( name, DNAM_FMT, s);
        return( name);
}


char *extnd_dlb( g)
arith g;
{
	sprintf( name, DLB_FMT, (long)g);
        return( name);
}


char *extnd_ilb( l, prcno)
arith l;
{
	sprintf( name, ILB_FMT, prcno, (long) l);
        return( name);
}


char *extnd_hol( hol)
int hol;
{
	sprintf( name, HOL_FMT, hol);
        return( name);
}


char *extnd_part( d)
int d;
{
	sprintf( name, "part%x", d);
	return( name);
}


char *extnd_cont( d)
int d;
{
	sprintf( name, "cont%x", d);
	return( name);
}


char *extnd_main( d)
int d;
{
	sprintf( name, "main%x", d);
	return( name);
}
