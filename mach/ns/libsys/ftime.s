.define _ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _ftime
.sect .text
_ftime:
enter[], 0
movd 8(fp),tos
movd 35,tos
jsr @.mon
exit []
ret 0
