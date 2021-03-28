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

! Get the current process identifier.  For MS-DOS, use the Program Segment
! Prefix (PSP) segment as the PID.
!
! (Note that pid_t is a 32-bit type.  This is to allow for PSP addresses
! above 0x7FFF:0.)

.define _getpid
_getpid:
	movb ah, 0x51
	int 0x21
	xchg bx, ax
	xor dx, dx
	ret
