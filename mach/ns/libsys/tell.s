.define _tell
.extern _tell
.text
_tell:
enter[], 0
movd 1,tos
movd 0,tos
movd 8(fp),tos
jsr @_lseek
adjspd -12
exit []
ret 0
