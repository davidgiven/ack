.define .mli4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 32-bit multiply routine for z80
! parameters:
!   on stack



! register utilization:
!   ix: least significant 2 bytes of result
!   hl: most  significant 2 bytes of result
!   bc: least significant 2 bytes of multiplicand
!   de: most  significant 2 bytes of multiplicand
!   iy: 2 bytes of multiplier (first most significant,
!	later least significant)
!   a:  bit count
.mli4:
	!initialization
	pop hl		! return address
	pop de
	ld (.mplier+2),de! least significant bytes of
			! multiplier
	pop de
	ld (.mplier),de	! most sign. bytes
	pop de		! least significant bytes of
			! multiplicand
	pop bc		! most sign. bytes
	push hl		! return address
	push iy		! LB
	ld ix,0
	xor a
	ld h,a		! clear result
	ld l,a
	ld (.flag),a	! indicate that this is
			! first pass of main loop
	ld iy,(.mplier)
	! main loop, done twice, once for each part (2 bytes)
	! of multiplier
1:
	ld a,16
	! sub-loop, done 16 times
2:
	add iy,iy	! shift left multiplier
	jr nc,3f	! skip if most sign. bit is 0
	add ix,de	! 32-bit add
	adc hl,bc
3:
	dec a
	jr z,4f		! done with this part of multiplier
	add ix,ix	! 32-bit shift left
	adc hl,hl
	jr 2b
4:
	! see if we have just processed the first part
	! of the multiplier (flag = 0) or the second
	! part (flag = 1)
	ld a,(.flag)
	or a
	jr nz,5f
	inc a		! a := 1
	ld (.flag),a	! set flag
	ld iy,(.mplier+2)! least significant 2 bytes now in iy
	add ix,ix	! 32-bit shift left
	adc hl,hl
	jr 1b
5:
	! clean up
	pop iy		! restore LB
	ex (sp),hl	! put most sign. 2 bytes of result
			! on stack!  put return address in hl
	push ix		! least sign. 2 bytes of result
	jp (hl)		! return
.sect .data
.flag:  .data1 0
.mplier: .space 4
