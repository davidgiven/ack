.define _pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _pause
.sect .text
_pause:
enter[], 0
movd 29,tos
jsr @.mon
exit []
ret 0
