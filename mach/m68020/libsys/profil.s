.define _profil
.extern _profil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_profil:	move.l #0x2C,d0
		trap #0
		rts
