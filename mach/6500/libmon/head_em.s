.define WRCH, RDCH, Earray, Erange, Eset
.define Eiovfl, Eidivz, Eiund, Econv
.define Estack, Eheap, Eillins, Eoddz
.define Ecase , Ebadmon, OSBYTE, MON
.define Ebadlin, Ebadgto, BASE, NBYTES
.define hol0, IGNMASK, ADDR, PROGNAME
.define LB, LBl, SP, HP, ERRPROC, UNSIGN
.define Ytmp, EXG, ARTH, RETURN, SIGN
.define RETSIZE, TRAPVAL, STACK, BRANCH
.define start, Push, Pop, STACKTh, STACKTl
.define F_DUM
.define EXIT
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! DEFINITIONS

	! The next three definitions are special for the
	! BBC microcomputer

WRCH	= 0x0FFEE	! This subroutine writes the character in
			! register A to the screen
RDCH	= 0x0FFE0	! This subroutine returns a character in
			! register A from the current input stream
OSBYTE	= 0x0FFF4	! This subroutine performs miscelaneous
			! operating system calls

F_DUM	= 0		! Dummy floating point constant

	! Here are the error numbers

Earray	= 0
Erange	= 1
Eset	= 2
Eiovfl	= 3
Eidivz	= 6
Eiund	= 8
Econv	= 10
Estack	= 16
Eheap	= 17
Eillins = 18
Eoddz	= 19
Ecase	= 20
Ebadmon = 25
Ebadlin = 26
Ebadgto = 27
MON = 0x78D0

BASE	= 240		! This is the offset from the localbase
			! for the second localbase

STACKTh = 0x78		! This is the top of the stack
STACKTl = 0x0D0

	! Some zeropage declarations

.sect .zero

RES: .space 76		! special for the operating system

hol0:	.space 16	! the hol0 block

IGNMASK: .space 2	! can hold the ingnore mask

ADDR: .space 4		! used for indirect addressing

LB: .space 2		! the localbase

LBl: .space 2		! the second localbase (localbase-BASE)

SP: .space 3		! the stackpointer (real_stack)

HP: .space 2		! the heap pointer

BRANCH: .space 2	! used for branch instructions

ERRPROC: .space 2	! can hold the address of the error handler

Ytmp: .space 1		! used for intermediate storage in Y

EXG: .space 2		! used by the exchange subroutine Exg

ARTH: .space 16		! used for arithmetic

NBYTES: .space 2	! containes the number of bytes for a block move


RETURN: .space 4	! the return area

RETSIZE: .space 1	! the size of the object returned

SIGN: .space 1		! the sign of the calculation

UNSIGN : .space 1	! is it signed or unsigned arithmetic

TRAPVAL: .space 1	! intermediate storage of the error number

STACK: .space 1		! contains the hardware stackpointer on
			! entering _m_a_i_n for a neat return

RESERVED: .space 112    ! used by the operating system

.sect .text
.base 0x0E02		! where to start in the BBC micro
! GENERAL PURPOSE ROUTINES

start:
	tsx
	stx STACK	! save stackpointer for exit and error

	! The following three operating system calls are only
	! for the BBC microcomputer

	lda #2
	ldx #0
	ldy #0
	jsr OSBYTE	! return control to the keyboard
	lda #15
	ldx #0
	ldy #0
	jsr OSBYTE	! clear all internal buffers
	lda #3
	ldx #5
	ldy #0
	jsr OSBYTE	! output to screen and RS423

	lda #STACKTl
	sta LB		! set localbase (lowbyte)
	sta SP+2
	lda #0
	sta SP		! set stackpointer (lowbyte)
	sta ERRPROC	! set start address for error handler (lowbyte)
	sta ERRPROC+1	! set start address for error handler (highbyte)
	sta hol0	! set the line number (lowbyte)
	sta hol0+1	! set the line number (highbyte)
	lda #STACKTh
	sta SP+1	! set the stacpointer (highbyte)
	sta LB+1	! set the localbase (highbyte)
	lda #[endbss].l
	sta HP		! set the heap pointer (lowbyte)
	lda #[endbss].h
	sta HP+1	! set the heap pointer (highbyte)
	lda #[PROGNAME].l
	sta hol0+4	! set fake programname pointer (lowbyte)
	lda #[PROGNAME].h
	sta hol0+5	! set fake programname pointer (highbyte)
	lda #[beginbss].l
	sta ADDR	! start address of bss block (lowbyte)
	lda #[beginbss].h
	sta ADDR+1	! start address of bss block (highbyte)
	ldy #0
	lda #0
    4:	ldx #[endbss].h	! clear bss block
	cpx ADDR+1
	bcc 1f		! end of bss block reached
	bne 2f
	ldx #[endbss].l
	cpx ADDR
	bcc 1f		! end of bss block reached
    2:	sta (ADDR),y
	inc ADDR
	bne 3f
	inc ADDR+1
    3:	jmp 4b
    1:	lda #0
	tax
	jsr Push	! push fake envelope pointer
	lda #[PROGNAME].h
	ldx #[PROGNAME].l
	jsr Push	! push argv[0]
	lda #0
	ldx #1
	jsr Push	! push argc 
    	jsr __m_a_i_n	! start the real program

	lda #0x0FF
	jsr WRCH	! send end of program to R423
	lda #3
	ldx #0
	jsr OSBYTE	! send output to screen only
	lda #2
	ldx #1
	jsr OSBYTE	! input only from R423
	rts

EXIT:
	ldx STACK	! load stackpointer
	dex
	dex		! adjust
	txs		! reset hardware stackpointer
	rts

! The subroutine Push pushes the registerpair AX onto the stack.

Push:
	sty Ytmp	! save Y
	ldy SP+2
	bne 1f		! lowbyte of stackpointer <> 0
	dec SP+1	! decrement highbyte of stackpointer
    1:  dey
	dey		! decrement lowbyte of stackpointer
	sty SP+2	! save lowbyte of stackpointer
	pha		! save A
	txa
    	sta (SP),y	! push X onto the stack
	iny
	pla		! get A
	sta (SP),y	! push A onto the stack
	ldy Ytmp	! restore Y
	rts


! The subroutine Pop pops the registerpair AX from the stack.

Pop:
	sty Ytmp	! save Y
	ldy SP+2
	lda (SP),y	! pop X from the stack
	tax
	iny
	lda (SP),y	! pop A from the stack
	iny
	bne 1f		! lowbyte of stackpointer <> 0
	inc SP+1	! increment highbyte of stackpointer
    1:  sty SP+2	! store lowbyte of stackpointer
	pha		! save A
	pla		! get A
	ldy Ytmp	! restore Y
	rts


.sect .data
PROGNAME:		! for initialising the programname pointer
.asciz "program"
.sect .bss
beginbss:
