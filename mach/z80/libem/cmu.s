.define .cmu
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! parameters:
!   hl   :  size (#bytes)
!   stack:  second operand
!	    first operand
!   stack:  result (out)



.cmu:
	! The two operands are compared byte by byte,
	! starting at the highest byte, until
	! they differ.
	pop ix		! return address
	pop hl		! #bytes
	ld b,h		! bc := hl
	ld c,l
	add hl,sp
	dec hl		! pointer to highest byte
			! of second operand
	ld d,h		! de := hl
	ld e,l
	add hl,bc	! pointer to highest byte
			! of first operand
	ld sp,hl	! points to where the
			! result will be stored
	ex de,hl
	! now, de points to highest byte of 1st operand
	!      sp   ,,		,,		,,
	!      hl   ,,		,,	    2nd	,,
	! bc contains #bytes

0:
	! loop, compare the two operands
	! byte by byte.
	ld a,(de)
	xor (hl)	! Avoid overflow during
			! subtraction. If the
			! signbits differ, then
			! the operands differ.
	jp m,2f		! signbits differ
	ld a,(de)	! signbits are equal,
			! so we can savely
			! compare the bytes.
	sub (hl)
	jr nz,1f	! operands are different
	dec de		! the two bytes are the
			! same, try next bytes,
			! if any.
	dec hl		! bump pointers
	dec bc
	ld a,b		! bc = 0 ?
	or c
	jr nz,0b	! no, try next bytes
	! yes, then the two operands are equal.
	! Note that a=0 now.
1:
	ld h,a		! hl := result
	ld l,a
	jr 3f
2:
	! the signbits differ
	ld h,(hl)	! hl := positive if
			! signbit of current
			! byte of 2nd operand
			! is "0", else negative
	ld l,1		! just in case (hl)=0
3:
	ex (sp),hl	! sp was set above
	jp (ix)		! return
