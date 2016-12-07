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
	! This code is the first thing that runs.  The booloader
	! passes the Open Firmware pointer in r5.
	!
	! We keep the bootloader's stack.  The ACK expects:
	!
	! sp+8            environment pointer
	! sp+4            argv as a pointer
	! sp              argc

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
.define EXIT
__exit:
EXIT:
	! Halt the CPU.  This code halts the default G3 emulation of
	! qemu-system-ppc.  It's wrong for some other CPU models.
#define hid0 0x3f0
#define mfmsr(r) [[31<<26]|[[r]<<21]|0x0a6]
#define mtmsr(r) [[31<<26]|[[r]<<21]|0x124]
	mfspr r3, hid0
	oris r3, r3, 0x00e0		! set DOZE, NAP, SLEEP
	mtspr hid0, r3			!   in hid0
	.data4 mfmsr(3)
	oris r3, r3, 0x0004		! set POW
	.data4 mtmsr(3)			!   in msr0
	b EXIT		! If we failed to halt, then spin.

.define _openfirmware_call
_openfirmware_call:
	lwz r3, 0(sp)
	li32 r4, __openfirmware_ptr
	lwz r4, 0(r4)
	mtspr ctr, r4
	bcctr 20, 0, 0

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .rom;        begrom:
.sect .data;       begdata:
.sect .bss;        begbss:

! The argv and env arrays.

.sect .rom
argv: .data4 exename, 0
envp: .data4 0
exename: .asciz 'qemuppc.img'

! Some magic data. All EM systems need these.

.sect .bss
.define _errno
.comm _errno, 4              ! Posix errno storage

.define .trppc, .ignmask
.comm .trppc, 4              ! ptr to user trap handler
.comm .ignmask, 4            ! user trap ignore mask 
.comm __openfirmware_ptr, 4      ! OpenFirmware entry point
