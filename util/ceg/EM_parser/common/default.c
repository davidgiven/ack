#include "decl.h"
#include <system.h>

/* This file contains a number of funtions which will be called to handle
 * the 'DEF_C_INSTR' (e.g., C_loe).
 */

#define _ICON	0
#define _UCON	4
#define _FCON	8
#define C_DEE	12
#define C_FIL	15
#define C_GTO	18
#define C_INE	21
#define C_LAE	24
#define C_LDE	27
#define C_LOE	30
#define C_SDE	33
#define C_STE	36
#define C_ZRE	39

struct { char *name; int segment, generated; }
		def_info[] = {
		/* _ICON */	{ "C_con_icon", SEGCON, 0},
				{ "C_rom_icon", SEGROM, 0},
				{ "C_hol_icon", NOSEG, 0},
				{ "C_bss_icon", NOSEG, 0},

		/* _UCON */	{ "C_con_ucon", SEGCON, 0},
				{ "C_rom_ucon", SEGROM, 0},
				{ "C_hol_ucon", NOSEG, 0},
				{ "C_bss_ucon", NOSEG, 0},

		/* _FCON */	{ "C_con_fcon", SEGCON, 0},
				{ "C_rom_fcon", SEGROM, 0},
				{ "C_hol_fcon", NOSEG, 0},
				{ "C_bss_fcon", NOSEG, 0},

		/* C_DEE */	{ "C_dee", SEGTXT, 0},
				{ "C_dee_dnam", SEGTXT, 0},
				{ "C_dee_dlb", SEGTXT, 0},

		/* C_FIL */	{ "C_fil", SEGTXT, 0},
				{ "C_fil_dnam", SEGTXT, 0},
				{ "C_fil_dlb", SEGTXT, 0},

		/* C_GTO */	{ "C_gto", SEGTXT, 0},
				{ "C_gto_dnam", SEGTXT, 0},
				{ "C_gto_dlb", SEGTXT, 0},

		/* C_INE */	{ "C_ine", SEGTXT, 0},
				{ "C_ine_dnam", SEGTXT, 0},
				{ "C_ine_dlb", SEGTXT, 0},

		/* C_LAE */	{ "C_lae", SEGTXT, 0},
				{ "C_lae_dnam", SEGTXT, 0},
				{ "C_lae_dlb", SEGTXT, 0},

		/* C_LDE */	{ "C_lde", SEGTXT, 0},
				{ "C_lde_dnam", SEGTXT, 0},
				{ "C_lde_dlb", SEGTXT, 0},

		/* C_LOE */	{ "C_loe", SEGTXT, 0},
				{ "C_loe_dnam", SEGTXT, 0},
				{ "C_loe_dlb", SEGTXT, 0},

		/* C_SDE */	{ "C_sde", SEGTXT, 0},
				{ "C_sde_dnam", SEGTXT, 0},
				{ "C_sde_dlb", SEGTXT, 0},

		/* C_STE */	{ "C_ste", SEGTXT, 0},
				{ "C_ste_dnam", SEGTXT, 0},
				{ "C_ste_dlb", SEGTXT, 0},

		/* C_ZRE */	{ "C_zre", SEGTXT, 0},
				{ "C_zre_dnam", SEGTXT, 0},
				{ "C_zre_dlb", SEGTXT, 0}
		};	/* This big array contains information about the 
			 * functions that will be generated from the descrip-
			 * tion of 1 DEF_C_INSTR. It contains the name, the
			 * segment and a flag of these instructions. The flag
			 * is used to determine if the instruction is explicitly
			 * given in the EM_table or not.
			 */


File	*save_file;	/* Save original output-file in this variable */
int	def_start,	/* Index in def_info[], start of the expanded C_INSTR */
	def_end,	/* last expanded C_INSTR index. */
	save_lineno;

extern File *outfile;
extern char yytext[];
extern int yylineno;




init_defaults( instr)
char *instr;

