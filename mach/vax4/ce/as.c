#include <ctype.h>
#include "as.h"

#define DOLLAR  '$'
#define	LEFT	'('	 
#define RIGHT   ')'


/*****************************************************************************/

/* Een constraint-function */

cst( arg)
struct t_operand *arg;
{
	return( arg->type == CONST);
}


/*****************************************************************************/


/*    decode_operand() recognizes the follwing assembly-argumnets:
 
	$cst 	  		: CONST
	register		: REGISTER
	-(register)		: AUTO_DEC
	(register)+		: AUTO_INC
        (register)		: REG_DEF
	indx(register)          : IND_REG
	label+offset		: LABEL
	label[b|f]		: L_ILB
 */


/* An assembly instruction has at most 5 arguments.
 * This is for the time being.
 */

char lab_buf[4][256],
     ind_buf[4][256],
     *match(), *lab(), *ind(), *end_arg(), *strindex();

static int n_index = -1;


process_label( l)
char *l;
{
}


process_mnemonic( m)
char *m;
{
}


process_operand( arg, op)
register char *arg;
register struct t_operand *op;
{
	register char *einde;

	if ( n_index == 3) 
		n_index = 0;
	else
		n_index++;

	if  ( arg[0] == '~' ) {
		op->type = CONST;
		op->cst = arg+1;
	}
	else if ( is_reg( arg, &(op->num)) ) {
		op->type = REGISTER;
	}
	else if ((arg[0]=='-') && (arg[1] == LEFT) && 
				is_reg( arg+2,&(op->num))) {
		op->type = AUTO_DEC;
	}
	else if( ( arg[0] == LEFT) && is_reg( arg+1, &(op->num))) {
		arg = match( arg, RIGHT);
		if ( *(arg+1) == '+') 
			op->type = AUTO_INC;
		else
			op->type = REG_DEF;
	}
	else  { 
		einde = end_arg( arg);
		if ( (*einde == 'b' || *einde == 'f') && isdigit( *(einde-1))) {
			*einde = '\0';
			op->type = L_ILB;
			op->offset = "0";
			op->lab = arg;
		}
		else if ( *einde == RIGHT) {
			op->type = IND_REG;
			arg = ind( ind_buf[ n_index], arg);
			if ( is_reg( arg+1, &(op->num)))
				op->indx = ind_buf[ n_index];
			else
				fprint( STDERR, "unknown argtype %s\n", arg);
		}
		else {
			op->type = LABEL;
			arg = lab( lab_buf[ n_index], arg);
			op->lab = lab_buf[ n_index];
			if ( *arg == '\0') 
				op->offset = "0";
			else 
				op->offset = arg+1; 
		}

	}	
}


char *ind( buf, str)
register char *buf, *str;

/* Reads the index in front of '(register)'.
 */
{
	while ( *str != LEFT) 
		*buf++ = *str++;
	*buf = '\0';
	return( str);
}


char *lab( buf, str)
register char *buf, *str;

/* Reads 'label' in front of '+offset'.
 */
{
	while ( ( *str != '+') && ( *str != '\0'))
		*buf++ = *str++;
	*buf = '\0';
	while ( isspace( *(buf-1)) ) {
		*(buf-1) = '\0';
		buf--;
	}
	return( str);
}
	

int is_reg( str, num)
register char *str;
register int *num; 

/* Is "str" a 'registers' ?
 */
{
	if ( ( *str == 'a') && ( *(str+1) == 'p')) {
		*num = 12;
		return( TRUE);
	}
	else if ( ( *str == 'f') && ( *(str+1) == 'p')) {
		*num = 13;
		return( TRUE);
	}
	else if ( ( *str == 's') && ( *(str+1) == 'p')) {
		*num = 14;
		return( TRUE);
	}
	if ( *str == 'r') {
	        if ( isdigit( *(str+1)) &&  isdigit( *(str+2))) {
			*num = ( *(str+1) - '0') * 10 + *(str+2) - '0';
			return( TRUE);
		}
		else if ( isdigit( *(str+1))) {
			*num = *(str+1) - '0';
			return( TRUE);
		}
		else 
		     return( FALSE);
	}	
	return( FALSE);
}


char *end_arg( str)
register char *str;

/* Shift to the last character of "str".
 */
{
	while ( *str != '\0')
		str++;
	return( str-1);
}


char *match( str, sym)
register char *str;
char sym;
{
	while ( *str != sym)
		str++;
	return( str);
}


/******************************************************************************/

char my_buf[256];

gen_operand( op)
register struct t_operand *op;

/* Generate object-code for a argument.
 */
{
	switch( op->type) {
		case CONST :
				if (isdigit(op->cst[0])) {
					long l, atol();
					l = atol(op->cst);
					if (fit_6bits(l)) {
						@text1(%$(op->cst));
					}
					else {
						@text1( 0x8f);
						@text4( %$(op->cst));
					}
				}
				else {
					@__as_const(%$(op->cst));
				}
				break;
		case REGISTER: 	@text1( %d(0x50 | op->num));
				break;
		case REG_DEF : 	@text1( %d(0x60 | op->num));
			  	break;
		case AUTO_DEC : @text1( %d(0x70 | op->num));
			  	break;
		case AUTO_INC : @text1( %d(0x80 | op->num));
			  	break;
		case IND_REG :
				if (isdigit(op->indx[0])) {
					long l, atol();
					l = atol(op->indx);
					if (fit_byte(l)) {
						@text1( %d(0xa0 | op->num));
			  			@text1( %$(op->indx));
					} else if (fit_word(l)) {
						@text1( %d(0xc0 | op->num));
			  			@text2( %$(op->indx));
					} else {
						@text1( %d(0xe0 | op->num));
			  			@text4( %$(op->indx));
					}
				}
				else {
					@__as_indexed(%$(op->indx) , %d(op->num));
				}
				break;
		case LABEL : 	@text1( 0xef);
				if ( strindex( op->lab, DOLLAR)) {
			@reloc4( %$(op->lab), %$(op->offset), PC_REL);
				}
				else if ( strindex( op->lab, LEFT)) {
			@reloc4( %$(op->lab), %$(op->offset), PC_REL);
				}
				else {
					sprint( my_buf, "\"%s\"", op->lab);
				@reloc4( %$(my_buf), %$(op->offset) , PC_REL);
				}
				break;
		case L_ILB :	@text1( %dist( op->lab));
				break;
		default : fprint( STDERR, "error");
	}
}
