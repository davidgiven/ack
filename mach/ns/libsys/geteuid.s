.define _geteuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _geteuid
.sect .text
_geteuid:
enter[], 0
movd 24,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
