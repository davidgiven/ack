{

#include <system.h>
#include "decl.h"
#include "em.h"
extern char yytext[];
extern int yylineno;
extern int first_action, last_action, token;

t_token tok, stok;
int no_conversions = FALSE, library, segment = UNKNOWN;
File *outfile;
char *to_change;

}

%token 	C_INSTR, DEF_C_INSTR, CONDITION, ARROW, EQUIV,
	CALL, ASSEM_INSTR, DEFAULT, ERROR;
%start	table, table;
%start	def_row, def_row;
%start	c_table, c_table;
%lexical	lex_analyzer ;


table	: row*
	;

row	: C_INSTR 		{ set_outfile( yytext); header( yytext);}
	  [ special | simple]	{ out( "}\n\n");}

	| DEF_C_INSTR		{ init_defaults( yytext);}
	  [ Dspecial | Dsimple] { handle_defaults();}
	;

special : CONDITION 		{ question( &yytext[0]);}
	  simple 		{ out( "}\n");}

	  [ CONDITION 		{ out( "else "); question( &yytext[0]);}
	    simple 		{ out( "}\n");}
 	  ]*

	  DEFAULT 		{ out( "else {\n");}
	  simple 		{ out( "}\n");}
	;

simple	: ARROW 		{ set_segment( segment); save_output();}
	  actionlist		{ back_patch();}

	| EQUIV			{ no_conversions = TRUE; save_output();}
	  actionlist		{ no_conversions = FALSE; back_patch();}

	;


actionlist :			{ first_action = TRUE;}
	[ action		{ first_action = FALSE;}
	  [ ';' action]*
	]?
	'.'
	;

action	: CALL			{ print_call( &yytext[0]);}

	| as_block		{ last_action = ( token == '.');
				  do_block_assemble();}

	;

as_block :			{ init_as_block();}
	ASSEM_INSTR 		{ save_as( &yytext[0]);}
	more_as
	;

more_as :
	  %if ( next_token() == ASSEM_INSTR)
	  ';' 
	  ASSEM_INSTR 		{ save_as( &yytext[0]);}
	  more_as
	|
	;

Dspecial: CONDITION 		{ out( " %s ", yytext);}
	  Dsimple 		

	  [ CONDITION 		{ out( " %s ", yytext);}
	    Dsimple 		
 	  ]*

	  DEFAULT 		{ out( " %s ", yytext);}
	  Dsimple 		
	;

Dsimple	: ARROW 		{ out( "%s", yytext);}
	  Dactionlist

	| EQUIV			{ out( "%s", yytext);}
	  Dactionlist		

	;

Dactionlist : 
	  [ Daction
	    [ ';'		{ out( ";\n");}
	      Daction
	    ]*
	  ]?
	  '.' 			{ out( ".\n");}
	;

Daction	: CALL			{ out( "%s", yytext);}

	| ASSEM_INSTR 		{ out( "\"%s\"", yytext);}

	;

def_row	: [ special | simple]	{ out( "}\n\n");}
	;

c_table	: c_row*
	;

c_row	: %if ( strcmp( yytext, to_change) == 0)
	  C_INSTR 		{ set_outfile( yytext); header( yytext);}
	  [ special | simple]	{ out( "}\n\n"); exit(0);}

	| C_INSTR
	  [ c_special | c_simple]

	| %if ( strcmp( yytext, to_change) == 0)
	  DEF_C_INSTR		{ init_defaults( yytext);}
	  [ Dspecial | Dsimple] { handle_defaults();exit(0);}

	| DEF_C_INSTR
	  [ c_special | c_simple]
	;

c_special : CONDITION
	  c_simple

	  [ CONDITION
	    c_simple
 	  ]*

	  DEFAULT
	  c_simple
	;

c_simple: ARROW
	  c_actionlist

	| EQUIV
	  c_actionlist
	;

c_actionlist : 
	  [ c_action [ ';' c_action]* ]? '.' 
	;

c_action: CALL
	| ASSEM_INSTR
	;


{

int saved = 0, token;


LLmessage( inserted_token)
int inserted_token;
{
	if ( inserted_token == 0) {
		fprint( STDERR, "EM_table : syntax error in line %d, >>",
			yylineno);
		print_token( LLsymb);
		fprint( STDERR, "<<  will be deleted!!\n");
	}
	else if ( inserted_token < 0) {
		fprint(STDERR,"EM_table : stack overflow in line %d (fatal)!\n",
			 yylineno);
		exit( 1);
	}
	else {
		fprint( STDERR, "EM_table : syntax error in line %d, >>",
			yylineno);
		print_token( inserted_token);
		fprint( STDERR, "<<  will be inserted!!\n");
		token = LLsymb;
		saved = 1;
	}
}

print_token( token)
int token;
{
	switch ( token) {
	  case C_INSTR 	: fprint( STDERR,  "C_INSTR  %s", yytext);
			  break;
	  case ASSEM_INSTR : fprint( STDERR,  "STRING  %s", yytext);
			  break;
	  case CALL	: fprint( STDERR,  "CALL  %s", yytext);
			  break;
	  case ARROW	: fprint( STDERR,  "==> ");
			  break;
	  case EQUIV	: fprint( STDERR,  "::= ");
			  break;
	  case CONDITION: fprint( STDERR,  "CONDITION  %s", yytext);
			  break;
	  case DEFAULT	: fprint( STDERR,  "default ");
			  break;
	  case ERROR	: fprint( STDERR,  "unmatched  %s", yytext);
			  break;
	  default	: fprint( STDERR, " %c", token);
			  break;
	}
}

int lex_analyzer()
{
	extern char *next;

	if ( saved) {
		saved = 0;
	}
	else {
		token = mylex();
		*next = '\0';
	}
	if ( token == C_INSTR)
		set_C_instr_info( yytext);
	return( token);
}

int next_token()
{
	extern char *next;

	if ( !saved) {
		token = mylex();
		*next = '\0';
		saved = 1;
	}
	return( token);
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
	  case SEGHOL  : out( "switchseg( SEGHOL); BESTAAT NIET!!\n");
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


out( fmt, argv)
char *fmt;
int argv;
{
	doprnt( outfile, fmt, &argv);
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
		out( "#define CODE_EXPANDER\n");
		out( "#include <em.h>\n");
		out( "#include \"mach.h\"\n");
		out( "#include \"back.h\"\n\n");
	}
}


main( argc, argv)
int argc;
char **argv;
{
	outfile = STDOUT;
	if ( argc > 1) {
		if ( strcmp( argv[1], "-l") == 0)
			library = TRUE;
		else if( strcmp( argv[1], "-c") == 0) {
			library = TRUE;
			to_change = argv[2];
			c_table();
			fprint( STDERR, "No rule for %s\n", to_change);
			return( 1);
		}
	}
	else {
		library = FALSE;
		out( "#define CODE_EXPANDER\n");
		out( "#include <em.h>\n");
		out( "#include \"mach.h\"\n");
		out( "#include \"back.h\"\n\n");
	}
	
	return( table());
}

}
