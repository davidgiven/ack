#include <ctype.h>
#include <system.h>
#include <varargs.h>

#define CODE_EXPANDER
#include "em.h"

#include "decl.h"

extern int library;
extern File *outfile;

/* This file contains some routines called from the parser in 'pars.g' */

question( str)
char *str;

/* Print : "if ( <str> ) {"
 */
{
	register char c;
	int arg_nr;

	out( "if ( ");
	for ( c = *str++; c != '\0'; c= *str++)
		if ( c == '$' && isdigit( *str)) {

			arg_nr = *str++ - '0' - 1;
			out( "%s", C_instr_info->arg_id[arg_nr]);
		}
		else
			out( "%c", c);
	
	out( ") {\n");
}

extern arith cur_pos;
extern int segment;

print_call( str)
char *str;

/* Print 'str' and handle the $i's. Also count the number of text-bytes
 * that are generated, look for text?() and reloc?() calls.
 */
{
	register char c;
	int arg_nr;
	extern int no_conversions;

	if ( strncmp( "text", str, 4) == 0 && isdigit( *(str+4))) 
		cur_pos += *(str+4) - '0';
	else if ( strncmp( "reloc", str, 5) == 0 && isdigit( *(str+5)) &&
		segment == SEGTXT) 
		cur_pos += *(str+5) - '0';

	for ( c = *str++; c != '\0'; c= *str++)
		if ( c == '$' && isdigit( *str)) {

			arg_nr = *str++ - '0' - 1;
			if ( no_conversions)
				out( "%s", C_instr_info->arg_id[arg_nr]);
			else
				out( "%s", C_instr_info->arg_conv[arg_nr]);
		}
		else
			out( "%c", c);
	
	out( ";\n");
}


header( instr)
char *instr;
{
	out( "void %s%s", instr, C_instr_info->header);
	def_admn( instr);
}


set_segment( seg)
int seg;
{
	switch ( seg) {
	  case SEGTXT : out( "switchseg( SEGTXT);\n");
			 break;
	  case SEGBSS  : out( "switchseg( SEGBSS);\n");
			 out( "dump_label();\n");
			 break;
	  case SEGHOL  : out( "switchseg( SEGHOL);\n");
			 break;
	  case SEGCON  : out( "switchseg( SEGCON);\n");
			 out( "dump_label();\n");
			 break;
	  case SEGROM  : out( "switchseg( SEGROM);\n");
			 out( "dump_label();\n");
			 break;
	  case UNKNOWN : break;	/* dan niet! */
	}
}

/*VARARGS*/
out(va_alist)
va_dcl
{
        va_list pvar;
        char *fmt;
 
        va_start(pvar);
        fmt = va_arg(pvar, char *);
        doprnt( outfile, fmt, pvar);
        va_end(pvar);
}

char *suffix( str, suf)
char *str, *suf;
{
	static char res[15];
	char *s, *strncpy(), *strcat();
	int strl, sufl, prefl;

	strl = strlen( str);
	sufl = strlen( suf);

	for ( s = str + strl; --s >= str && *s != '.';)
		;

	if ( *s == '.')
		prefl = s - str;
	else
		prefl = strl;
	
	if ( prefl + sufl + 1 > 14 )
		prefl = 14 - 1 - sufl;
	
	strncpy( res, str, prefl);
	res[prefl] = '\0';
	s = strcat( res, ".");
	s = strcat( s, suf);
	return( s);
}


set_outfile( name)
char *name;
{
	if ( library) {
		name = suffix( name, "c");
		sys_close( outfile);
		if ( !sys_open( name, OP_WRITE, &outfile))
			fprint( STDERR, "!! can't create %s !!\n", name);
		file_header();
	}
}


file_header()
{
	out( "#define CODE_EXPANDER\n");
	out( "#include \"mach.h\"\n");
	out( "#include \"back.h\"\n\n");
	out( "extern int B_holno, B_procno;\n\n");
}
