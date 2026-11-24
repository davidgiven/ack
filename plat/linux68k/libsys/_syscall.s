#
! $Source: /cvsroot/tack/Ack/plat/linux68k/libsys/_syscall.s,v $
! $State: Exp $
! $Revision: 1.2 $

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
	move.l d2, -(sp)
	move.l d3, -(sp)
	! Stack: (0)saved d3, (4)saved d2, (8)ret, (12)op, (16)p1, (20)p2, (24)p3
	move.l (12, sp), d0
	move.l (16, sp), d1
	move.l (20, sp), d2
	move.l (24, sp), d3
	trap #0
	move.l (sp)+, d3
	move.l (sp)+, d2

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
