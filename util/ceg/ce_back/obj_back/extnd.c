#include <system.h>
#include <out.h>
#include <em.h>
#include "header.h"
#include "back.h"
#include "mach.h"
#include <varargs.h>

/* Mysprint() stores the string directly in the string_arae. This saves
 * a copy action. It is assumed that the strings stored in the string-table
 * are never longer than MAXSTRLEN bytes.
 */

#define MAXSTRLEN	1024

/*VARARGS*/
static int mysprint(va_alist)
	va_dcl
{
	char *fmt;
	va_list args;
	int retval;

	va_start(args);
	fmt = va_arg(args, char *);
	while (string + MAXSTRLEN - string_area > size_string)
		mem_string();
	retval = _format(string, fmt, args);
	string[retval] = '\0';
	va_end(args);
	return retval;
}

/* The extnd_*()s make a name unique. The resulting string is directly stored
 * in the symbol_table (by mysprint()). Later additional fields in the 
 * symbol_table are filled. For these actions the values of the index in
 * the symbol_table and the length of the string are stored.
 */

extern int 	string_lengte, index_symbol_table;


char *extnd_pro( prcno)
int prcno;
{
	string_lengte = mysprint( "%cprc%d", GENLAB, prcno);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_start( prcno)
int prcno;
{
	string_lengte = mysprint( "%cstrt%d", GENLAB, prcno);
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
label g;
{
	string_lengte = mysprint( DLB_FMT, (long)g);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}


char *extnd_ilb( l, prcno)
arith l;
{
	string_lengte = mysprint( ILB_FMT, prcno, (long) l);
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


char *extnd_main( d)
int d;
{
	string_lengte = mysprint( "%cmcnt%x", GENLAB, d);
        index_symbol_table = find_sym( string, STORE_STRING);
	return( symbol_table[ index_symbol_table].on_foff + string_area);
}
