.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_profil
profil = 44

.align	1
_profil:
	.data2	0x0000
	chmk	$profil
	ret
