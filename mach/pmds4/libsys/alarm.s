.define _alarm
.extern _alarm
.text
_alarm:		trap #0
.short	0x1B
			rts
