#
.sect .text; .sect .rom; .sect .data; .sect .bss


.sect .text
.define .trap_ecase
.trap_ecase:
	li	r4, 20			! ECASE = 20 in h/em_abs.h
	! FALLTHROUGH to .trp

! Raises an EM trap.
! Expects r4 = trap number.

.define .trp
.trp:
	andi at, r4, 0xfff0
	bne at, zero, 1f				! traps > 15 can't be ignored
	nop

	lui at, ha16[.ignmask]
	lw r5, lo16[.ignmask] (at)		! load ignore mask
	srlv r5, r5, r4
	andi r5, r5, 1
	bne r5, zero, return			! return if ignoring trap
	nop
1:

	lui at, ha16[.trppc]
	lw r5, lo16[.trppc] (at)		! r5 = user trap routine
	sw zero, lo16[.trppc] (at)		! reset the trap routine for next time
	beq r5, zero, abend				! abort if no user trap routine
	nop

	addiu sp, sp, -8
	sw r4, 0(sp)					! push trap number
	sw ra, 4(sp)					! and link register
	jalr r5							! call trap routine
	nop

	lw ra, 4(sp)					! ...and return
	addiu sp, sp, 8
return:
	jr ra
	nop

	! No trap handler; write error message and exit.
abend:
	addiu sp, sp, -12
	li at, 2
	sw at, 0(sp)
	lui at, hi16[message]
	ori at, at, lo16[message]
	sw at, 4(sp)
	li at, 6
	sw at, 8(sp)
	jal _write						! write(2, message, 6)
	nop

	li at, 1
	sw at, 0(sp)
	j __exit						! _exit(1)

.sect .rom
message:
	.ascii "TRAP!\n"
