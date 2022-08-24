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

! Rename a file.

.define _rename
_rename:
	mov ebx, esp
	push esi
	push edi

	! Source filename.

	mov esi, 4(ebx)
	movzx edi, (transfer_buffer_ptr)
	mov es, (pmode_ds)
	cld
1:
	lodsb
	stosb
	testb al, al
	jnz 1b
	
	! Destination filename.

	mov eax, edi
	mov esi, 8(ebx)
1:
	lodsb
	stosb
	testb al, al
	jnz 1b
	
	! Make the DOS call.

	o16 mov dx, (transfer_buffer_ptr)
	o16 mov di, ax
	movb ah, 0x56
	mov ebx, 0x210000
	callf (interrupt_ptr)
	pop edi
	pop esi
	push ss
	pop es
	jmp .sys_zret
