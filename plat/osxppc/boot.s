! boot.s for osx386

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

begtext:
	! This code is placed at the entry point of the Mach-o
	! executable and is the first thing that runs.
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

	lwz r3, 0(sp)            ! r3 = argc
	addi r4, sp, 4           ! r4 = argv
	rlwinm r5, r3, 32-2, 2, 31 ! shift left 2 bits
	add r5, r5, r4 
	addi r5, r5, 8           ! r5 = env
	
	stwu r5, -4(sp)
	stwu r4, -4(sp)
	stwu r3, -4(sp)
	
	b __m_a_i_n

.sect rom
begrom:

.sect .data
begdata:

! Some magic data. All EM systems need these.

.sect .bss
begbss:
.define hol0
.comm hol0, 8                ! line number and filename (for debugging)

.define _errno
.comm _errno, 4              ! Posix errno storage

.define .trppc, .ignmask
.comm .trppc, 4              ! ptr to user trap handler
.comm .ignmask, 4            ! user trap ignore mask 
