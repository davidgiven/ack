/* This file contains the description of the 'as_table' parser.
 * It transforms every entry into a C-funtion, for example :
 *
 *	and dst:REG, src:EADDR  ==>     @text1( 0x23);
 *					mod_RM( dst->reg, src).
 *
 *	... dst:ACCU, src:DATA  ==>     @text1( 0x25);
 *					@text2(
 *					%$(src->expr)).
 *
 * Will be transformed into :
 *
 * 	and_instr( dst, src)
 *	struct t_operand *dst, *src;
 *	{
 *		if (  REG( dst) && EADDR( src)) {
 *			 cur_pos += 1;
 *			 fprint( outfile, "text1( 0x23)");
 *			 fprint( outfile, ";");
 *			 mod_RM( dst->reg, src);
 *		}
 *		else if ( ACCU( dst) && DATA( src)) {
 *			cur_pos += 1;
 *			fprint( outfile, "text1( 0x25)");
 *			fprint( outfile, ";");
 *			cur_pos += 2;
 *			fprint( outfile, "text2( ");
 *			eval( src->expr);
 *			fprint( outfile, ")");
 *			fprint( outfile, ";");
 *		}
 *		else
 *			error( "No match for and");
 *	}
 * 
 * At the end of the table a list is generated enumerating all the assembler
 * mnemonics and their corresponding function names.
 */

{

#include "decl.h"
extern int lineno, yyleng;
#ifdef FLEX
extern char *yytext;
#else
extern char yytext[];
#endif

}

%token IDENTIFIER, CALL, CONDITION, IF, ELSIF, ELSE, FI, ARROW, MORE;
%start	table, table;
%lexical	lex_analyzer ;


table		: 			{ init_table();}
		  instruction* 		{ end_table();}
		;

instruction	: 			{ clean();}
		  first_row
		  [ 			{ operand_clean();}
		    extra_row
		  ]*			{ pr_warning(); out( "}\n\n");}
		;

first_row	: mnemonic		{ save_instr( yytext, yyleng);}
		  [ decl_list]?
		  ARROW			{ pr_header(); pr_restriction();}
		  action_list
		;

extra_row	: MORE
		  [ decl_list]?
		  ARROW			{ out( "else "); pr_restriction();}
		  action_list
		;

mnemonic	: IDENTIFIER
		;

decl_list	: 			{ clear_restriction();}
		  declaration
		  [ ',' declaration] *7
		;

declaration	: IDENTIFIER		{ save_name( yytext, yyleng);}
		  [ ':'
		    IDENTIFIER		{ save_type( yytext, yyleng);}
		  ]?			{ inc_ops();}
		;

action_list	: 					{ out( "{\n");}
		  [ action [ ';' action]* ]? '.'	{ out( "}\n");}
		;

action		: if_statement
		| call			
		| subroutine
		;

/* A function call is just an identifier followed by an expression surrounded
 * by '(' and ')'. CONDITION is a token that matches this construct;
 */

subroutine
  { char *s; }	: IDENTIFIER		{ s = Salloc(yytext, yyleng+1); }
		  CONDITION		{ s = Realloc(s, strlen(s)+yyleng+1);
					  strcat(s, yytext);
					  pr_subroutine( s);
					  free(s);
					}
		;

call
  { char *s; }	: '@'
		  IDENTIFIER		{ s = Salloc(yytext, yyleng+1); }
		  CONDITION		{ s = Realloc(s, strlen(s)+yyleng+1);
					  strcat(s, yytext);
					  pr_call( s);
					  free(s);
					}
		;

if_statement	: IF
		  CONDITION		{ pr_question( yytext);}
		  action_list		{ pr_end();}
		  [ ELSIF 		{ pr_els();}
		    CONDITION		{ pr_question( yytext);}
		    action_list		{ pr_end();}
		  ]*
		  [ ELSE		{ pr_else();}
		    action_list		{ pr_end();}
		  ]?
		  FI
		;

{

int nerrors;
static int saved = 0, token;


LLmessage( inserted_token)
int inserted_token;
{
	nerrors++;
	if ( inserted_token == 0) {
		fprint( STDERR, "Sytax error in line %d, ", lineno);
		print_token( LLsymb);
		fprint( STDERR, "  will be deleted!!\n");
	}
	else if ( inserted_token < 0) {
		fprint( STDERR, "Garbage at end, line %d!!\n",
			 lineno);
	}
	else {
		fprint( STDERR, "Sytax error in line %d, ", lineno);
		print_token( inserted_token);
		fprint( STDERR, "  will be inserted!!\n");
		token = LLsymb;
		saved = 1;
	}
}

print_token( token)
int token;
{
	switch ( token) {
	  case IDENTIFIER : fprint( STDERR,  "IDENTIFIER %s", yytext);
			  break;
	  case CALL	: fprint( STDERR,  "CALL  %s", yytext);
			  break;
	  case CONDITION: fprint( STDERR,  "CONDITION  %s", yytext);
			  break;
	  case IF	: fprint( STDERR,  "@if ");
			  break;
	  case ELSIF	: fprint( STDERR,  "@elsif ");
			  break;
	  case ELSE	: fprint( STDERR,  "@else ");
			  break;
	  case FI	: fprint( STDERR,  "@fi ");
			  break;
	  case ARROW	: fprint( STDERR,  "==> ");
			  break;
	  case MORE	: fprint( STDERR,  "... ");
			  break;
	  default	: fprint( STDERR, "%c ", token);
			  break;
	}
}

int lex_analyzer()
{
	int tok;

	if ( saved) {
		saved = 0;
		return( token);
	}
	else {
		tok = yylex();
		yytext[yyleng] = '\0'; /* strings must end with '\0' */
		return( tok);
	}
}

main()
{
	table();
	exit(nerrors);
}

}
