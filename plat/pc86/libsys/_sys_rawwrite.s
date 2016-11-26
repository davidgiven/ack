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

! Writes a single byte to the console.

.define __sys_rawwrite
.extern __sys_rawwrite

__sys_rawwrite:
	push bp
	mov bp, sp

	! Write to the BIOS console.

	movb al, 4(bp)
	movb ah, 0x0E
	mov bx, 0x0007
	int 0x10

	! Also write to the serial port (used by the test suite).

	movb ah, 0x01
	xor dx, dx
	int 0x14

	jmp .cret
	