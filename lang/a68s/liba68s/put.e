#include "e.h"
 exa _1PUTT		; 1st label in PUTT (run68d)
 exp $PUT
 exp $PRINT
 exp $WRITE
 exp $PUTT
 exp $NXTBIT
 exp $STANDOUT

 ina jumpdesc
jumpdesc
 con 0I SZADDR,0I SZADDR,0I SZADDR	; enough space for 3 pointers

 pro $PUT,PUTTVARSPACE
 mes 11
 loc PUTSTAMP
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
 LFE _1PUTT-ENTRYOFFSET ; and code entry point
 SFE jumpdesc
 gto jumpdesc		; jump to PUTT, in run68d
 end PUTTVARSPACE

 pro $PRINT,PUTTVARSPACE
 mes 11
 loc PUTSTAMP
 stl -SZWORD		; set up frame stamp
 LFE .HTOP-STOUTOFFSET; address of stout in global frame
 SFL SZADDR		; store in first param after static link
 lor 1			; fill in jump info with SP
 SFE jumpdesc+SZADDR
 lxl 0			; and LB
 SFE jumpdesc+SZADDR+SZADDR
 LFE _1PUTT-ENTRYOFFSET ; and code entry point
 SFE jumpdesc
 gto jumpdesc		; jump to PUTT, in run68d
 end PUTTVARSPACE

 pro $WRITE,PUTTVARSPACE
 mes 11
 loc PUTSTAMP
 stl -SZWORD		; set up frame stamp
 LFE .HTOP-STOUTOFFSET; address of stout in global frame
 SFL SZADDR		; store in first param after static link
 lor 1			; fill in jump info with SP
 SFE jumpdesc+SZADDR
 lxl 0			; and LB
 SFE jumpdesc+SZADDR+SZADDR
 LFE _1PUTT-ENTRYOFFSET ; and code entry point
 SFE jumpdesc
 gto jumpdesc		; jump to PUTT, in run68d
 end PUTTVARSPACE

 pro $NXTBIT,SZWORD	; FUNCTION(VAR N: INTEGER): INTEGER;
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR		; address of N
 loi SZWORD
 dup SZWORD
 loc 1
 slu SZWORD		; shift left
 LFL SZADDR
 sti SZWORD		; replace shifted N
 tlt			; test original N
 ret SZWORD		; return old top bit of N
 end SZWORD

 pro $STANDOUT,SZWORD			; call to stoutch (run68d)
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR+SZADDR			; param 1, pcov
 LFL SZADDR				; param 2, lfn
 LFL 0					; static link
 cal $STOUTCH
 asp SZADDR+SZADDR+SZADDR
 ret 0
 end SZWORD

