#define PROC    0

; $Header$
 mes 2,EM_WSIZE,EM_PSIZE

; _sig is called with one parameter:
;       - procedure instance identifier (PROC)
; and returns nothing.
; only the procedure identifier inside the PROC is used.

 exp $_sig
 pro $_sig,0
 lal PROC
 loi EM_PSIZE
 sig
 ret 0                  ; ignore the result of sig
 end ?
