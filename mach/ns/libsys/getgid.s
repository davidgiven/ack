.define _getgid
.extern _getgid
.text
_getgid:
enter[], 0
movd 47,tos
jsr @.mon
adjspd -4
movd tos,r7
movd r7,r4
exit []
ret 0
