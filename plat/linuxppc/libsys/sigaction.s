#define __NR_sigaction		67
#define __NR_sigprocmask	126
#define SIG_SETMASK		2

/* offsets into struct sigaction */
#define sa_handler	0	/* in union with sa_sigaction */
#define sa_mask		4
#define sa_flags	8
#define sa_restorer	12

/* offsets from stack pointer */
#define mynewact	16	/* struct sigaction */
#define myoldact	32
#define newmask		64	/* signal set */
#define oldmask		68
#define oldhandler	72
#define myret		76
#define savelr		80
#define signum		84	/* first argument */
#define newact		88
#define oldact		92

.sect .text; .sect .rodata; .sect .data; .sect .bss

/*
 * Linux calls signal handlers with arguments in registers, but the
 * ACK expects arguments on the stack.  This sigaction() uses a
 * "bridge" to move the arguments, but
 *
 *  - If the caller passes a bad pointer, this sigaction() causes
 *    SIGBUS or SIGSEGV instead of setting errno = EFAULT.
 *
 *  - This sigaction() only works with signals 1 to 31, not with
 *    real-time signals 32 to 64.
 *
 *  - This sigaction() is not safe for multiple threads.
 *
 * int sigaction(int signum, const struct sigaction *newact,
 *		 struct sigaction *oldact);
 */
.sect .text
.define _sigaction
_sigaction:
	mflr	r0
	li	r3, __NR_sigprocmask
	stwu	r3, -signum(sp)		/* keep 0(sp) = __NR_sigprocmask */
	stw	r0, savelr(sp)

	/* Copy newact to stack (before blocking SIGBUS, SIGSEGV). */
	lwz	r3, newact(sp)
	mr.	r3, r3
	beq	1f			/* skip if newact == NULL */
	lwz	r4, sa_handler(r3)
	lwz	r5, sa_mask(r3)
	lwz	r6, sa_flags(r3)
	lwz	r7, sa_restorer(r3)
	stw	r4, mynewact+sa_handler(sp)
	stw	r5, mynewact+sa_mask(sp)
	stw	r6, mynewact+sa_flags(sp)
	stw	r7, mynewact+sa_restorer(sp)

	/*
	 * Block all signals to prevent a race.  After we set sharray,
	 * we must call the kernel's sigaction before the next signal
	 * handler runs.  This prevents two problems:
	 *
	 *  - The bridge might call the new handler while the kernel
	 *    uses the mask and flags of the old handler.
	 *
	 *  - The signal handler might call sigaction() and destroy
	 *    sharray.  We must block all signals because any signal
	 *    handler might call sigaction() for our signal.
	 */
1:	li	r4, SIG_SETMASK
	li	r5, -1			/* mask signals 1 to 32 */
	stw	r5, newmask(sp)
	la	r5, newmask(sp)
	la	r6, oldmask(sp)
	stw	r4, 4(sp)		/* kept 0(sp) = __NR_sigprocmask */
	stw	r5, 8(sp)
	stw	r6, 12(sp)
	bl	__syscall

	/*
	 * If the signal number is in range 1 to 31, and the new
	 * handler is not SIG_DFL 0 or SIG_IGN 1, then we interpose
	 * our bridge.
	 */
	lwz	r4, signum(sp)		/* keep r4 = signum */
	addi	r5, r4, -1
	cmplwi	r5, 30
	bgt	2f			/* skip if out of range */

	slwi	r5, r5, 2		/* r5 = sharray index */
	lis	r6, ha16[sharray]
	la	r6, lo16[sharray](r6)	/* r6 = sharray */
	lwzx	r0, r6, r5
	stw	r0, oldhandler(sp)	/* remember old handler */
	lwz	r0, newact(sp)
	mr.	r0, r0
	beq	2f			/* skip if newact == NULL */

	lwz	r3, mynewact+sa_handler(sp)
	cmplwi	r3, 2			/* r3 = new handler */
	blt	2f			/* skip if SIG_DFL or SIG_IGN */

	stwx	r3, r6, r5		/* put new handler in sharray */
	lis	r3, ha16[sigbridge]
	la	r3, lo16[sigbridge](r3)
	stw	r3, mynewact+sa_handler(sp)

	/* Call the kernel's sigaction. */
	/* sigaction(signum, &mynewact or NULL, &myoldact or NULL) */
2:	li	r3, __NR_sigaction
	lwz	r0, newact(sp)
	mr.	r0, r0
	beq	3f
	la	r5, mynewact(sp)
	b	4f
3:	li	r5, 0
4:	lwz	r0, oldact(sp)
	mr.	r0, r0
	beq	5f
	la	r6, myoldact(sp)
	b	6f
5:	li	r6, 0
6:	stw	r3, 0(sp)
	stw	r4, 4(sp)		/* kept r4 = signum */
	stw	r5, 8(sp)
	stw	r6, 12(sp)
	bl	__syscall
	stw	r3, myret(sp)

	/* Unblock signals by restoring old signal mask. */
	li	r3, __NR_sigprocmask
	li	r4, SIG_SETMASK
	la	r5, oldmask(sp)
	li	r6, 0
	stw	r3, 0(sp)
	stw	r4, 4(sp)
	stw	r5, 8(sp)
	stw	r6, 12(sp)
	bl	__syscall

	/* Copy oldact from stack (after unblocking BUS, SEGV). */
	lwz	r3, oldact(sp)
	mr.	r3, r3
	beq	8f			/* skip if oldact == NULL */
	lwz	r4, myoldact+sa_handler(sp)
	lis	r5, ha16[sigbridge]
	la	r5, lo16[sigbridge](r5)
	cmpw	r4, r5
	bne	7f
	lwz	r4, oldhandler(sp)
7:	lwz	r5, myoldact+sa_mask(sp)
	lwz	r6, myoldact+sa_flags(sp)
	lwz	r7, myoldact+sa_restorer(sp)
	stw	r4, sa_handler(r3)
	stw	r5, sa_mask(r3)
	stw	r6, sa_flags(r3)
	stw	r7, sa_restorer(r3)

8:	lwz	r0, savelr(sp)
	lwz	r3, myret(sp)
	addi	sp, sp, signum
	mtlr	r0
	blr

/*
 * Linux calls sigbridge(signum) or sigbridge(signum, info, context)
 * with arguments in registers r3, r4, r5.
 */
sigbridge:
	mflr	r0
	stwu	r3, -16(sp)	/* signal number */
	stw	r4, 4(sp)	/* info */
	stw	r5, 8(sp)	/* context */
	stw	r0, 12(r1)

	lis	r6, hi16[sharray - 4]
	la	r6, lo16[sharray - 4](r6)
	slwi	r7, r3, 2
	lwzx	r6, r6, r7
	mtctr	r6
	bctrl			/* call our signal handler */

	lwz	r0, 12(sp)
	addi	r1, r1, 16
	mtlr	r0
	blr			/* sigreturn(2) */

.sect .bss
sharray:
	.space 4 * 31		/* handlers for signals 1 to 31 */
