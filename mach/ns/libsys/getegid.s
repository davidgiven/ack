.define _getegid
.extern _getegid
.text
_getegid:
enter[], 0
movd 47,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
