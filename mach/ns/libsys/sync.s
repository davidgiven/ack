.define _sync
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _sync
.sect .text
_sync:
enter[], 0
movd 36,tos
jsr @.mon
exit []
ret 0
