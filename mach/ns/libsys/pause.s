.define _pause
.extern _pause
.text
_pause:
enter[], 0
movd 29,tos
jsr @.mon
exit []
ret 0
