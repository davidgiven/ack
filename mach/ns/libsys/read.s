.define _read
.extern _read
.text
_read:
enter[], 0
save [r0,r1,r2,r3]
movd 3,r0
movd 12(fp),r1
movd 16(fp),r2
movd 0,r3
svc
movd 16(fp),r4
restore [r0,r1,r2,r3]
exit[]
ret 0
