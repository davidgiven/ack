#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "as.h"
#include "arg_type.h"


process_label() {}


process_mnemonic( m)
char *m;
{
	for ( ; *m != '\0'; m++)
		if ( *m == '.')
			*m = '_';
}


process_operand( str, op)
register char *str;
register struct t_operand *op;
{
	char *glob_lbl(), *strindex();

	op->type = 0;

	switch ( *str) {
	  case '#' : op->type = IS_IMMEDIATE;
		     op->expr = str+1;

		     if ( *(op->expr) != '$') {			/*  #1  */
			op->val = atoi( str+1);
			if ( 1 <= op->val  &&  op->val <= 8 ) {
				op->type = IS_QUICK;
				op->lbl = NULL;
			}
		     }
		     else 
			if ( arg_type( str+1) == STRING) {	/*  #$1+$2  */
				op->lbl = op->expr;
				*(op->lbl+2) = '\0';
				op->expr = op->lbl+3;
		     	}
		     	else					/*  #$1  */
				op->lbl = NULL;
		     break;

	  case '(' : if ( strindex( str+1, ',') == NULL)
			if ( is_reg( str+1)) {
				op->reg = reg_val( str+1);

				if ( *(str+4) == '+')		/*  (sp)+  */
					op->type = IS_INCR;
				else				/*  (a0)  */
					op->type = IS_IND_REG;
			}
			else {
				op->type = IS_IND_MEM;
				op->expr = str+1;
				for ( str++; *++str != ')';)
					;
				*str = '\0';

				if ( *(op->expr) == '$')
					if ( arg_type( op->expr) == STRING) {
								/*  ($1+$2)  */
						op->lbl = op->expr;
						if ( strlen( op->lbl) == 2) 
							op->expr = "0";
						else {
							*(op->lbl+2) = '\0';
							op->expr = op->lbl+3;
						}
		     			}
		     			else			/*  ($1)  */
						op->lbl = NULL;
				else if ( isdigit( *(op->expr))) /*  (1)  */
					op->lbl = NULL;
				else {				/*  (.trppc) */
					op->lbl = glob_lbl( op->expr);
					op->expr = "0";
				}
			}
		     else
			if ( *(str+1) == '[') {
				op->type = IS_IND_IND;
				op->expr = str+2;
				for ( str += 2; *++str != ',';)
					;
				*str++ = '\0';
				for ( ; *str == ' '; str++)
					;
				op->reg = reg_val( str);

				for ( ; *str++ != ']';)
					;
				if ( *str == ')')	/*  ([$1, a6])  */
					op->expr2 = 0;
				else {			/*  ([8,a6],8)  */
					for ( ; *str++ != ',';)
						;
					for ( ; *str == ' '; str++)
						;
					op->expr2 = atoi( str);
				}
			}
			else {
				op->expr = str+1;
				for ( str++; *++str != ',';)
					;
				*str++ = '\0';
				for ( ; *str == ' '; str++)
					;
				op->reg = reg_val( str);
				if ( *(str+2) == ')')		/*  (4, a0)  */
					op->type = IS_IND_REG_DISPL;
				else {			 /*  (0, sp, d0.l*1)  */
					op->type = IS_3_OPS;
					for ( str++; *++str != ',';)
						;
					for ( ; *str == ' '; str++)
						;
					op->reg2 = reg_val( str);

					for ( ; *str++ != '*';)
						;
					op->scale = atoi( str);
				}
			}
		     break;

	  case '-' : op->type = IS_DECR;			/*  -(sp)  */
		     op->reg = reg_val( str+2);
		     break;

	  case '$' : op->type = IS_GLOB_LBL;			/*  $1  */
		     op->lbl = str;
		     op->expr ="0";
		     break;

	  default  : if ( is_reg( str)) {
			op->reg = reg_val( str);
			if ( *(str+2) == ':') {			/*  d2:d1  */
				op->type = IS_REG_PAIR;
				op->reg2 = reg_val( str+3);
			}
			else					/*  a6  */
				op->type = ( *str == 'd' ? IS_D_REG : IS_A_REG);
		     }
		     else if ( isdigit( *str)) {		/*  1f  */
			op->type = IS_LOC_LBL;
			op->lbl = str;
			op->expr = "0";
			*(str+1) ='\0';
		     }
		     else {					/*  .strhp  */
			op->type = IS_GLOB_LBL;
			op->lbl = glob_lbl( str);
			*(str+1) ='\0';
			op->expr = "0";
		     }
	}
}


int reg_val( reg)
char *reg;
{
	return( *reg == 's' ? 7 : atoi( reg+1));
}

int is_reg( str)
register char *str;
{
	switch ( *str) {
	  case 'a' :
	  case 'd' : return( isdigit( *(str+1)));

	  case 's' : return( *(str+1) == 'p');

	  default  : return( 0);
	}
}


char *glob_lbl( lbl)
char *lbl;
{
	char *gl, *Malloc();

	gl = Malloc( strlen( lbl) + 3);
	sprintf( gl, "\"%s\"", lbl);
	return( gl);
}


/******************************************************************************/


