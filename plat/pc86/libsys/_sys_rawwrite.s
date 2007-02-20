#
! $Source$
! $State$

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

	movb al, 4(bp)
	movb ah, 0x0E
	mov bx, 0x0007
	int 0x10
	jmp .cret
	