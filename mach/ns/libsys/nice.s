.define _nice
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _nice
.sect .text
_nice:
enter[], 0
movd 8(fp),tos
movd 34,tos
jsr @.mon
exit []
ret 0
