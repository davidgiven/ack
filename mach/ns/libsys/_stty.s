.define __stty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __stty
.sect .text
__stty:
enter[], 0
movd 12(fp),tos
movd 29705,tos
movd 8(fp),tos
jsr @__ioctl
adjspd -12
exit []
ret 0
