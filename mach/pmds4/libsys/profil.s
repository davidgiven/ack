.define _profil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _profil
.sect .text
_profil:		trap #0
.data2	0x2C
			rts
