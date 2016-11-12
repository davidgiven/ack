#
! $Source: /cvsroot/tack/Ack/plat/linux386/boot.s,v $
! $State: Exp $
! $Revision: 1.3 $

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

	li32 r3, __openfirmware_ptr
	stw r5, 0(r3)

	li32 r3, envp
	stwu r3, -4(sp)

	li32 r3, argv
	stwu r3, -4(sp)

	li32 r3, 1 ! argc
	stwu r3, -4(sp)
	
	bl _openfirmware_init
	bl __m_a_i_n
	! falls through

.define __exit
.extern __exit
.define EXIT
.extern EXIT
__exit:
EXIT:
	b EXIT

.define _openfirmware_call
.extern _openfirmware_call
_openfirmware_call:
	lwz r3, 0(sp)
	li32 r4, __openfirmware_ptr
	lwz r4, 0(r4)
	mtspr ctr, r4
	bcctr 20, 0, 0

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define _errno
.comm _errno, 4              ! Posix errno storage

! The argv and env arrays.

.sect .rom
argv: .data4 exename, 0
envp: .data4 0
exename: .asciz 'qemuppc.img'

.define .trppc, .ignmask
.comm .trppc, 4              ! ptr to user trap handler
.comm .ignmask, 4            ! user trap ignore mask 
.comm __openfirmware_ptr, 4      ! OpenFirmware entry point