int mode_reg( eaddr)
register struct t_operand *eaddr;
{
	switch ( eaddr->type) {
	  case IS_A_REG         : return( 0x08 | eaddr->reg);

	  case IS_D_REG         : return( 0x00 | eaddr->reg);

	  case IS_IND_REG       : return( 0x10 | eaddr->reg);

	  case IS_INCR          : return( 0x18 | eaddr->reg);

	  case IS_DECR          : return( 0x20 | eaddr->reg);

	  case IS_IND_MEM       : return( 0x39);

	  case IS_IND_IND       : return( 0x30 | eaddr->reg);

	  case IS_IND_REG_DISPL : return( 0x28 | eaddr->reg);

	  case IS_GLOB_LBL      : return( 0x39);

	  case IS_3_OPS         : if ( isdigit( *(eaddr->expr)) &&
				                    atoi( eaddr->expr) < 128)
					return( 0x30 | eaddr->reg);
				  else
					fprintf( stderr, "FOUT in IS_3_OPS\n");
				  break;

	  case IS_QUICK		:
	  case IS_IMMEDIATE	: return( 0x3c);

	  default		: fprintf( stderr,
					   "mode_reg(), verkeerde operand %d\n",
					   eaddr->type);
				  abort();
				  break;
	}
}


code_extension( eaddr)
register struct t_operand *eaddr;
{

	switch ( eaddr->type) {
	  case IS_IND_MEM       : if ( eaddr->lbl == NULL) 
					@text4( %$( eaddr->expr));
				  else
					@reloc4( %$( eaddr->lbl),
						 %$( eaddr->expr),
						 ABSOLUTE);
				  break;

	  case IS_IND_IND       : if ( eaddr->expr2 == 0) {
				  	@text2( 0x161);
					@text2( %$(eaddr->expr));
				  }
				  else {
				  	@text2( 0x162);
					@text2( %$(eaddr->expr));
					@text2( %d(eaddr->expr2));
				  }
				  break;

	  case IS_IND_REG_DISPL : @text2( %$( eaddr->expr));
				  break;

	  case IS_GLOB_LBL      : @reloc4( %$(eaddr->lbl),
					   %$(eaddr->expr),
					   ABSOLUTE);
				  break;

	  case IS_3_OPS         : if ( isdigit( *(eaddr->expr)) &&
				                    atoi( eaddr->expr) < 128) {

					@text2( %d( 0x0800 |
				  	  	  ( eaddr->reg2 << 12) |
					  	  ( two_log( eaddr->scale)<<9) |
					  	  atoi( eaddr->expr)));
				  }
				  else
					fprintf( stderr, "FOUT in IS_3_OPS\n");
				  break;

	  case IS_QUICK		:
	  case IS_IMMEDIATE	: if ( eaddr->lbl != NULL)
					@reloc4( %$(eaddr->lbl),
						 %$(eaddr->expr),
						 ABSOLUTE);
				  else
					@text4( %$(eaddr->expr));
	}
}


int reg_mode( eaddr)
struct t_operand *eaddr;
{
	int mr;

	mr = mode_reg( eaddr);
	return( ((mr & 0x7) << 3) | ((mr & 0x38) >> 3));
}


code_opcode( opcode, field1, field2, eaddr)
int opcode, field1, field2;
struct t_operand *eaddr;
{
	@text2( %d(((opcode & 0xf) << 12) | ((field1 & 0x7) << 9) |
		   ((field2 & 0x7) << 6) | (mode_reg(eaddr) & 0x3f)));
}


code_instr( opcode, field1, field2, eaddr)
int opcode, field1, field2;
struct t_operand *eaddr;
{
	if (eaddr->type == IS_IND_REG_DISPL) {
		@__instr_code(%d(((opcode & 0xf) << 12) | ((field1 & 0x7) << 9) |
			        ((field2 & 0x7) << 6)),
			      %d(eaddr->reg), %$(eaddr->expr));
	}
	else {
		code_opcode( opcode, field1, field2, eaddr);
		code_extension( eaddr);
	}
}


code_move( size, src, dst)
int size;
struct t_operand *src, *dst;
{
	if (src->type == IS_IND_REG_DISPL) {
		if (dst->type == IS_IND_REG_DISPL) {
			@__moveXX(%d( ((size & 0x3) << 12)),
				 %d(dst->reg), %$(dst->expr),
				 %d(src->reg), %$(src->expr));
		}
		else {
			@__instr_code(%d( ((size & 0x3) << 12)|((reg_mode( dst) & 0x3f) << 6)),
				 %d(src->reg), %$(src->expr));
		}
	}
	else if (dst->type == IS_IND_REG_DISPL) {
		@__move_X(%d( ((size & 0x3) << 12) | (mode_reg( src) & 0x3f)),
			 %d(dst->reg), %$(dst->expr));
	}
	else {
		@text2( %d( ((size & 0x3) << 12) | ((reg_mode( dst) & 0x3f) << 6) |
		    	     		   (mode_reg( src) & 0x3f)));
		code_extension( src);
		code_extension( dst);
	}
}


code_dist4( dst)
struct t_operand *dst;
{
	@reloc4( %$(dst->lbl), %$(dst->expr) + 4, PC_REL);
}


int two_log( nr)
register int nr;
{
	register int log;

	for ( log = 0; nr >= 2; nr >>= 1)
		log++;

	return( log);
}
