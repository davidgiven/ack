.define _exit
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _exit
.sect .text
_exit:
enter[], 0
jsr @__cleanup
movd 8(fp),tos
jsr @__exit
adjspd -4
exit []
ret 0
