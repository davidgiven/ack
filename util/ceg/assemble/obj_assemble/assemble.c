#include <ctype.h>
#include <system.h>
#include "as.h"
#include "const.h"

extern char *mnemonic[];
extern int (*instruction[])(), n_mnems;
struct t_operand operand[ MAX_OPERANDS];

/* To change MAX_OPERANDS, the last statement in execute_mnem() must
 * be changed.
 */


char *skip_space(), *parse_label(), *parse_mnemonic(), *parse_operand(),
     *skip_string(), *match_ch(), *Salloc(), *skip_operand();
int  label();


assemble( instr)
	char *instr;

/* INSTR ::= [ STRING ':']? [ STRING [ OPERAND ( ',' OPERAND)*]? ]?
 * OPERAND ::= STRING [ '{' char* '}' |
 * 			'(' char* ')' | 
 * 			'[' char* ']'     ]?
 * Break an assembly instruction down in a LABEL, MNEMONIC and OPERANDS.
 */
{
	char *ptr, *copy, *mnem;
	int  n_ops = 0;

	copy = ptr = Salloc( instr, strlen( instr)+1);

	ptr = skip_space( ptr);
	if  ( label( ptr))  {
		ptr = parse_label( ptr);
		if  ( *ptr == '\0')  return;
	}

	ptr = parse_mnemonic( ptr, &mnem);
	while  ( *ptr != '\0')  {
		if ( n_ops++ == MAX_OPERANDS) 
			error( "to many operands\n");
		ptr = parse_operand( ptr, n_ops, instr);
	}

	execute_mnemonic( mnem);
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
	char *op = ptr;

	ptr = skip_operand( ptr, instr);
	if ( *ptr != '\0')  {
		if ( *ptr == ',') 
			*ptr++ = '\0';
		else {
			*ptr++ = '\0';
			ptr = skip_space( ptr);
			if  ( *ptr != '\0') ptr++;   /* skip ';' */
		}
		ptr = skip_space( ptr);
	}
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
	while  ( isalnum( *ptr) || ( *ptr == '_'))
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


/****************************************************************************/


execute_mnemonic( mnem)
char *mnem;

/* Find the function by "mnem" and execute it.
 */
{
	int low, mid, high, rel;

	low = 0;
	high = n_mnems-1;

	while ( TRUE) {
		mid = ( low + high) / 2;
		rel = strcmp(mnem, mnemonic[ mid]);

		if ( rel == 0 )
			break;
		else if ( high == low)
			error( "can't find %s", mnem);
		else if ( rel < 0)
			high = mid;
		else
			/* pas op, mid is naar beneden afgerond !! */
			low = ( mid == low ? low + 1: mid);
	}
	process_mnemonic( mnem);
	( *( instruction[ mid]))( &operand[0], &operand[1], &operand[2], 
			      	  &operand[3]);
}


error( fmt, argv)
char *fmt;
int argv;
{
	extern int yylineno;

	fprint( STDERR, "!! ERROR in line %d :	", yylineno);
	doprnt( STDERR, fmt, &argv);
	fprint( STDERR, "	!!\n");
}
