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

.sect .text
begtext:
	! Check if bss would overlap BDOS.  We must not overwrite
	! BDOS and crash CP/M.  We cheat by comparing only high bytes
	! of each address.

	lxi b, __end
	lda 0x0007
	mov c, a		! c = high byte of BDOS address
	mov a, b		! a = high byte of _end
	cmp c
	rnc       		! emergency exit if a >= c

	! We have to clear the bss. (argify requires it.)
	
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

	lxi h, 0
	dad sp
	shld saved_sp			! save old stack pointer
	lxi sp, stack + STACKSIZE

	! Initialise the rsts (if desired).

	#ifdef USE_I80_RSTS
		call .rst_init
	#endif

    ! Now the 'heap'.

	lxi h, __end
	shld _cpm_ram

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
	lxi h, argv              ! hl = argv pointer

loop_of_argify:
	ldax b			! a = next character
	ora a			! check for end of string
	jz end_of_argify
	cpi ' '			! scan for non-space
	jz 2f

	mov m, c		! put next argument in argv
	inx h
	mov m, b
	inx h

	lda argc		! increment argc
	inr a
	sta argc
	cpi MAX_ARGV		! exit loop if argv is full
	jz end_of_argify

1:	inx b			! scan for space
	ldax b
	ora a
	jz end_of_argify
	cpi ' '
	jnz 1b

	xra a			! replace the space with a '\0'
	stax b

2:	inx b
	jmp loop_of_argify
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
	push h                   ! return address is 0
	jmp __m_a_i_n

.define _cpm_fastexit
_cpm_fastexit:
saved_sp = _cpm_fastexit + 1
	lxi sp, 0                ! patched on startup
	ret

! Emergency exit routine.

.define EXIT, __exit, _cpm_exit
EXIT = 0
__exit = 0
_cpm_exit = 0
	
! Special CP/M stuff.

.define _cpm_fcb
_cpm_fcb = 0x005c

.define _cpm_ram
.comm _cpm_ram, 2
.define _cpm_ramtop
_cpm_ramtop = 0x0001

.define _cpm_default_dma
_cpm_default_dma = 0x0080

.define _cpm_cmdlinelen, _cpm_cmdline
_cpm_cmdlinelen = 0x0080
_cpm_cmdline = 0x0081

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
