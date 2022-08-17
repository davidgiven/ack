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

.extern pmode_ds
.extern pmode_cs
.extern rmode
.extern transfer_buffer_ptr
.extern interrupt_ptr

! Write bytes to/to a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawwrite
__sys_rawwrite:
	enter 4, 0
amount_transferred = -1*4
file_handle = 2*4
read_buffer = 3*4
amount_to_write = 4*4

	mov es, (pmode_ds)
	mov amount_transferred(ebp), 0

mainloop:
	mov ecx, 32*1024
	cmp amount_to_write(ebp), ecx
	jge 2f
	mov ecx, amount_to_write(ebp)
	test ecx, ecx
	jz exit
2:
	! Copy ecx bytes into the transfer buffer.

	push ecx
	mov esi, read_buffer(ebp)
	movzx edi, (transfer_buffer_ptr)
	rep movsb
	pop ecx

	movb ah, 0x40
	o16 mov dx, (transfer_buffer_ptr)
	o16 mov bx, file_handle(ebp)
	or ebx, 0x210000
	callf (interrupt_ptr)
	movzx eax, ax

	add read_buffer(ebp), eax
	add amount_transferred(ebp), eax
	sub amount_to_write(ebp), eax
	jmp mainloop

exit:
	mov eax, amount_to_write(ebp)
	leave
	ret