/* Save current output-file and write on a file called ".default" */

{
	save_file = outfile;
	save_lineno = yylineno;
	sys_open( ".default", OP_WRITE, &outfile);
	set_def_params( instr);
}


int bss_or_hol_instr( index)
int index;
{
	return ( index == _ICON + 2 || index == _ICON + 3 ||
	     	 index == _UCON + 2 || index == _UCON + 3 ||
	     	 index == _FCON + 2 || index == _FCON + 3);
}


set_def_params( instr)
char *instr;

/* Give 'def_start' and 'def_end' their correct values. */

{
	int low, high, mid, rel;

	if ( *instr == '.') {	/* ..icon | ..ucon | ..fcon */
		switch ( *(instr+2)) {
		  case 'i' : def_start = _ICON;
			     break;
		  case 'u' : def_start = _UCON;
			     break;
		  case 'f' : def_start = _FCON;
			     break;
		}
		def_end = def_start + 3;
	}
	else {
		low = C_DEE;
		high = C_ZRE;

		while ( TRUE) {
			mid = ( low + high) / 6 * 3;
			rel = strncmp( instr, def_info[mid].name, 5);

			if ( rel == 0 )
				break;
			else if ( high == low)
				abort();
			else if ( rel < 0)
				high = mid;
			else
				/* be careful : 'mid' is truncated */
				low = ( mid == low ? low + 3: mid);
		}
		def_start = mid;
		def_end = def_start + 2;
	}
}

#include <stdio.h>
#include "Lpars.h"

handle_defaults()

/* Switch back to original output-file and start generating the functions 
 * from the file ".default".
 */
{
	FILE *old, *tmp, *switch_input();
	int i, old_yylineno;
	extern int CD_pos;

	sys_close( outfile);
	outfile = save_file;

	tmp = fopen( ".default", "r");
	old = switch_input( tmp);
	old_yylineno = yylineno;

	for ( i = def_start; i <= def_end; i++) {
		yylineno = save_lineno;
		if ( !def_info[i].generated) {
			set_C_instr_info( def_info[i].name);
			segment = def_info[i].segment;

			set_outfile( def_info[i].name);
			header( def_info[i].name);
			CD_pos = TRUE;	/* Set mylex() in correct state */

			if ( bss_or_hol_instr( i)) {
				extnd_header();
				/* Rest of body is just the same as the corres-
				 * ponding C_con_xxx C_rom_xxx instruction
				 * so set correct info.
				 */
				set_C_instr_info( def_info[(i>>2)<<2].name);
				segment = UNKNOWN;
				/* to prevent another dump_label+switchseg */
				def_row();
				out( "}\n\n");
			}
			else
				def_row();
			rewind( tmp);
		}
	}
	fclose( tmp);
	switch_input( old);
	yylineno = old_yylineno;
}

def_admn( instr)
char *instr;

/* Mark this 'instr' as being generated */

{
	int low, high, mid, rel;

	low = _ICON;
	high = C_ZRE + 2;

	while ( TRUE) {
		mid = ( low + high) / 2;
		rel = strcmp( instr, def_info[mid].name);

		if ( rel == 0 )
			break;
		else if ( high == low)
			return;
		else if ( rel < 0)
			high = mid;
		else
			low = ( mid == low ? low + 1: mid);
	}
	def_info[mid].generated = 1;
}

extnd_header()

/* Generates code for loooking at the parameters 'nbytes' and 'i' of the
 * bss an hol pseudo-instructions.
 */
{
	out( "if ( %s == 0 ) {\n", C_instr_info->arg_id[3]);
	out( "common( %s);\n", C_instr_info->arg_id[0]);
	out( "}\nelse {\n");
	set_segment( SEGCON);
	out( "for ( %s = 0; %s < %s/EM_WSIZE; %s++)\n",
	     C_instr_info->arg_id[3], C_instr_info->arg_id[3],
	     C_instr_info->arg_id[0], C_instr_info->arg_id[3]);
}
