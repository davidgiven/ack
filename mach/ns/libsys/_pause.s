.define __pause
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __pause
.sect .text
__pause:
enter[], 0
movd 29,tos
jsr @.mon
exit []
ret 0
