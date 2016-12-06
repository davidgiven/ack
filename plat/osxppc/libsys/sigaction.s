#define IFTRUE 12
#define ALWAYS 20
#define EQ 2

! OS X, unlike FreeBSD, requires us to provide our own signal
! trampoline.  We must change the new action from a struct sigaction
! to a bigger struct that includes the trampoline.

.sect .text
.define _sigaction
_sigaction:
	addi r0, r0, 46		! sigaction
	lwz r3, 0(sp)		! sig
	lwz r4, 4(sp)		! ptr to new action
	lwz r5, 8(sp)		! ptr to old action
	or. r6, r4, r4
	bc IFTRUE, EQ, 1f	! skip if new action is NULL

	! We may use the "red zone" from -224(sp) to 0(sp).
	addi r4, sp, -16	! r4 = bigger struct
	lwz r7, 0(r6)
	stw r7, 0(r4)		! sa_handler
	li32 r7, trampoline
	stw r7, 4(r4)		! sa_tramp
	lwz r7, 4(r6)
	stw r7, 8(r4)		! sa_mask
	lwz r7, 8(r6)
	stw r7, 12(r4)		! sa_flags
1:
	sc 0
	b .set_errno
	bclr 20, 0, 0

trampoline:
	! r3 = handler
	! r4 = info style
	! r5 = sig
	! r6 = info
	! r7 = context
	or r31, r4, r4		! ack preserves r30, r31
	or r30, r7, r7

	! Call handler(sig, info, context).
	mtspr ctr, r3
	stwu r7, -4(sp)		! ack expects arguments on stack
	stwu r6, -4(sp)
	stwu r5, -4(sp)
	bcctrl ALWAYS, 0, 0

	! Return from trampoline.
	addi r0, r0, 184	! sigreturn
	or r3, r30, r30		! context
	or r4, r31, r31		! info style
	sc 0
	ori r0, r0, 0		! nop

	! Only if sigreturn() fails:
	addi r0, r0, 1		! exit
	sc 0
