#include <system.h>
#include <em.h>
#include <out.h>
#include "header.h"
#include "data.h"
#include "mach.h"

/* The extnd_*()s make a name unique. The resulting string is directly stored
 * in the symbol_table (by mysprint()). Later additional fields in the 
 * symbol_table are filled. For these actions the values of the index in
 * the symbol_table and the length of the string are stored.
 */

extern int 	string_lengte, index_symbol_table;
int 		procno = 0, holno = 0;


char *extnd_pro( procno)
int procno;
{
	string_lengte = mysprint( "%cprc%d", GENLAB, procno);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_start( procno)
int procno;
{
	string_lengte = mysprint( "%cstrt%d", GENLAB, procno);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_name( s)
char *s;
{
	string_lengte = mysprint( NAME_FMT, s);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( string_area + symbol_table[ index_symbol_table].on_foff);
}


char *extnd_dnam( s)
char *s;
{
	string_lengte = mysprint( DNAM_FMT, s);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_dlb( g)
arith g;
{
	string_lengte = mysprint( DLB_FMT, (arith)g);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_ilb( l)
arith l;
{
	string_lengte = mysprint( ILB_FMT, procno, (arith) l);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_hol( hol)
int hol;
{
	string_lengte = mysprint( HOL_FMT, hol);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_part( d)
int d;
{
	string_lengte = mysprint( "%cprt%x", GENLAB, d);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_cont( d)
int d;
{
	string_lengte = mysprint( "%ccnt%x", GENLAB, d);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}
