.sect .text;.sect .rom;.sect .data;.sect .bss
.define _printn
.define _printf
.extern _printf
.sect .text
_putchar:
move.l	#1,-(sp)
pea	9(sp)
move.l	#1,-(sp)
jsr	_write
add.l	#12,sp
rts
_printf:
tst.b -56(sp)
link	a6,#-16
!Local -4 into d7
!Local -12 into d6
!Local -8 into a5
!Local 8 into a4
!Local -16 into a3
movem.l d7/d6/a5/a4/a3,-(sp)
move.l 8(a6),a4
lea 12(a6),a1
move.l a1, a5
I0014:
move.l a4, a0
add.l #1,a4
clr.l d0
move.b (a0),d0
move.l d0, d7
cmp.l #37,d7
beq I0015
tst.l d7
beq I0012
move.l d7,-(sp)
jsr _putchar
add #4,sp
jmp I0014
I0015:
move.l a4, a0
add.l #1,a4
clr.l d0
move.b (a0),d0
move.l d0, d7
cmp.l #100,d7
beq I0018
cmp.l #117,d7
bne I0017
I0018:
move.l a5, a0
add.l #4,a5
move.l (a0), d6
cmp.l #100,d7
bne I0019
tst.l d6
bge I0019
clr.l d2
sub.l d6,d2
move.l d2, d6
pea 45
jsr _putchar
add #4,sp
I0019:
move.l d6,-(sp)
jsr _printn
add #4,sp
jmp I0014
I0017:
cmp.l #115,d7
bne I0014
move.l a5, a0
add.l #4,a5
move.l (a0), a3
I001c:
move.l a3, a0
add.l #1,a3
clr.l d0
move.b (a0),d0
move.l d0, d7
tst.l d7
beq I0014
move.l d7,-(sp)
jsr _putchar
add #4,sp
jmp I001c
I0012:
movem.l (sp)+,d7/d6/a5/a4/a3
unlk a6
rts
.extern _printn
_printn:
tst.b -44(sp)
link	a6,#-4
!Local -4 into d7
move.l d7,-(sp)
.sect .data
_14:
.data4	808530483
.data4	875902519
.data4	943259648
.sect .text
move.l 8(a6),-(sp)
pea 10
jsr .dvu
move.l d1, d7
tst.l d7
beq I0023
move.l d7,-(sp)
jsr _printn
add #4,sp
I0023:
pea _14
move.l 8(a6),-(sp)
pea 10
jsr .dvu
move.l (sp)+,a1
add.l d2,a1
clr.l d0
move.b (a1),d0
move.l d0,-(sp)
jsr _putchar
add #4,sp
move.l (sp)+,d7
unlk a6
rts
