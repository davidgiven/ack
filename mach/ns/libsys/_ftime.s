.define __ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __ftime
.sect .text
__ftime:
enter[], 0
movd 8(fp),tos
movd 35,tos
jsr @.mon
exit []
ret 0
