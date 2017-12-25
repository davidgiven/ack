.sect .text

.define .trap_ecase
.trap_ecase:
	li	r3, 20			! ECASE = 20 in h/em_abs.h
	! FALLTHROUGH to .trp

! Raises an EM trap.
! Expects r3 = trap number.

.define .trp
.trp:
	cmplwi	r3, 15			! traps > 15 can't be ignored
	bgt	1f

	lis	r4, ha16[.ignmask]
	lwz	r4, lo16[.ignmask](r4)	! load ignore mask
	srw	r4, r4, r3
	andi.	r4, r4, 1
	bnelr				! return if ignoring trap

1:	lis	r4, ha16[.trppc]
	lwz	r5, lo16[.trppc](r4)	! r5 = user trap routine
	mr.	r5, r5
	beq	2f			! if no user trap routine, bail out

	mtspr	ctr, r5
	mfspr	r6, lr
	li	r0, 0
	stwu	r3, -8(sp)		! push trap number
	stw	r0, lo16[.trppc](r4)	! reset trap routine
	stw	r6, 4(sp)		! save old lr
	bctrl				! call trap routine

	lwz	r0, 4(sp)
	mtspr	lr, r0
	addi	sp, sp, 8		! retract over stack usage
	blr

2:	! No trap handler.  Write error message, exit.
	li	r3, 2
	stwu	r3, -12(sp)
	lis	r4, ha16[message]
	addi	r4, r4, lo16[message]
	li	r5, 6
	stw	r4, 4(sp)
	stw	r5, 8(sp)
	bl	_write			! write(2, message, 6)

	li	r3, 1
	stw	r3, 0(sp)
	bl	__exit			! _exit(1)

.sect .rom
message:
	.ascii "TRAP!\n"
