.define _umask
.extern _umask
.text
_umask:		trap #0
.short	0x3C
			rts
