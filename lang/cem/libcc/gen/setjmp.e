#
 mes 2,EM_WSIZE,EM_PSIZE

;
; layout of a setjmp buffer:
;
;  -----------------
; |   signal mask   |		(only for Berkeley 4.2)
;  -----------------
; |   PC of caller  |
;  -----------------
; |                 |
; |  GTO descriptor |
; |   (SP, LB, PC)  |
; |                 |
;  -----------------
;
; setjmp saves the signalmask, PC, SP, and LB of caller, and creates a
; GTO descriptor with a program counter indicating a piece of code that
; gets the return value right before jumping to the caller of setjmp in
; the setjmp buffer, which has a layout as displayed above.
; The big problem here is how to het the return address, i.e. the PC of
; the caller; This problem is solved by the front-end, which must pass
; it as an extra parameter to setjmp.

gtobuf
 bss 3*EM_PSIZE,0,0
retval
 bss EM_WSIZE,0,0
jmpbuf_addr
 bss EM_PSIZE,0,0

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
 stf 4*EM_PSIZE
#endif
 ; create GTO descriptor for longjmp
 lxl 0
 dch		; Local Base of caller
 lxa 0		; Stackpointer of caller
.1
 rom *1
 lae .1
 loi EM_PSIZE	; where the longjmp ends up
 lal 0
 loi EM_PSIZE
 sti 3*EM_PSIZE	; stored in jmpbuf
 ; also save return address, which has been supplied by the compiler
 lal EM_PSIZE
 loi EM_PSIZE
 lal 0
 loi EM_PSIZE
 adp 3*EM_PSIZE
 sti EM_PSIZE
 loc 0
 ret EM_WSIZE
1
 ; so, the longjmp returns here.
 ; now, create a GTO descriptor for the final GTO
 lae jmpbuf_addr
 loi EM_PSIZE
 adp 3*EM_PSIZE
 loi EM_PSIZE
 lae gtobuf
 sti EM_PSIZE	; this is the return address
 lae jmpbuf_addr
 loi EM_PSIZE
 adp EM_PSIZE
 loi 2*EM_PSIZE
 lae gtobuf+EM_PSIZE
 sti 2*EM_PSIZE	; that's it, the GTO descriptor is ready;
		; now take care of the return value ...
 cal $fill_ret_area
 gto gtobuf	; there we go ...
 end 0

 pro $fill_ret_area,0
 loe retval
 ret EM_WSIZE
 end 0

 exp $longjmp
 pro $longjmp,?
 lal 0
 loi EM_PSIZE
#ifdef __BSD4_2
 dup EM_PSIZE
 lof 4*EM_PSIZE
 cal $sigsetmask
 asp EM_WSIZE
 lfr EM_WSIZE
 asp EM_WSIZE
#endif
 lae jmpbuf_addr
 sti EM_PSIZE	; save address of jmpbuf
 lol EM_PSIZE
 dup EM_WSIZE
 zne *3
		; of course, longjmp may not return 0!
 asp EM_WSIZE
 loc 1
3
 ste retval	; save return value
 lal 0
 loi EM_PSIZE
 lae gtobuf
 blm 3*EM_PSIZE	; create GTO descriptor
 gto gtobuf	; and GTO
 end 0
