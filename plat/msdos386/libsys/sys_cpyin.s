#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! .sys_cpyin: copy ecx bytes from the transfer buffer to es:eax (= ds:eax).
! Preserve all registers, except the flags.

.extern .sys_cpyin
.sys_cpyin:
	push eax
	push ecx
	push esi
	push edi
	movzx esi, (transfer_buffer_ptr)
	xchg edi, eax
	mov ds, (pmode_ds)
	mov eax, ecx
	shr ecx, 2
	rep movs
	andb al, 3
	movb cl, al
	rep movsb
	mov eax, ss
	mov ds, eax
	pop edi
	pop esi
	pop ecx
	pop eax
	ret
