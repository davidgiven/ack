.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.gto
        ! $Id$

.extern	.strlb
	! Nonlocal goto.
	! On the stack is a pointer to a block containing :
	! 1. the new local base		(ptr+8)
	! 2. the new stack pointer	(ptr+4)
	! 3. the new program counter	(ptr)
.gto:
	movl	(sp)+,r2
	movl	4(r2),sp	! Store new stack pointer
	pushl	(r2)		! Push target address
	pushl	8(r2)		! Push new local base
	jsb	.strlb		! Store in fp, compute ap
	rsb
