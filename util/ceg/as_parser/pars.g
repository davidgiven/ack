{

#include "decl.h"
extern int yylineno, yyleng, yymorfg;
extern char yytext[];

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

subroutine	: IDENTIFIER		{ yymorfg=1;}
		  CONDITION		{ pr_subroutine( yytext);}
		;

call		: '@'
		  IDENTIFIER		{ yymorfg=1;}
		  CONDITION		{ pr_call( yytext);}
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

static int saved = 0, token;


LLmessage( inserted_token)
int inserted_token;
{
	if ( inserted_token == 0) {
		fprint( STDERR, "Sytax error in line %d, ", yylineno);
		print_token( LLsymb);
		fprint( STDERR, "  will be deleted!!\n");
	}
	else if ( inserted_token < 0) {
		fprint( STDERR, "Stack overflow in line %d, fatal error!\n",
			 yylineno);
		exit( 1);
	}
	else {
		fprint( STDERR, "Sytax error in line %d, ", yylineno);
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
		yytext[yyleng] = '\0'; /* strings moeten op een '\0' eindigen */
		return( tok);
	}
}

main()
{
	table();
	return( 0);
}

}
