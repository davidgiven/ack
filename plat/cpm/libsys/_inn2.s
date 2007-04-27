#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

! Bit test on 16 bits set
! Expects on stack:	bit number
!			set to be tested
! Yields in de-registers:	0 if bit is reset or bit number out of range
!				1 if bit is set

.sect .text
.define .inn2
.inn2:	pop h
	shld .retadr

	pop d		!bit number
	pop h		!set to be tested
	mov a,e
	cpi 16
	jnc 3f
	cpi 8
	jnc 1f
	mov e,a
	mov a,l		!l-reg contains the wanted bit
	jmp 2f

1:	sbi 8
	mov e,a
	mov a,h		!h-reg contains the wanted bit

2:	dcr e
	jm 4f
	rar
	jmp 2b

3:	xra a		!return 0 if bit number out of range
4:	ani 1
	mov e,a
	mvi d,0

	lhld .retadr
	pchl
