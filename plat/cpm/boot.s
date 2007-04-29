#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

MAX_ARGV = 8

.sect .bss
STACKSIZE = 2*1024
.comm stack, STACKSIZE
.comm oldstack, 2

.sect .text
begtext:
	! The absolute first thing we have to do is to clear the bss. (argify
	! requires it.)
	
	lxi h, begbss
	lxi b, endbss
	mvi e, 0
1:
	mov m, e
	inx h
	mov a, b
	cmp h
	jnz 1b
	mov a, c
	cmp l
	jnz 1b

	! Set up the stack (now it's been cleared, since it's in the BSS).
		
	lxi sp, oldstack + STACKSIZE

	! C-ify the command line at 0x0080.
	
	lxi h, 0x0080
	mov a, m                 ! a = length of command line
	cpi 0x7F                 ! 127-byte command lines...
	jnz 1f
	dcr a                    ! ...lose the last character 
1:
	adi 0x81                 ! write a 0 at 0x0081+length
	mov l, a
	mov m, h
	
	! Now argify it.
	
	lxi b, 0x0081            ! bc = command line pointer
	lxi d, argv              ! de = argv pointer
		
	ldax b                   ! peek for any leading whitespace
	ora a
	cpi ' '
	jz 3f
	
1:  xchg                     ! write out the next argument
    mov m, c
    inx h
    mov m, b
    inx h
    xchg

	lda argc                 ! exit if this was the last argument
	inr a
	sta argc
	cpi MAX_ARGV
    jz end_of_argify
    
2:  inx b                    ! scan for whitespace
    ldax b
    ora a
    jz end_of_argify
    cpi ' '
    jnz 2b
    
    xra a                    ! replace the space with a \0
    stax b
    
3:  inx b                    ! scan for non-whitespace
    ldax b
    ora a
    jz end_of_argify
    cpi ' '
    jz 3b
    jmp 1b
end_of_argify:

	! Add the fake parameter for the program name.
	
	lxi h, progname
	shld argv0
	lxi h, argc
	inr m
	
	! Push the main() arguments and go.
	
	lxi h, envp
	push h
	lxi h, argv0
	push h
	lhld argc                ! slightly evil
	mvi h, 0
	push h
	call __m_a_i_n
	jmp EXIT
	
! Emergency exit routine.

.define EXIT, __exit
EXIT:
__exit:
	rst 0
	
! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 2
.comm .ignmask, 2
.comm _errno, 2

! Used to store the argv array.

argc: .space 1          ! number of args
argv0: .space 2         ! always points at progname
argv: .space 2*MAX_ARGV ! argv array (must be after argv0)
envp: .space 2          ! envp array (always empty, must be after argv)

! These are used specifically by the i80 code generator.

.define .trapproc, .retadr, .retadr1
.define .bcreg, .areg
.define .tmp1, .fra, block1, block2, block3

.comm .trapproc, 2
.comm .retadr, 2        ! used to save return address
.comm .retadr1, 2       ! reserve
.comm .bcreg, 2
.comm .areg, 1
.comm .tmp1, 2
.comm .fra, 8           ! 8 bytes function return area
block1: .space 4        ! used by 32 bits divide and
block2: .space 4        ! multiply routines
block3: .space 4        ! must be contiguous (.comm doesn't guarantee this)

.sect .rom
progname: .asciz 'ACKCPM'
