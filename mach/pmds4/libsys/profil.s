.define _profil
.extern _profil
.text
_profil:		trap #0
.short	0x2C
			rts
