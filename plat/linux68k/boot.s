#
! $Source: /cvsroot/tack/Ack/plat/linux68k/boot.s,v $
! $State: Exp $
! $Revision: 1.4 $

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
	! sp+8+(4*argc)   envp[0] ...
	! sp+4+(4*argc)   NULL
	! sp+4            argv[0] ... argv[argc-1]
	! sp              argc
	!
	! _main parameters are:
	!   argc (word), argv (pointer), envp (pointer)
	! sp+12           envp
	! sp+8            argv
	! sp+4            argc
	! sp              return

	move.l (0, sp), d0      ! d0 = argc
	lea (4, sp), a0         ! a0 = argv

	move.l d0, d1
	asl.l #2, d1            ! d1 = argc*4
	add.l a0, d1            ! d1 = &argv[argc]
	add.l #4, d1            ! d1 = envp

	move.l d1, -(sp)        ! envp
	move.l a0, -(sp)        ! argv
	move.l d0, -(sp)        ! argc
	pea (0)                 ! return

	jmp (__m_a_i_n)
	 	
	! This provides an emergency exit routine used by EM.
	
.define EXIT
.extern EXIT
EXIT:
	pea (1)
	jmp (__exit)
	
	! This is used by the trap routine for diagnostics.
	
.define WRITE
.extern WRITE
WRITE:
	jmp (_write)

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 4
.comm .ignmask, 4
.comm _errno, 4
