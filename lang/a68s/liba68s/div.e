#include "e.h"

 exp $DIV

 pro $DIV,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 lol SZADDR+SZWORD	; 1st param
 loc SZWORD
 loc SZREAL
 cif
 lol SZADDR		; 2nd param
 loc SZWORD
 loc SZREAL
 cif
 dvf SZREAL
 ret SZREAL
 end SZWORD
