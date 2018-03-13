! plat/osx386/boot.s

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

.sect .rom
begrom:

.sect .data
begdata:

! Some magic data. All EM systems need these.

.sect .bss
begbss:

.define _errno
.comm _errno, 4              ! Posix errno storage

.define .trppc, .ignmask
.comm .trppc, 4              ! ptr to user trap handler
.comm .ignmask, 4            ! user trap ignore mask
