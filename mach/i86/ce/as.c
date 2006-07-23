#include "arg_type.h"
#include "as.h"

static struct t_operand dummy = { IS_REG, AX, 0, 0, 0};
struct t_operand saved_op, *AX_oper = &dummy; 

save_op( op)
struct t_operand *op;
{
	saved_op.type = op->type;
	saved_op.reg = op->reg;
	saved_op.expr = op->expr;
	saved_op.lab = op->lab;
	saved_op.off = op->off;
}

#define last( s)	( s + strlen( s) - 1)
#define LEFT	'('
#define RIGHT	')'
#define DOLLAR	'$'

block_assemble( instr, nr, first, Last)
char **instr;
int nr, first, Last;
{
	int i;

	if ( first) {
		if( strncmp( instr[0], "pop", 3) == 0) {
			*instr[0] = 'P';
			*( instr[0]+1) = 'O';
			*( instr[0]+2) = 'P';
		}
		else
			@clean_push_buf();
	}
	if ( Last && strncmp( instr[nr-1], "push", 4) == 0) {
			*instr[nr-1] = 'P';
			*( instr[nr-1]+1) = 'U';
			*( instr[nr-1]+2) = 'S';
			*( instr[nr-1]+3) = 'H';
	}

	for( i=0; i<nr; i++)
		assemble( instr[i]);
}
		

process_label( l)
char *l;
{
}


process_mnemonic( m)
char *m;
{
}


process_operand( str, op)
char *str;
struct t_operand *op;

/*	expr		->	IS_DATA en IS_LABEL
 *	reg		->	IS_REG en IS_ACCU
 *	(expr)		->	IS_ADDR
 *	expr(reg)	->	IS_MEM
 */
{
	char *ptr, *strchr();

	op->type = UNKNOWN;
	if ( *last( str) == RIGHT) {
		ptr = strchr( str, LEFT);
		*last( str) = '\0';
		*ptr = '\0';
		if ( is_reg( ptr+1, op)) {
			op->type = IS_MEM;
			op->expr = ( *str == '\0' ? "0" : str);
		}
		else {
			set_label( ptr+1, op);
			op->type = IS_ADDR;
		}
	}
	else
		if ( is_reg( str, op))
			op->type = IS_REG;
		else {
			if ( contains_label( str))
				set_label( str, op);
			else {
				op->type = IS_DATA;
				op->expr = str;
			}
		}
}

int is_reg( str, op)
char *str;
struct t_operand *op;
{
	if ( strlen( str) != 2)
		return( 0);

	switch ( *(str+1)) {
	  case 'x' :
	  case 'l' : switch( *str) {
		       case 'a' : op->reg = 0;
				  return( TRUE);

		       case 'c' : op->reg = 1;
				  return( TRUE);

		       case 'd' : op->reg = 2;
				  return( TRUE);

		       case 'b' : op->reg = 3;
				  return( TRUE);

		       default  : return( FALSE);
		     }

	  case 'h' : switch( *str) {
		       case 'a' : op->reg = 4;
				  return( TRUE);

		       case 'c' : op->reg = 5;
				  return( TRUE);

		       case 'd' : op->reg = 6;
				  return( TRUE);

		       case 'b' : op->reg = 7;
				  return( TRUE);

		       default  : return( FALSE);
		     }

	  case 'p' : switch ( *str) {
		       case 's' : op->reg = 4;
				  return( TRUE);

		       case 'b' : op->reg = 5;
				  return( TRUE);

		       default  : return( FALSE);
		     }

	  case 'i' : switch ( *str) {
		       case 's' : op->reg = 6;
				  return( TRUE);

		       case 'd' : op->reg = 7;
				  return( TRUE);

		       default  : return( FALSE);
		     }

	  default  : return( FALSE);
	}
}

#include <ctype.h>
#define	 isletter( c)	( isalpha( c) || c == '_')

int contains_label( str)
char *str;
{
	while( !isletter( *str) && *str != '\0')
		if ( *str == '$')
			if ( arg_type( str) == STRING)
				return( TRUE);
			else
				str += 2;
		else
			str++;

	return( isletter( *str));
}

set_label( str, op)
char *str;
struct t_operand *op;
{
	char *ptr, *strchr(), *sprint();
	static char buf[256];

	ptr = strchr( str, '+');

	if ( ptr == 0)
		op->off = "0";
	else {
		*ptr = '\0';
		op->off = ptr + 1;
	}

	if ( isdigit( *str) && ( *(str+1) == 'b' || *(str+1) == 'f') &&
	     *(str+2) == '\0') {
		*(str+1) = '\0';	/* b of f verwijderen! */
		op->lab = str;
		op->type = IS_ILB;
	}
	else {
		op->type = IS_LABEL;
		if ( strchr( str, DOLLAR) != 0)
			op->lab = str;
		else 
			/* nood oplossing */
			op->lab = sprint( buf, "\"%s\"", str);
	}
}


/******************************************************************************/



mod_RM( reg, op)
int reg;
struct t_operand *op;
{
	if ( REG( op))
		R233( 0x3, reg, op->reg);
	else if ( ADDR( op)) {
		R233( 0x0, reg, 0x6);
		@reloc2( %$(op->lab), %$(op->off), ABSOLUTE);
	}
	else if ( strcmp( op->expr, "0") == 0)
		switch( op->reg) {
		  case SI : R233( 0x0, reg, 0x4);
			    break;

		  case DI : R233( 0x0, reg, 0x5);
			    break;

		  case BP : R233( 0x1, reg, 0x6);	/* Uitzondering! */
			    @text1( 0);
			    break;

		  case BX : R233( 0x0, reg, 0x7);
			    break;

		  default : fprint( STDERR, "Wrong index register %d\n",
				    op->reg);
		}
	else {
		@if ( fit_byte( %$(op->expr)))
			switch( op->reg) {
		  	  case SI : R233( 0x1, reg, 0x4);
				    break;
	
			  case DI : R233( 0x1, reg, 0x5);
				    break;
	
			  case BP : R233( 0x1, reg, 0x6);
				    break;
	
			  case BX : R233( 0x1, reg, 0x7);
				    break;
	
			  default : fprint( STDERR, "Wrong index register %d\n",
					    op->reg);
			}
			@text1( %$(op->expr));
		@else
			switch( op->reg) {
			  case SI : R233( 0x2, reg, 0x4);
				    break;
	
			  case DI : R233( 0x2, reg, 0x5);
				    break;
	
			  case BP : R233( 0x2, reg, 0x6);
				    break;
	
			  case BX : R233( 0x2, reg, 0x7);
				    break;
	
			  default : fprint( STDERR, "Wrong index register %d\n",
					    op->reg);
			}
			@text2( %$(op->expr));
		@fi
	}
}

mov_REG_EADDR( dst, src)
struct t_operand *dst, *src;
{
	if ( REG(src) && dst->reg == src->reg)
		; /* Nothing!! result of push/pop optimization */
	else {
		@text1( 0x8b);
		mod_RM( dst->reg, src);
	}
}


R233( a, b, c)
int a,b,c;
{
	@text1( %d( (a << 6) | ( b << 3) | c));
}


R53( a, b)
int a,b;
{
	@text1( %d( (a << 3) | b));
}
