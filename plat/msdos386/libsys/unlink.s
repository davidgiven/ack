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

! Remove a file.

.define _unlink
_unlink:
	! Copy filename to transfer buffer.

	mov ebx, esp
	push esi
	push edi

	mov esi, 4(ebx)
    movzx edi, (transfer_buffer_ptr)
    mov es, (pmode_ds)
    cld
1:
	lodsb
	stosb
	testb al, al
	jnz 1b

	! Make the DOS call.

    movzx edi, (transfer_buffer_ptr)
	movb ah, 0x41
	or ebx, 0x210000
	callf (interrupt_ptr)

	pop edi
	pop esi
	jmp .sys_zret
