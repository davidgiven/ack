.define __alarm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __alarm
.sect .text
__alarm:
enter[], 0
movd 8(fp),tos
movd 27,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
