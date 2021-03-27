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

! Read/write bytes to/to a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawread
__sys_rawread:
	movb ah, 0x3f
	.data1 0x3d			! eat up the next instruction
.define __sys_rawwrite
__sys_rawwrite:
	movb ah, 0x40
	mov bx, sp
	mov dx, 4(bx)
	mov cx, 6(bx)
	mov bx, 2(bx)
	int 0x21
	jmp .sys_axret
