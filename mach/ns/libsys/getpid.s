.define _getpid
.extern _getpid
.text
_getpid:
enter[], 0
movd 20,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
