#
 mes 2,EM_WSIZE,EM_PSIZE

 exp $setjmp
 pro $setjmp,2*EM_WSIZE

; setjmp saves the StackPointer and the LocalBase, and the chunk of
; memory between the StackPointer and the ArgumentBase, + its size in a
; buffer, pointed to by the parameter.
; longjump can then restore this buffer and return.
; Notice that this does not work on EM implementations in which every
; procedure frame has a different fragment in memory, because in this case
; the ArgumentBase will point into the fragment of the caller.
; What really is needed is a way to find out the size of the return
; status block.
; On EM-implementations in which the stack grows upwards it is untested,
; as there are no such implementations available now.
; This implementation of setjmp/longjmp
; depends on the assumption that the routine calling
; setjmp does not have register variables, and that it saves all registers
; that are available for variables.

 loc 0
 stl -2*EM_WSIZE
 lor 1		; load StackPointer
 lal 0
 loi EM_PSIZE
 sti EM_PSIZE	; save it
 lxl 0		; load LocalBase
 lal 0
 loi EM_PSIZE
 adp EM_PSIZE
 sti EM_PSIZE	; save it
 lxa 0		; load ArgumentBase
 lal 0
 loi EM_PSIZE
 loi EM_PSIZE	; load saved StackPointer
 sbs EM_WSIZE	; gives size of block that is to be saved, or negative size
 dup EM_WSIZE
 zgt *5
 ngi EM_WSIZE
 loc 1
 stl -2*EM_WSIZE; one if the stack grows upwards
5
 stl -EM_WSIZE	; save size of block in local
 lol -EM_WSIZE
 lal 0
 loi EM_PSIZE
 adp 2*EM_PSIZE
 sti EM_WSIZE	; and also in the buffer
 lal 0
 loi EM_PSIZE
 loi EM_PSIZE	; load saved StackPointer
 lol -2*EM_WSIZE; positive if the stack grows upwards
 zle *6
 asp EM_PSIZE
 lxa 0
 adp EM_WSIZE	; in this case the source address = ArgumentBase+EM_WSIZE
6
 lal 0
 loi EM_PSIZE
 adp 2*EM_PSIZE+EM_WSIZE
		; destination address
 lol -EM_WSIZE	; count
 bls EM_WSIZE	; block copy
 loc 0
 ret EM_WSIZE
 end 2*EM_WSIZE

 exp $longjmp
 pro $longjmp,2*EM_WSIZE

; first, find out wether the stack grows upwards
 loc 0
 stl -2*EM_WSIZE
 lxa 0
 lxl 0
 cmp
 zge *7
 loc 1
 stl -2*EM_WSIZE; this local contains 1 if it does, otherwise it contains 0
7
; then, adjust StackPointer until it is below the saved StackPointer (or
; above if it grows upwards)
; then push parameters
; then copy the saved block in its proper place
; notice that the parameters can not be used anymore now
; then restore the LocalBase and the Stackpointer and return.
1
 loc 0
2
 lor 1
 lal 0
 loi EM_PSIZE
 loi EM_PSIZE	; saved StackPointer
 cmp		; compare with current one
 lol -2*EM_WSIZE
 zle *8
 zlt *1
 bra *10
8
 zgt *1
10
 lal 0
 loi EM_PSIZE
 loi EM_PSIZE	; push saved StackPointer
 lol EM_PSIZE	; push value to be returned by longjmp
 lal 0
 loi EM_PSIZE
 adp EM_PSIZE
 loi EM_PSIZE	; push saved LocalBase
 lal 0
 loi EM_PSIZE
 adp 2*EM_PSIZE+EM_WSIZE
		; source address
 lal 0
 loi EM_PSIZE
 loi EM_PSIZE	; saved stackpointer
 lol -2*EM_WSIZE
 zle *9		; if not positive, this is the destination address,
		; otherwise subtract the size of the saved area and add EM_WSIZE
 adp EM_WSIZE
 lal 0
 loi EM_PSIZE
 adp 2*EM_PSIZE
 loi EM_WSIZE
 ngi EM_WSIZE
 ads EM_WSIZE
9		; destination address
 lal 0
 loi EM_PSIZE
 adp 2*EM_PSIZE
 loi EM_WSIZE	; size
 bls EM_WSIZE	; now we have a frame exactly as it was in setjmp,
		; and exactly at the same place
 str 0		; restore LocalBase
 stl -EM_WSIZE	; saves the return value
 str 1		; restores the StackPointer
 lol -EM_WSIZE
 dup EM_WSIZE
 zne *3
		; of course, longjmp may not return 0!
 asp EM_WSIZE
 loc 1
3
 ret EM_WSIZE
 end 2*EM_WSIZE
