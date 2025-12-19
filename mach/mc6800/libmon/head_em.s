.define BASE, EM_BSIZE
.define hol0, ADDR
.define LB, LBl
.define	ARTH, RETURN, RETSIZE
.define TMP,TMP2
.define NBYTES
.define start

.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end

BASE    = 240
EM_BSIZE = 4

.sect .zero
hol0:   .space 16       ! the hol0 block
ADDR: .space 4          ! used for indirect addressing
LB: .space 2            ! the localbase
LBl: .space 2           ! the second localbase (localbase-BASE)
ARTH: .space 16         ! used for arithmetic
RETURN: .space 4        ! the return area
RETSIZE: .space 1
TMP: .space 2
TMP2: .space 2
NBYTES: .space 1

.sect .text
!! .base 0x0100            ! where to start in the emu6800
! GENERAL PURPOSE ROUTINES

START:
	jmp	_main

.sect .data
PROGNAME:               ! for initialising the programname pointer
.asciz "program"

.sect .bss
beginbss:
