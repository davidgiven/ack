.define _getuid
.extern _getuid
.text
_getuid:
enter[], 0
movd 24,tos
jsr @.mon
adjspd -4
movd tos,r7
movd r7,r4
exit []
ret 0
