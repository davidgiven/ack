#include "e.h"

 exp $WRC
 exp $WRS

 pro $WRC,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR		; first param after static link, fil
 lol SZADDR+SZADDR	; second param ,ch
 cal $_wrc		; call to PC run time system
 asp SZADDR+SZWORD
 ret 0
 end SZWORD

 pro $WRS,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR		; first param after static link ,fil
 LFL SZADDR+SZADDR	; second param ,cp
 lol SZADDR+SZADDR+SZADDR ; third param ,len
 cal $_wrs
 asp SZADDR+SZADDR+SZWORD
 ret 0
 end SZWORD
