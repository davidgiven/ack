.define _gtty
.extern _gtty
.text
_gtty:
enter[], 0
movd 12(fp),tos
movd 29704,tos
movd 8(fp),tos
jsr @_ioctl
adjspd -12
exit []
ret 0
