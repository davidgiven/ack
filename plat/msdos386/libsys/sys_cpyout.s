#
! $Source$
! $State$
! $Revision$

#include "libsysasm.h"

! .sys_cpyout: copy ecx bytes starting from ds:eax to the transfer
! buffer.  Return eax := offset of transfer buffer in the real mode data
! segment.  Preserve all other registers, except the flags.

.extern .sys_cpyout
.sys_cpyout:
	push ecx
	push edx
	push esi
	push edi
	xchg esi, eax
	movzx edi, (transfer_buffer_ptr)
	mov eax, edi
	mov es, (pmode_ds)
	mov edx, ecx
	shr ecx, 2
	rep movs
	movb cl, dl
	andb cl, 3
	rep movsb
	mov ecx, ss
	mov es, ecx
	pop edi
	pop esi
	pop edx
	pop ecx
	ret
