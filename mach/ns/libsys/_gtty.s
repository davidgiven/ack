.define __gtty
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __gtty
.sect .text
__gtty:
enter[], 0
movd 12(fp),tos
movd 29704,tos
movd 8(fp),tos
jsr @__ioctl
adjspd -12
exit []
ret 0
