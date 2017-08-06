#define __NR_sigaction		67
#define SIG_BLOCK		0
#define SIG_SETMASK		2
#define MAXSIG			32

/* offsets into our stack frame */
#define mynew	16	/* new sigaction */
#define mynset	32	/* new signal set */
#define myoset	36	/* old signal set */
#define mysave	40
#define mysize	56

.sect .text; .sect .rodata; .sect .data; .sect .bss

/*
 * Linux calls signal handlers with arguments in registers, but the
 * ACK expects arguments on the stack.  This sigaction() uses a
 * "bridge" to move the arguments.
 */
.sect .text
.define _sigaction
_sigaction:
	mflr	r0
	subi	r1, r1, mysize
	stw	r31, mysave+8(r1)
	stw	r30, mysave+4(r1)
	stw	r29, mysave(r1)
	stw	r0, mysave+12(r1)
	li	r3, 0
	stw	r3, mynset(r1)	   	! mynset = 0
	lwz	r29, mysize(r1)		! r29 = signal number
	lwz	r30, mysize+4(r1)	! r30 = new action
	lwz	r31, mysize+8(r1)	! r31 = old action
	/*
	 * If the new action is non-NULL, the signal number is in
	 * range 1 to MAXSIG, and the new handler is not SIG_DFL 0
	 * or SIG_IGN 1, then we interpose our bridge.
	 */
	cmpwi	cr0, r30, 0
	subi	r7, r29, 1		! r7 = index in handlers
	cmplwi	cr7, r7, MAXSIG		! unsigned comparison
	beq	cr0, kernel
	bge	cr7, kernel
	lwz	r3, 0(r30)		! r3 = new handler
	clrrwi.	r3, r3, 1
	beq	cr0, kernel
	/*
	 * Block the signal while we build the bridge.  Prevents a
	 * race if a signal arrives after we change the bridge but
	 * before we change the action in the kernel.
	 */
	li	r4, 1
	slw	r4, r4, r7
	stw	r4, mynset(r1)		! mynmask = 1 << (signal - 1)
	li	r3, SIG_BLOCK
	la	r4, mynset(r1)
	la	r5, myoset(r1)
	stw	r3, 0(r1)
	stw	r4, 4(r1)
	stw	r5, 8(r1)
	bl	_sigprocmask
	/*
	 * Point our bridge to the new signal handler.  Then copy the
	 * new sigaction but point it to our bridge.
	 */
	lis	r6, hi16[handlers]
	ori	r6, r6, lo16[handlers]
	subi	r7, r29, 1
	slwi	r7, r7, 2
	lwz	r3, 0(r30)		! r3 = new handler
	stwx	r3, r6, r7		! put it in array of handlers
	lis	r3, hi16[bridge]
	ori	r3, r3, lo16[bridge]
	lwz	r4, 4(r30)
	lwz	r5, 8(r30)
	lwz	r6, 12(r30)
	stw	r3, mynew(r1)		! sa_handler or sa_sigaction
	stw	r4, mynew+4(r1)		! sa_mask
	stw	r5, mynew+8(r1)		! sa_flags
	stw	r6, mynew+12(r1)	! sa_restorer
	la	r30, mynew(r1)
kernel:
	li	r3, __NR_sigaction
	stw	r3, 0(r1)
	stw	r29, 4(r1)
	stw	r30, 8(r1)
	stw	r31, 12(r1)
	bl	__syscall
	/*
	 * If we blocked the signal, then restore the old signal mask.
	 */
	lwz	r3, mynset(r1)
	cmpwi	cr0, r3, 0
	beq	cr0, fixold
	li	r3, SIG_SETMASK
	la	r4, myoset(r1)
	li	r5, 0
	stw	r3, 0(r1)
	stw	r4, 4(r1)
	stw	r5, 8(r1)
	bl	_sigprocmask
	/*
	 * If the old sigaction is non-NULL and points to our bridge,
	 * then point it to the signal handler.
	 */
fixold:
	cmpwi	cr0, r31, 0
	beq	cr0, leave
	lis	r3, hi16[bridge]
	ori	r3, r3, lo16[bridge]
	lwz	r4, 0(r31)
	cmpw	cr0, r3, r4
	bne	cr0, leave
	lis	r6, hi16[handlers]
	ori	r6, r6, lo16[handlers]
	subi	r7, r29, 1
	slwi	r7, r7, 2
	lwzx	r3, r6, r7	! get it from array of handlers
	stw	r3, 0(r31)	! put it in old sigaction
leave:
	lwz	r0, mysave+12(r1)
	lwz	r29, mysave(r1)
	lwz	r30, mysave+4(r1)
	lwz	r31, mysave+8(r1)
	addi	r1, r1, mysize
	mtlr	r0
	blr			! return from sigaction

/*
 * Linux calls bridge(signum) or bridge(signum, info, context) with
 * arguments in registers r3, r4, r5.
 */
bridge:
	mflr	r0
	subi	r1, r1, 16
	stw	r0, 12(r1)
	stw	r3, 0(r1)	! signal number
	stw	r4, 4(r1)	! info
	stw	r5, 8(r1)	! context

	lis	r6, hi16[handlers]
	ori	r6, r6, lo16[handlers]
	subi	r7, r3, 1
	slwi	r7, r7, 2
	lwzx	r6, r6, r7
	mtctr	r6
	bctrl			! call our signal handler

	lwz	r0, 12(r1)
	addi	r1, r1, 16
	mtlr	r0
	blr			! return from bridge

.sect .bss
handlers:
	.space 4 * MAXSIG	! array of signal handlers
