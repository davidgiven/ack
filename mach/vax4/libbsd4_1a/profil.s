.globl	_profil
.set	profil,44

.align	1
_profil:
	.word	0x0000
	chmk	$profil
	ret
