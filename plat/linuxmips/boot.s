#
! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

begtext:
	! This code is placed at the beginning of the ELF executable and is the
	! first thing that runs.
	!
	! On entry, the stack looks like this:
	!
	! sp+...          NULL
	! sp+8+(4*argc)   env (X quads)
	! sp+4+(4*argc)   NULL
	! sp+4            argv (argc quads)
	! sp              argc
	!
	! The ACK actually expects:
	!
	! sp+8            argc
	! sp+4            ptr to argv
	! sp              ptr to env

	lw r4, 0(sp)             ! r4 = argc
	addiu r5, sp, 4          ! r5 = argv
	sll r6, r4, 2            ! r6 = argc*4
	addu r6, r6, r5          ! r6 = last arg
	addiu r6, r6, 8          ! r6 = env

	addiu sp, sp, -3 * 4
	sw r4, 0(sp)
	sw r5, 0(sp)
	sw r6, 0(sp)

	b __m_a_i_n
	nop

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define _errno
.comm _errno, 4              ! Posix errno storage

.define .trppc, .ignmask
.comm .trppc, 4              ! ptr to user trap handler
.comm .ignmask, 4            ! user trap ignore mask 
