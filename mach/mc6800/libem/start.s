.define START,EM_BSIZE

.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end

BASE    = 240
EM_BSIZE = 4

.sect .text
!.base 0x0100            ! where to start in the emu6800
! GENERAL PURPOSE ROUTINES
START:  jmp     _main

.sect .data
PROGNAME:               ! for initialising the programname pointer
.asciz "program"
.sect .bss
beginbss:
