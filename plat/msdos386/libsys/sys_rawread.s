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

! Read bytes from a file descriptor.  These routines do not do any
! translation between CRLF and LF line endings.
!
! Note that, for MS-DOS, a raw "write" request of zero bytes will truncate
! (or extend) the file to the current file position.

.define __sys_rawread
__sys_rawread:
	enter 0, 0
	push esi
	push edi
file_handle = 2*4
write_buffer = 3*4
amount_to_read = 4*4

	mov eax, amount_to_read(ebp)
	mov ecx, 32*1024
	cmp eax, ecx
	jge 2f
	mov ecx, eax
2:

	! Read from DOS into the transfer buffer.

	movb ah, 0x3f
	o16 mov dx, (transfer_buffer_ptr)
	o16 mov bx, file_handle(ebp)
	or ebx, 0x210000
	callf (interrupt_ptr)
	jnc success

	! Process errors.

	push eax
	call __sys_seterrno
	pop ecx
	jmp exit
success:

	! Copy eax bytes out of the transfer buffer.

	push eax
	mov ecx, eax
	movzx esi, (transfer_buffer_ptr)
	mov edi, write_buffer(ebp)
	mov es, (pmode_ds)
	cld
1:
	eseg lodsb
	movb (edi), al
	inc edi
	loop 1b
	pop eax

exit:
	pop edi
	pop esi
	leave
	ret

