.define Lcs
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine creates space for locals on procedure entry
! by lowering the stackpointer.

! For faster execution, AccAB is passed to Lcs with inverted sign,
!  which reduces STA usage. see mach/mc6800/cg/mach.c

Lcs:
	tsx
	ldx 0,x
	ins
	ins
	sts TMP
	addb TMP+1
	adca TMP
	stab TMP+1
	staa TMP
	lds TMP
	jmp 0,x


