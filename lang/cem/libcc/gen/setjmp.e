#
 mes 2,EM_WSIZE,EM_PSIZE

;
; layout of a setjmp buffer:
;
;  -----------------
; |   signal mask   |		(only for Berkeley 4.2)
;  -----------------
; |                 |
; |  GTO descriptor |
; |   (SP, LB, PC)  |
; |                 |
;  -----------------
;
; setjmp saves the signalmask, PC, SP, and LB of caller, and creates a
; GTO descriptor from this.
; The big problem here is how to get the return address, i.e. the PC of
; the caller; This problem is solved by the front-end, which must pass
; it as an extra parameter to setjmp.

gtobuf
 bss 3*EM_PSIZE,0,0

 inp $fill_ret_area
 exp $setjmp
 pro $setjmp,0
 mes 11
#ifdef __BSD4_2
 loc 0
 cal $sigblock
 asp EM_WSIZE
 lfr EM_WSIZE
 lal 0
 loi EM_PSIZE
 stf 3*EM_PSIZE
#endif
 ; create GTO descriptor for longjmp
 lxl 0
 dch		; Local Base of caller
 lxa 0		; Stackpointer of caller
 lal EM_PSIZE
 loi EM_PSIZE
 lal 0
 loi EM_PSIZE
 sti 3*EM_PSIZE	; stored in jmpbuf
 loc 0
 ret EM_WSIZE
 end 0

 pro $fill_ret_area,0
 lol 0
 ret EM_WSIZE
 end 0

 exp $longjmp
 pro $longjmp,?
 lal 0
 loi EM_PSIZE
#ifdef __BSD4_2
 dup EM_PSIZE
 lof 3*EM_PSIZE
 cal $sigsetmask
 asp EM_WSIZE
 lfr EM_WSIZE
 asp EM_WSIZE
#endif
 lae gtobuf
 blm 3*EM_PSIZE
 lol EM_PSIZE
 dup EM_WSIZE
 zne *3
		; of course, longjmp may not return 0!
 asp EM_WSIZE
 loc 1
3
 cal $fill_ret_area
 asp EM_WSIZE
 gto gtobuf	; there we go ...
 end 0
