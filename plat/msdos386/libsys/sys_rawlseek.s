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

! Move the current file position for a file descriptor.

.define __sys_rawlseek
__sys_rawlseek:
	int 3
	movb ah, 0x42
	mov bx, sp
	mov dx, 4(bx)
	mov cx, 6(bx)
	movb al, 8(bx)
	mov bx, 2(bx)
	int 0x21
	jmp .sys_dxaxret
