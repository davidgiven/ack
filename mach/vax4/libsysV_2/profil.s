.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
prof = 44
.define	_profil

_profil:
	.data2	0x0000
	chmk	$prof
	ret
