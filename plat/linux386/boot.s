#
! $Source$
! $State$
! $Revision$

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
	
	mov eax, (esp)    ! eax = argc
	lea ebx, 4(esp)   ! ebx = argv
	lea ecx, (esp)(eax*4)
	add ecx, 12       ! environ
	
	push ecx         ! environ
	push ebx         ! argc
	push eax         ! argv
	push eax         ! dummy, representing the return argument
	xor ebp, ebp
	
	jmp __m_a_i_n
	
	! This provides an emergency exit routine used by EM.
	
.define EXIT
.extern EXIT
EXIT:
	push 1
	jmp __exit
	
! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask
.comm .trppc, 4
.comm .ignmask, 4
