.define .dvu4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 4-byte divide routine for z80
! parameters:
!    stack: divisor
!	    dividend
!    stack: quotient (out)
!    bc de: remainder (out)  (high part in bc)



! a n-byte divide may be implemented
! using 2 (virtual) registers:
!  - a n-byte register containing
!    the divisor
!  - a 2n-byte shiftregister (VSR)
!
! Initially, the VSR contains the dividend
! in its low (right) n bytes and zeroes in its
! high n bytes. The dividend is shifted
! left into a "window" bit by bit. After
! each shift, the contents of the window
! is compared with the divisor. If it is
! higher or equal, the divisor is subtracted from
! it and a "1" bit is inserted in the
! VSR from the right side; else a "0" bit
! is inserted. These bits are shifted left
! too during subsequent iterations.
! At the end, the rightmost part of VSR
! contains the quotient.
! For n=4, we need 2*4+4 = 12 bytes of
! registers. Unfortunately we only have
! 5 2-byte registers on the z80
! (bc,de,hl,ix and iy). Therefore we use
! an overlay technique for the rightmost
! 4 bytes of the VSR. The 32 iterations
! are split up into two groups: during
! the first 16 iterations we use the high
! order 16 bits of the dividend; during
! the last 16 iterations we use the
! low order 16 bits.
! register allocation:
!   VSR        iy hl ix
!   divisor   -de bc
.dvu4:
	! initialization
	pop hl		! save return address
	ld (.retaddr),hl
	pop bc		! low part (2 bytes)
			! of divisor in bc
	xor a		! clear carry, a := 0
	ld h,a		! hl := 0
	ld l,a
	ld (.flag),a	! first pass main loop
	pop de		! high part divisor
	sbc hl,de	! inverse of high part
	ex de,hl	! of divisor in de
	pop hl		! save low part of
			! dividend in memory
	ld (.low),hl	! used during second
			! iteration over main loop
	pop ix		! high part of dividend
	push iy		! save LB
	ld h,a		! hl := 0
	ld l,a
	ld iy,0		! now the VSR is initialized

	! main loop, done twice
1:
	ld a,16
	! sub-loop, done 16 times
2:
	add iy,iy	! shift VSR left
	add ix,ix
	adc hl,hl
	jp nc,3f
	inc iy
3:
	or a		! subtract divisor from
			! window (iy hl)
	ld (.iysave),iy
	sbc hl,bc
	jr nc,4f	! decrement iy if there
			! was no borrow
	dec iy
4:
	add iy,de	! there is no "sbc iy,ss"
			! on the z80, so de was
			! inverted during init.
	inc ix
	! see if the result is non-negative,
	! otherwise undo the subtract.
	! note that this uncooperating machine
	! does not set its S -or Z flag after
	! a 16-bit add.
	ex (sp),iy	! does anyone see a better
	ex (sp),hl	! solution ???
	bit 7,h
	ex (sp),hl
	ex (sp),iy
	jp z,5f
	! undo the subtract
	add hl,bc
	ld iy,(.iysave)
	dec ix
5:
	dec a
	jr nz,2b
	ld a,(.flag)	! see if this was first or
			! second iteration of main loop
	or a		! 0=first, 1=second
	jr nz,6f
	inc a		! a := 1
	ld (.flag),a	! flag := 1
	ld (.result),ix ! save high part of result
	ld ix,(.low)	! initialize second
			! iteration, ix := low
			! part of dividend
	jr 1b
6:
	! clean up
	push iy		! transfer remainder
	pop bc		! from iy-hl to bc-de
	ex de,hl
	pop iy		! restore LB
	ld hl,(.result) ! high part of result
	push hl
	push ix		! low part of result
	ld hl,(.retaddr)
	jp (hl)		! return

.sect .data
.flag:		.data1 0
.low:		.data2 0
.iysave:	.data2 0
.retaddr:	.data2 0
.result:	.data2 0
