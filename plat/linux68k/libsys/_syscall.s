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
	
! Perform a Linux system call.

.define __syscall
__syscall:
	move.l (4, sp), d0
	move.l (8, sp), d1
	move.l (12, sp), d2
	move.l (16, sp), d3
	trap #0
	
	tst.l d0
	bmi 1f
	rts
	
1:
	neg.l d0
	! It just so happens that errnos 1-34 are the same in Linux as in ACK.
	cmp.l #1, d0
	blt 2f
	cmp.l #34, d0
	bgt 2f
	move.l d0, (_errno)
3:
	move.l #-1, d0
	rts
	
2:
	move.l #EINVAL, (_errno)
	bra 3b
