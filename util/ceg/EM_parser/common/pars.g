/* This file contains the parser for the 'EM_table'.
 * Every row in the table is converted to a C-function.
 * The parser is a fairly simple program, for each row it prints a function-
 * header and the following actions are printed as C-statements. A CALL is just
 * copied, and an ASSEM_INSTR will be handed to the routine assemble().
 * Assemble() will turn the assembly-string into a sequence of C-function calls.
 * How this should be done is expressed in the 'as_table'.
 *
 * There are however some complicating factors:
 *	- A row in the 'EM_table' may contain CONDITION's. 
 *	  The parser supports this by outptutting some "if" and "else"'s.
 *
 *	- The user is allowed to use some abbreviation in the 'EM_table', (s)he
 *	  can use the DEF_C_INSTR's ( e.g. C_loe..).
 *	  One such DEF_C_INSTR must be expanded in 3 or more C-functions.
 *	  The solution is to copy the DEF_C_INSTR to a temporary file. Then
 *	  this file will be processed as many times as needed to generate all
 *	  the C-functions.
 *
 *	- The assembler-instructions must be processed in blocks not one at a
 *	  time, so buffering is needed.
 *
 *	- When generating object-code, the user can make use of the dist()
 * 	  function. This function should return the number of bytes between
 *	  the current instruction and a labelled one, both forward and backward
 *	  references must be handled.
 *	  This requires the saving of the generated function, and 
 *	  backpatching in a second phase.
 */
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


/* Grammar rules for a default-instruction, just copy to a temporary file. */

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



/* This is a grammar to handle the -c flag ( only one entry has changed).
 * Skip all entries in the table until the one that has changed, then
 * use the grammar rules for the normal case above.
 */

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


/* The lexical analyzer.
 *	It uses mylex() to get the tokens.
 *	It supports the buffering of one token.
 *	If the curent token is a C_INSTR it will set the global variable
 *	'C_instr_info' to point to the correct information ( arguments, header,
 *	etc.).
 */

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


/******************************************************************************/

/* Handle arguments :
 *	-l 		: Library, generate one function per file.
 *	-c <name>	: Change, only update the <name> file.
 */

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
		file_header();
	}
	
	return( table());
}

}
