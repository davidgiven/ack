.define _execv
.extern _execv
.text
_execv:
enter[], 0
movd @__penvp,tos
movd 12(fp),tos
movd 8(fp),tos
movd 59,tos
jsr @.mon
movd tos,r7
movd r7,@_errno
movd -1,r4
exit []
ret 0
