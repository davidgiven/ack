! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .bss
STACKSIZE = 1024
.comm stack, STACKSIZE

.sect .text
begtext:
	ld sp, _stack + STACKSIZE

	! Clear the BSS.

	ld hl, endbss
	ld de, begbss
	push de
	and a
	sbc hl, de			! length of BSS
	push hl
	pop bc

	pop hl
	push hl
	pop de
	inc de
	
	ld (hl), 0
	ldir

	ld hl, 0
	push hl
	push hl
	push hl

	call __m_a_i_n
	rst 0

! Panic and exit the program.

.define _panic
_panic:
	ld hl, 1f
	ld bc, 0
	xor a
	rst 0x18
.define __exit
__exit:
	rst 0

1:
	.ascii "ABEND"
	.data1 13, 10, 0

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:

.sect .bss
.comm _stack, STACKSIZE		! the stack is outside the area cleared in the BSS.
begbss:

! Some magic data. All EM systems need these.

.define .ignmask, _errno
.comm .ignmask, 3
.comm _errno, 3

! The stack.

