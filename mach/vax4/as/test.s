.sect .text
.define _main
.define _main2
.define _xxx
! integer arithmetic and logical instructions

_main:
.data2 0x0
adawi	r0, r1
addb2	r0,r1
addb3	r0,r2,r4
addw2	r0,r1
addw3	r0,r2,r4
addl2	r0,r1
addl3	r0,r2,r4
adwc	r0, r1
ashl	r0, r1, r2
ashq	r0, r1, r2
bicb2	r0,r1
bicb3	r0,r2,r4
bicw2	r0,r1
bicw3	r0,r2,r4
bicl2	r0,r1
bicl3	r0,r2,r4
bisb2	r0,r1
bisb3	r0,r2,r4
bisw2	r0,r1
bisw3	r0,r2,r4
bisl2	r0,r1
bisl3	r0,r2,r4
bitb	r0, r1
bitw	r0, r1
bitl	r0, r1
clrb	r0
clrw	r0
clrl	r0
clrq	r0
clro	r0
cmpb	r0, r1
cmpw	r0, r1
cmpl	r0, r1
cvtbw	r0, r1
cvtbl	r0, r1
cvtwb	r0, r1
cvtwl	r0, r1
cvtlb	r0, r1
cvtlw	r0, r1
decb	r0
decw	r0
decl	r0
divb2	r0,r1
divb3	r0,r2,r4
divw2	r0,r1
divw3	r0,r2,r4
divl2	r0,r1
divl3	r0,r2,r4
ediv	r0,r1,r2,r3
emul	r0,r1,r2,r3
incb	r0
incw	r0
incl	r0
mcomb	r0, r1
mcomw	r0, r1
mcoml	r0, r1
mnegb	r0, r1
mnegw	r0, r1
mnegl	r0, r1
movb	r0, r1
movw	r0, r1
movl	r0, r1
movq	r0, r1
movo	r0, r1
movzbw	r0, r1
movzbl	r0, r1
movzwl	r0, r1
mulb2	r0,r1
mulb3	r0,r2,r4
mulw2	r0,r1
mulw3	r0,r2,r4
mull2	r0,r1
mull3	r0,r2,r4
pushl	r0
pushl	$1
rotl	r0, r1, r2
sbwc	r0, r1
subb2	r0,r1
subb3	r0,r2,r4
subw2	r0,r1
subw3	r0,r2,r4
subl2	r0,r1
subl3	r0,r2,r4
tstb	r0
tstw	r0
tstl	r0
xorb2	r0,r1
xorb3	r0,r2,r4
xorw2	r0,r1
xorw3	r0,r2,r4
xorl2	r0,r1
xorl3	r0,r2,r4

!* Address instructions */

movab	(sp),r0
movaw	(sp),r0
moval	(sp),r0
movaf	(sp),r0
movaq	(sp),r0
movad	(sp),r0
movag	(sp),r0
movah	(sp),r0
movao	(sp),r0
pushab	(sp)
pushaw	(sp)
pushal	(sp)
pushaf	(sp)
pushaq	(sp)
pushad	(sp)
pushag	(sp)
pushah	(sp)
pushao	(sp)

!* Variable length bit-field instructions */

cmpv	r0,r1,r2,r3
cmpzv	r0,r1,r2,r3
extv	r0,r1,r2,r3
extzv	r0,r1,r2,r3
ffc	r0,r1,r2,r3
ffs	r0,r1,r2,r3
insv	r0,r1,r2,r3

!* Control instructions */

