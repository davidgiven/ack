#include <ctype.h>
#include <system.h>
#include <stdio.h>
#if __STDC__
#include <stdarg.h>
extern error(char *, ...);
#else
#include <varargs.h>
#endif
#include "as.h"
#include "const.h"


/* This file contains the routine assemble(). Assemble() cuts an
 * assembly instruction in a label, a mnemonic and several operands.
 * For a mnemonic,label and operands it calls table writer defined 
 * routines, process_mnemonic() * process_label() and process_operand(), 
 * to give the table writer the oppurtunity to do something special.
 * At the end assemble() calls the routine belonging to the mnemonic 
 * with the supplied operands.
 *     If the table writer has other expectations of assemble() he should
 * write his own version.
 * 	Assemble parser the following instructions :
 * INSTR ::= [ STRING ':']? [ STRING [ OPERAND ( ',' OPERAND)*]? ]?
 * OPERAND ::= STRING [ '{' char* '}' |
 * 			'(' char* ')' | 
 * 			'[' char* ']'     ]?
 * note : nested brackets are not recognized.
 */


/* The following global varaibles are defined in the EM_parser.
 */

extern char *mnemonic[];
extern int (*instruction[])(), n_mnems;

/* The struct t_operand must be defined by the table writer in "as.h".
 * The constant MAX_OPERANDS is defined in "const.h"
 * To change MAX_OPERANDS effectively, the last statement in 
 * execute_mnem() must be changed.
 */

struct t_operand operand[ MAX_OPERANDS];



char *skip_space(), *parse_label(), *parse_mnemonic(), *parse_operand(),
     *skip_string(), *match_ch(), *Salloc(), *skip_operand();
int  label();


assemble( instr)
	char *instr;
/* Break an assembly instruction down in a LABEL, MNEMONIC and OPERANDS.
 */
{
	char *ptr, *copy, *mnem;
	int  n_ops = 0;

	copy = ptr = Salloc( instr, strlen( instr)+1);

	ptr = skip_space( ptr);
	if  ( label( ptr))  {     /* Look for a label */
		ptr = parse_label( ptr);
		if  ( *ptr == '\0')  return;
	}

	ptr = parse_mnemonic( ptr, &mnem);
	while  ( *ptr != '\0')  {  /* parse operans */
		if ( n_ops++ == MAX_OPERANDS) 
			error( "to many operands\n");
		ptr = parse_operand( ptr, n_ops, instr);
	}

	execute_mnemonic( mnem);   /* Execute the assembler instruction */
	free( copy);
}


int label( ptr)
	char *ptr;
{
	ptr = skip_string( ptr);
	ptr = skip_space( ptr);
	return( *ptr == ':');
}


char *parse_label( ptr)
	char *ptr;
{
	char *lab = ptr;

	ptr = skip_string( ptr);
	if  ( *ptr == ':') 
		*ptr++ = '\0';
	else {
		*ptr++ = '\0';
		ptr = skip_space( ptr);
		ptr++;		/* skip ':' */
	}
	handle_label( lab);
	ptr = skip_space( ptr);
	return( ptr);
}


char *parse_mnemonic( ptr, mnem)
	char *ptr, **mnem;
{
	*mnem = ptr;
	ptr = skip_string( ptr);
	if  ( *ptr != '\0') {
		*ptr++ = '\0';
		ptr = skip_space( ptr);
	}
	return( ptr);
}


char *parse_operand( ptr, n_ops, instr)
	char *ptr, *instr;
	int  n_ops;
{
	char *op = ptr,
	     *last;

	ptr = skip_operand( ptr, instr);
	for( last=ptr-1; isspace( *last); last--) 
		;
	if ( *ptr == ',')  {
		ptr = skip_space( ptr + 1);
	}
	*(last+1) = '\0';
		
	process_operand( op, &operand[ n_ops-1]);
	return( ptr);
}


char *skip_operand( ptr, instr)
	char *ptr, *instr;
{
	while  ( *ptr != ',' && *ptr != '\0')  {
		switch  ( *ptr)  {
		  case '{' : ptr = match_ch( '}', ptr, instr);
			     break;
		  case '(' : ptr = match_ch( ')', ptr, instr);
			     break;
		  case '[' : ptr = match_ch( ']', ptr, instr);
			     break;
		}
		ptr++;
	}
	return( ptr);
}


char *match_ch( c, str, instr)
	char c, *str, *instr;
{
	char *ptr, *strindex();

	ptr = strindex( str, c);
	if  ( ptr == 0)  {
		error( "syntax error in %s : %c expected\n", instr, c);
		return( str);
	}
	else
		return( ptr);
}


char *skip_string( ptr)
	char *ptr;
{
	while  ( *ptr != '\0' && !isspace( *ptr) && *ptr != ':')
		ptr++;
	return( ptr);
}


char *skip_space( ptr)
char *ptr;
{
	while ( isspace( *ptr) )
		ptr++;
	return( ptr);
}


/*** Execution **************************************************************/


execute_mnemonic( mnem)
char *mnem;

/* Find the function by "mnem" and execute it.
 */
{
	int low, mid, high, rel;

	process_mnemonic( mnem);

	low = 0;
	high = n_mnems-1;

	while ( TRUE) {
		mid = ( low + high) / 2;
		rel = strcmp(mnem, mnemonic[ mid]);

		if ( rel == 0 )
			break;
		else if ( high == low) {
			error( "can't find %s", mnem);
			return;
		}
		else if ( rel < 0)
			high = mid;
		else
			/* watch it, mid is truncated */
			low = ( mid == low ? low + 1: mid);
	}
	( *( instruction[ mid]))( &operand[0], &operand[1], &operand[2], 
			      	  &operand[3]);
}


/*** Error ****************************************************************/

#if __STDC__
/*VARARGS*/
error(char *fmt, ...)
{
	va_list args;
	extern int yylineno;
	extern int nerrors;

	va_start(args, fmt);
		fprint( STDERR, "ERROR in line %d :	", yylineno);
		doprnt( STDERR, fmt, args);
		fprint( STDERR, "\n");
	va_end(args);
	nerrors++;
}
#else
/*VARARGS*/
error(va_alist)
	va_dcl
{
	char *fmt;
	va_list args;
	extern int yylineno;
	extern int nerrors;

	va_start(args);
		fmt = va_arg(args, char *);
		fprint( STDERR, "ERROR in line %d :	", yylineno);
		doprnt( STDERR, fmt, args);
		fprint( STDERR, "\n");
	va_end(args);
	nerrors++;
}
#endif
