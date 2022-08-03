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
.use16

#define STACK_BUFFER 128 /* number of bytes to leave for stack */

begtext:
	! Make sure we are running under MS-DOS 2 or above.
	!
	! While at it, also remember the actual DOS version, so that we know
	! whether DOS gives us the program's name in the environment
	! segment.  (DOS 3+ does; DOS 2.x does not.)
	movb ah, 0x30
	int 0x21
	cbw
	cmpb al, 2
	xchg bp, ax
	jnc ok_sys

	mov dx, bad_sys_msg
dos_msg:
	movb ah, 9
	int 0x21
	ret

ok_sys:
	! Resize the program's memory control block (MCB) to cover only the
	! program's near code and data space.  Use the starting sp value as
	! a guide to how much memory we can grab.  Abort on any failure.
	!
	! As a side effect, this also frees up any memory allocated to our
	! program beyond 64 KiB.  (The freed memory can possibly be used by
	! e.g. child processes, in the future.)
	!
	! Also check that we have some space between the BSS end and the
	! starting sp.
	cmp sp, endbss+STACK_BUFFER
	jb no_room

	movb ah, 0x4a
	mov bx, sp
	movb cl, 4
	shr bx, cl
	inc bx
	int 0x21
	jc no_room

	! Clear BSS.
	mov di, begbss
	mov cx, endbss+1
	sub cx, di
	shr cx, 1
	xor ax, ax
	cld
	rep stosw

	! Get the size of the environment variables plus (if present) the
	! program name.  Also count the number of environment variables.
	xor di, di
	mov es, 0x002C(di)
	! ax = 0 from above
	cwd				! dx = count of env. vars.
	! cx = 0 from above
	dec cx				! cx = max. str. bytes to scan
	scasb				! handle special case of empty env.
	jz is_empty_env
size_env:
	inc dx
	repnz scasb
	scasb
	jnz size_env
is_empty_env:
	cmp bp, 2
	jz no_argv0
	scasw
	repnz scasb
no_argv0:

	! Copy out the environment variables and (possibly) program name
	! onto the stack.
	mov si, di
	dec si
	std
copy_env:
	and si, -2
	eseg lodsw
	push ax
	jnz copy_env
	mov cx, sp

	! Reset DF and es properly.
	cld
	push ss
	pop es

	! Reserve space for argc and the argv and envp pointers on the
	! stack.  These will be passed to __m_a_i_n later.
	sub sp, 6
	mov ax, sp

	! Build up argc, argv[], and envp[].
	push ax				! output buffer for argc, argv, envp
	push bp				! MS-DOS version
	push cx				! env. string data
	push dx				! count of env. vars.
	mov ax, 0x0080
	push ax				! raw command line
	call __sys_initmain
	add sp, 10

	! Bail out if something went wrong.
	test ax, ax
	jnz no_room

	! argc, argv, and envp are now at the stack top.  Now go.
	call __m_a_i_n
	add sp, 6
	push ax
	call _exit

no_room:
	mov dx, no_room_msg
	call dos_msg
	movb al, -1
	jmp al_exit

	! Exit.
.define __exit
.extern __exit
.define EXIT
.extern EXIT
__exit:
EXIT:
	pop bx
	pop ax
al_exit:
	movb ah, 0x4c
	int 0x21

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

.sect .rom

! Some text messages.
bad_sys_msg: .ascii 'Bad DOS$'
no_room_msg: .ascii 'No room$'

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 4
.comm .ignmask, 4
.comm _errno, 4