acbb	r0,r1,r2,_main
acbw	r0,r1,r2,_main
acbl	r0,r1,r2,_main
acbf	r0,r1,r2,_main
acbd	r0,r1,r2,_main
acbg	r0,r1,r2,_main
acbh	r0,r1,r2,_main
aobleq	r0, r1, 1f
1:
aoblss	r0, r1, 1f
1:
bgtr	1f
1:
bleq	1f
1:
bneq	1f
1:
bnequ	1f
1:
beql	1f
1:
beqlu	1f
1:
bgeq	1f
1:
blss	1f
1:
bgtru	1f
1:
blequ	1f
1:
bvc	1f
1:
bvs	1f
1:
bgequ	1f
1:
bcc	1f
1:
blssu	1f
1:
bcs	1f
1:
bbs	r0, r1, 1f
1:
bbc	r0, r1, 1f
1:
bbss	r0, r1, 1f
1:
bbcs	r0, r1, 1f
1:
bbsc	r0, r1, 1f
1:
bbcc	r0, r1, 1f
1:
bbssi	r0, r1, 1f
1:
bbcci	r0, r1, 1f
1:
blbs	r0,1f
blbc	r0,1f
1:
jgtr	_main
jleq	_main
jneq	_main
jnequ	_main
jeql	_main
jeqlu	_main
jgeq	_main
jlss	_main
jgtru	_main
jlequ	_main
jvc	_main
jvs	_main
jgequ	_main
jcc	_main
jlssu	_main
jcs	_main
jlbs	r0, _main
jlbc	r0, _main
jbs	r0, r1, _main
jbc	r0, r1, _main
jbss	r0, r1, _main
jbcs	r0, r1, _main
jbsc	r0, r1, _main
jbcc	r0, r1, _main
jbssi	r0, r1, _main
jbcci	r0, r1, _main
br	_main
br	_main2
brb	1f
1:
brw	1f
1:
jbr	_main
jbr	_main2
bsb	1f
1:
caseb	r0, r1, r2
casew	r0, r1, r2
casel	r0, r1, r2
jmp	_main
jsb	_main
rsb
sobgeq	r0, 1f
sobgtr	r0, 1f
1:

!* Procedure call instructions */

callg	(sp),_main
calls	$2, _main
ret

!* Miscellaneous instructions */

bicpsw	r0
bispsw	r0
bpt
halt
index	r0,r1,r2,r3,r4,r5
movpsl	r0
nop
popr	r0
pushr	r0
xfc

!* Queue instructions */

insqhi	(sp), _main
insqti	(sp), _main
insque	(sp), _main
remqhi	(sp), _main
remqti	(sp), _main
remque	(sp), _main

!* Floating point instructions */

addf2	r0,r1
addf3	r0,r2,r4
addd2	r0,r1
addd3	r0,r2,r4
addg2	r0,r1
addg3	r0,r2,r4
addh2	r0,r1
addh3	r0,r2,r4
clrf	r0
clrd	r0
clrg	r0
clrh	r0
cmpf	r0,r2
cmpd	r0,r2
cmpg	r0,r2
cmph	r0,r2
cvtbf	r0,r2
cvtbd	r0,r2
cvtbg	r0,r2
cvtbh	r0,r2
cvtwf	r0,r2
cvtwd	r0,r2
cvtwg	r0,r2
cvtwh	r0,r2
cvtlf	r0,r2
cvtld	r0,r2
cvtlg	r0,r2
cvtlh	r0,r2
cvtfb	r0,r2
cvtdb	r0,r2
cvtgb	r0,r2
cvthb	r0,r2
cvtfw	r0,r2
cvtdw	r0,r2
cvtgw	r0,r2
cvthw	r0,r2
cvtfl	r0,r2
cvtdl	r0,r2
cvtgl	r0,r2
cvthl	r0,r2
cvtrfl	r0,r2
cvtrdl	r0,r2
cvtrgl	r0,r2
cvtrhl	r0,r2
cvtfd	r0,r2
cvtfg	r0,r2
cvtfh	r0,r2
cvtdf	r0,r2
cvtdh	r0,r2
cvtgf	r0,r2
cvtgh	r0,r2
cvthf	r0,r2
cvthd	r0,r2
cvthg	r0,r2
divf2	r0,r1
divf3	r0,r2,r4
divd2	r0,r1
divd3	r0,r2,r4
divg2	r0,r1
divg3	r0,r2,r4
divh2	r0,r1
divh3	r0,r2,r4
emodf	(r0),(r1),(r2),(r3),(r4)
emodd	(r0),(r1),(r2),(r3),(r4)
emodg	(r0),(r1),(r2),(r3),(r4)
emodh	(r0),(r1),(r2),(r3),(r4)
mnegf	r0,r2
mnegd	r0,r2
mnegg	r0,r2
mnegh	r0,r2
movf	r0,r2
movd	r0,r2
movg	r0,r2
movh	r0,r2
mulf2	r0,r1
mulf3	r0,r2,r4
muld2	r0,r1
muld3	r0,r2,r4
mulg2	r0,r1
mulg3	r0,r2,r4
mulh2	r0,r1
mulh3	r0,r2,r4
polyf	r0,r2,(sp)
polyd	r0,r2,(sp)
polyg	r0,r2,(sp)
polyh	r0,r2,(sp)
subf2	r0,r1
subf3	r0,r2,r4
subd2	r0,r1
subd3	r0,r2,r4
subg2	r0,r1
subg3	r0,r2,r4
subh2	r0,r1
subh3	r0,r2,r4
tstf	r0
tstd	r0
tstg	r0
tsth	r0

