.define _alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _alarm
.sect .text
_alarm:
enter[], 0
movd 8(fp),tos
movd 27,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
