.define _profil
.extern _profil
.text
_profil:
enter[], 0
addr 8(fp),r7
movd 16,r0
addd r0,r7
1:
subd 4,r7
movd 0(r7),tos
acbd -4,r0,1b
movd 44,tos
jsr @.mon
exit []
ret 0
