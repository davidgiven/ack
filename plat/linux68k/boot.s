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
	! sp+..            NULL
	! sp+8+(4*argc)   env (X quads)
	! sp+4+(4*argc)   NULL
	! sp+4            argv (argc quads)
	! sp              argc
	!
	! The ACK actually expects:
	!
	! sp+8            argc
	! sp+4            argv
	! sp              env

	move.l (0, sp), d0 ! d0 = argc
	move.l (4, sp), d1 ! d1 = argv
	move.l d0, d2
	asl.l #2, d2
	move.l (sp, d2), d2
	add.l #12, d2      ! d2 = environ
	
	move.l d2, -(sp)   ! environ
	move.l d0, -(sp)   ! argc
	move.l d1, -(sp)   ! argv
	pea (0)            ! dummy, representing the return address
	
	jmp (__m_a_i_n)
	 	
#if 0
	mov eax, (esp)    ! eax = argc
	lea ebx, 4(esp)   ! ebx = argv
	lea ecx, (esp)(eax*4)
	add ecx, 12       ! environ
	
	push ecx         ! environ
	push ebx         ! argc
	push eax         ! argv
	push eax         ! dummy, representing the return argument
	xor ebp, ebp
#endif
	
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
