.define _getuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _getuid
.sect .text
_getuid:
enter[], 0
movd 24,tos
jsr @.mon
adjspd -4
movd tos,r7
movd r7,r4
exit []
ret 0
