#
 mes 2,EM_WSIZE,EM_PSIZE

;
; layout of a setjmp buffer:
;
;  -----------------
; |   signal mask   |		(only for Berkeley 4.[2-])
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

; a GTO descriptor must be in the global data area
gtobuf
 bss 3*EM_PSIZE,0,0

 inp $fill_ret_area
 exp $setjmp
 pro $setjmp,0
#ifdef __BSD4_2
; save mask of currently blocked signals. 
; longjmp must restore this mask
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
 loi EM_PSIZE	; Return address of caller
 lal 0
 loi EM_PSIZE	; address of jmpbuf
 sti 3*EM_PSIZE	; LB, SP, and PC stored in jmpbuf
 loc 0
 ret EM_WSIZE	; setjmp must return 0
 end 0

 pro $fill_ret_area,0
; put argument in function result area
 lol 0
 ret EM_WSIZE
 end 0

 exp $longjmp
 pro $longjmp,?
#ifdef __BSD4_2
; restore signal mask
 lal 0
 loi EM_PSIZE
 lof 3*EM_PSIZE
 cal $sigsetmask
 asp EM_WSIZE
 lfr EM_WSIZE
 asp EM_WSIZE
#endif
 lal 0
 loi EM_PSIZE	; address of jmpbuf
 lae gtobuf
 blm 3*EM_PSIZE	; fill GTO descriptor from jmpbuf
 lol EM_PSIZE	; second parameter of longjmp: the return value
 dup EM_WSIZE
 zne *3
; of course, longjmp may not return 0!
 asp EM_WSIZE
 loc 1
3
; put return value in function result area
 cal $fill_ret_area
 asp EM_WSIZE
 gto gtobuf	; there we go ...
; ASP and GTO do not damage function result area
 end 0
