#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .bss
STACKSIZE = 2*1024
.comm stack, STACKSIZE
.comm oldstack, 2

.sect .text
begtext:
	lxi sp, oldstack + STACKSIZE
	
	lxi h, 0
	push h
	push h
	push h
	call __m_a_i_n
	jmp EXIT
	
! Emergency exit routine.

.define EXIT, __exit
EXIT:
__exit:
	rst 0
	
! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 2
.comm .ignmask, 2
.comm _errno, 2

! These are used specifically by the CP/M port.

.define .trapproc, .retadr, .retadr1
.define .bcreg, .areg
.define .tmp1, .fra, block1, block2, block3

.comm .trapproc, 2
.comm .retadr, 2        ! used to save return address
.comm .retadr1, 2       ! reserve
.comm .bcreg, 2
.comm .areg, 1
.comm .tmp1, 2
.comm .fra, 8           ! 8 bytes function return area
block1: .space 4        ! used by 32 bits divide and
block2: .space 4        ! multiply routines
block3: .space 4        ! must be contiguous (.comm doesn't guarantee this)
