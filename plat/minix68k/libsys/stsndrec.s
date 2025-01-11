.define	_send
.define	_receive
.define	_sendrec
.sect	.text
.sect	.rom
.sect	.data
.sect	.bss
! =====================================================================
!                           send and receive                          =
! =====================================================================
! send(), receive(), sendrec() destroy d0, d1, and a0.

.sect	.text

_send:	jmp __send

_receive:
	jmp __receive

_sendrec:
	jmp __sendrec
