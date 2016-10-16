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

EINVAL = 22

#define IFFALSE 4
#define IFTRUE 12
#define ALWAYS 20

#define LT 0
#define GT 1
#define EQ 2
#define OV 3
	
! Perform a Linux system call.

.define __syscall
__syscall:
	lwz r0, 0(sp)
	lwz r3, 4(sp)
	lwz r4, 8(sp)
	lwz r5, 12(sp)
	sc 0
	bclr IFFALSE, OV, 0
	
	! On error, r3 contains the errno.	
	! It just so happens that errnos 1-34 are the same in Linux as in ACK.
	cmpi cr0, 0, r3, 1
	bc IFTRUE, LT, 2f
	cmpi cr0, 0, r3, 34
	bc IFTRUE, GT, 2f
	
3:
	la r4, _errno
	stw r3, 0(r4)
	addi r3, r0, -1
	bclr ALWAYS, 0, 0
	
2:
	addi r3, r0, EINVAL
	b 3b
