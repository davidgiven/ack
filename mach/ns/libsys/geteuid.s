.define _geteuid
.extern _geteuid
.text
_geteuid:
enter[], 0
movd 24,tos
jsr @.mon
movd tos,r7
movd r7,r4
exit []
ret 0
