
ws		([ \t]*)
arrow		("==>")
letter		([a-zA-Z_])
digit		([0-9])
identifier	({letter}({letter}|{digit})*)

%start COM CAL CON

%{

#include "Lpars.h"
#include "decl.h"
int special, n_haakjes;
int lineno = 1;

%}


%%

"/*"			BEGIN COM;
<COM>[^\n\*]*"*"+"/"	BEGIN 0;
<COM>[^\n^\*]*"*"+	;
<COM>[^\n\*]*"\n"	lineno++;

";"			return( ';');
"."			return( '.');
","			return( ',');
":"			return( ':');
"@"			return( '@');
"@if"{ws}		return( IF);
"@elsif"{ws}		return( ELSIF);
"@else"{ws}		return( ELSE);
"@fi"{ws}		return( FI);
"..."			return( MORE);
{arrow}			return( ARROW);
{identifier}		return( IDENTIFIER);


<CON>[^()]*"("  	{ n_haakjes++; yymore();}
"("			{ BEGIN CON; n_haakjes = 1; yymore();}
<CON>[^()]*")"  	{ if ( n_haakjes == 1) {
				BEGIN 0;
				return( CONDITION);
		  	  }
		  	  else {
				n_haakjes--;
				yymore();
		  	  }
			}

{ws}			;
\n			lineno++;