!* Character string instructions */

cmpc3	r0,(sp),(r1)
cmpc5	(r0),(r1),(r2),(r3),(r4)
locc	(r0),(r1),(r2)
matchc	(r0),(r1),(r2),(r3)
movc3	r0,(sp),(r1)
movc5	(r0),(r1),(r2),(r3),(r4)
movtc	(r0),(r1),(r2),(r3),(r4),(r4)
movtuc	(r0),(r1),(r2),(r3),(r4),(r4)
scanc	(r0),(r1),(r2),(r3)
skpc	r0,(sp),(r1)
spanc	(r0),(r1),(r2),(r3)

!* Cyclic redundancy check instructions */

crc	(r0),(r1),(r2),(r3)

!* Decimal string instructions */

addp4	(r0),(r1),(r2),(r3)
addp6	(r0),(r1),(r2),(r3),(r4),(r4)
ashp	(r0),(r1),(r2),(r3),(r4),(r4)
cmpp3	r0,(sp),(r1)
cmpp4	(r0),(r1),(r2),(r3)
cvtlp	(r0),(r1),(r2)
cvtpl	(r0),(r1),(r2)
cvtps	(r0),(r1),(r2),(r3)
cvtpt	(r0),(r1),(r2),(r3),(r4)
cvtsp	(r0),(r1),(r2),(r3)
cvttp	(r0),(r1),(r2),(r3),(r4)
divp	(r0),(r1),(r2),(r3),(r4),(r4)
movp	(r0),(r1),(r2)
mulp	(r0),(r1),(r2),(r3),(r4),(r4)
subp4	(r0),(r1),(r2),(r3)
subp6	(r0),(r1),(r2),(r3),(r4),(r4)

!* Edit instruction */

editpc	(r0),(r1),(r2),(r3)

!* Other VAX-11 instructions */

bugw	_main
bugl	_main

prober	(r0),(r1),(r2)
probew	(r0),(r1),(r2)

rei
chmk	r0
chme	r0
chms	r0
chmu	r0

ldpctx
svpctx
mtpr	r0,r1
mfpr	r0,r1

! test some addressing modes
_main2:
.data2 0x0
pushal	_xxx
pushal	_main
pushal	_main2
pushal	*_xxx
pushal	*_main
pushal	*_main2
movl	$_xxx,-(sp)
movl	$_main2,-(sp)
movl	*$_main2,-(sp)
movl	(sp)+,r9
movl	*(sp)+,r9
movl	(r0),r9
movl	12(r0),r9
movl	*12(r0),r9
movl	_xxx(r0),r9
movl	*_xxx(r0),r9
movl	*_xxx(r0)[r1],r9
movb	*_xxx(r0)[r1],r9
movw	*_xxx(r0)[r1],r9


.sect .rom
_xxx:
	.data2 0
