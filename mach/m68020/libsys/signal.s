.define _signal
.extern _signal
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_signal:
	move.l	#0x30,d0
	jmp	(callc)
! NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
!
! Just as in the m68000 tail_mon library the acual signal system call
! is not called directly by the user, but an `interface'. This `interface'
! does the actual system call. On the m68020 machine the table was tested on
! this `interface' did things that I did not understand, e.g. it called
! a sytem call with number 38, called `_sysm68k'. I couldn't get the signal
! system call get to work properly on that machine, but as it is clear
! that this is in no way to blame to my back end table.
! The system call as presented here does not reset the catch routine: a
! signal that is caught by a user function will be caught every time, not
! just once. I couldn't change this.
