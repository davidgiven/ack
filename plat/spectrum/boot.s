#

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
	! We have to clear the bss. (argify requires it.)
	
	ld hl, endbss
	push hl
	ld de, begbss
	sbc hl, de
	ld b, h
	ld c, l
	dec bc
	pop hl
	xor a
	ld (de), a
	inc de
	ldir

	! Set up the stack (now it's been cleared, since it's in the BSS).

	ld sp, stack + STACKSIZE

	! Push standard parameters.

	ld hl, envp
	push hl
	ld hl, argv
	push hl
	ld hl, 1
	push hl

	! Go.
	
	call __m_a_i_n
.define EXIT, __exit
EXIT:
__exit:
	jr __exit
	.align 2

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .ignmask, _errno, .trapproc
.comm .ignmask, 2
.comm _errno, 2
.comm .trapproc, 2

! The fake argv and env arrays.

.sect .data
argv: .data2 exename, 0
envp: .data2 0
exename: .asciz 'spectrum'
