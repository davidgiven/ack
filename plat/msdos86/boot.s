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
	! Make sure we are running under MS-DOS 2 or above.
	!
	! While at it, also remember the actual DOS version, so that we know
	! whether DOS gives us the program's name in the environment
	! segment.  (DOS 3+ does; DOS 2.x does not.)
	movb ah, 0x30
	int 0x21
	cmpb al, 2
	xchg bx, ax
	jc bad_sys

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
	mov es, (0x002C)
	xor di, di
	! ax = 0 from above
	cwd				! dx = count of env. vars.
	mov cx, -1
	scasb				! handle special case of empty env.
	jz is_empty_env
size_env:
	inc dx
	repnz scasb
	scasb
	jnz size_env
is_empty_env:
	cmpb bl, 2
	jz no_argv0
	scasw
	repnz scasb
no_argv0:

	! Copy out the environment variables and (possibly) program name
	! onto the stack.
	mov si, di
	dec si
	and si, -2
	std
copy_env:
	test si, si
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
	push bx				! MS-DOS version
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

bad_sys:
	mov dx, bad_sys_msg
dos_msg:
	movb ah, 9
	int 0x21
	ret

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
