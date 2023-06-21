! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .bss
STACKSIZE = 512
.comm stack, STACKSIZE

.sect .text
begtext:
	! fall through

! Exit the program.

.define __exit
__exit:
	rst 0

! Panic the program.

.define _panic
_panic:
	ld hl, 1f
	ld bc, 0
	xor a
	rst 0x8
	rst 0

1:
	.ascii "ABEND"
	.data1 13, 10, 0

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .ignmask, _errno
.comm .ignmask, 2
.comm _errno, 2


