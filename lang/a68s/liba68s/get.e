#include "e.h"

 exa _1GETT		; 1st label in GETT (run68d)
 exp $GET
 exp $READ
 exp $GETT
 exp $STANDINC

 ina jumpdesc
jumpdesc
 con 0I SZADDR,0I SZADDR,0I SZADDR	; enough space for 3 pointers

 pro $GET,GETTVARSPACE
 mes 11
 loc GETSTAMP
 stl -SZWORD		; set up frame stamp
 lxa 0			; load argument base
 lol SZADDR+SZADDR	; load length of data lost, skip static link & space
 loc SZADDR+SZADDR+SZWORD
 adu SZWORD		; add on space for static link & file pointer & count
 ads SZWORD		; add argument base and offset
 loi SZADDR		; load file address, objectp
 SFL SZADDR		; store in space, left for this reason
 lor 1			; fill in jump info with SP
 SFE jumpdesc+SZADDR
 lxl 0			; and LB
 SFE jumpdesc+SZADDR+SZADDR
 LFE _1GETT-ENTRYOFFSET ; and code entry point
 SFE jumpdesc
 gto jumpdesc		; jump to GETT, in run68d
 end GETTVARSPACE

 pro $READ,GETTVARSPACE
 mes 11
 loc GETSTAMP
 stl -SZWORD		; set up frame stamp
 LFE .HTOP-STINOFFSET	; address of stout in global frame
 SFL SZADDR		; store in first param after static link
 lor 1			; fill in jump info with SP
 SFE jumpdesc+SZADDR
 lxl 0			; and LB
 SFE jumpdesc+SZADDR+SZADDR
 LFE _1GETT-ENTRYOFFSET ; and code entry point
 SFE jumpdesc
 gto jumpdesc		; jump to GETT, in run68d
 end GETTVARSPACE

 pro $STANDINC,SZWORD			; call to stinch (run68d)
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR+SZADDR			; param 1, pcov
 LFL SZADDR				; param 2, lfn
 LFL 0					; static link
 cal $STINCH
 asp SZADDR+SZADDR+SZADDR
 ret 0
 end SZWORD

