.define _stty
.extern _stty
.text
_stty:
enter[], 0
movd 12(fp),tos
movd 29705,tos
movd 8(fp),tos
jsr @_ioctl
adjspd -12
exit []
ret 0
