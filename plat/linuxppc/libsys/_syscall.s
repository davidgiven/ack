#
! $Source: /cvsroot/tack/Ack/plat/linux386/libsys/_syscall.s,v $
! $State: Exp $
! $Revision: 1.1 $

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define EINVAL 22

! Perform a Linux system call.

.define __syscall
__syscall:
	lwz r0, 0(sp)
	lwz r3, 4(sp)
	lwz r4, 8(sp)
	lwz r5, 12(sp)
	sc 0
	bnslr

	! On error, r3 contains the errno.	
	! It just so happens that errnos 1-34 are the same in Linux as in ACK.
	cmpwi r3, 1
	blt 2f
	cmpwi r3, 34
	bgt 2f

3:
	lis r4, ha16[_errno]
	stw r3, lo16[_errno](r4)
	li r3, -1
	blr

2:
	li r3, EINVAL
	b 3b
