#include "e.h"

 exp $HOIST

 pro $HOIST,SZADDR	; used to balance the amount of space on the stack
			; for a call to PUT or PRINT. This is done by 
			; loading an amount of dummy data (-1).
			; The parameter is the amount of dummy space needed.

 lxa 0			; base address of params
 lol SZADDR		; param, after static link, the difference
 loc SZADDR+SZWORD	; diff calculated from after param and static link
 adu SZWORD		; add last two
 ads SZWORD		; add total to arg base
 dup SZADDR		; this is the address of 'count'
 SFL -SZADDR		; save for later use
 loi SZWORD		; load count
 lol SZADDR		; load difference
 adu SZWORD		; new count
 stl SZADDR+SZWORD	; store in new place, at bottom of dummy data
1
 loc -1			; dummy data to PRINT
 LFL -SZADDR		; address to place data, initialy where count was
 dup SZADDR
 adp -SZWORD		; reduce pointer by SZWORD ready for next time
 SFL -SZADDR		; re-save
 sti SZWORD		; place -1 in target address
 lol SZADDR		; use difference as a loop counter now
 loc SZWORD
 sbi SZWORD		; reduce loop counter by SZWORD
 dup SZWORD		; need one to store and one to test
 stl SZADDR		; re-save
 zgt *1			; do next SZWORD block
 ret 0
 end SZADDR
